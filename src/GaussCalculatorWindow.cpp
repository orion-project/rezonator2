#include "GaussCalculatorWindow.h"

#include "widgets/Plot.h"
#include "widgets/ParamEditor.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWindows.h"
#include "tools/OriSettings.h"
#include "widgets/OriActions.h"
#include "widgets/OriFlatToolBar.h"

#include <QGridLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QIcon>

/*void BeamPlotter::calculate()
{
    valuesZ.clear();
    valuesW.clear();
    int points = 50;
    double step = z_max / double(points);
    for (double z = 0; z <= z_max; z += step)
    {
        double z0 = M_PI * w0*w0 / lambda;
        double w = sqrt(w0*w0 * (1 + z*z/z0/z0));
        valuesZ.append(units->si2linear(z));
        valuesW.append(units->si2beamsize(w));
    }
}*/

//--------------------------------------------------------------------------------
//                              GaussCalculatorParam
//--------------------------------------------------------------------------------

GaussCalcParam::GaussCalcParam(GaussCalculator *calc,
        Z::Dim dim, const QString& alias, const QString& name,
        GaussCalcSetValueFunc setValue, GaussCalcGetValueFunc getValue)
    : QObject(), _calc(calc), _setValueToCalculator(setValue), _getValueFromCalculator(getValue)
{
    _param = new Z::Parameter(dim, alias, name);
    _editor = new ParamEditor(ParamEditor::Options(_param));
    connect(_editor, SIGNAL(valueEdited(double)), this, SLOT(paramEdited()));
    connect(_editor, SIGNAL(unitChanged(Z::Unit)), this, SLOT(paramEdited()));
}

GaussCalcParam::~GaussCalcParam()
{
    delete _editor;
    delete _param;
}

void GaussCalcParam::paramEdited()
{
    _editor->apply();
    _setValueToCalculator(_calc, _param->value().toSi());
    emit calcNeeded();
}

void GaussCalcParam::populate()
{
    auto unit = _param->value().unit();
    double value = unit->fromSi(_getValueFromCalculator(_calc));
    _param->setValue(Z::Value(value, unit));
}

//--------------------------------------------------------------------------------
//                              GaussCalculatorWindow
//--------------------------------------------------------------------------------

void GaussCalculatorWindow::showCalcWindow()
{
    (new GaussCalculatorWindow)->show();
}

GaussCalculatorWindow::GaussCalculatorWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Gauss Beam Calculator"));
    setWindowIcon(QIcon(":/window_icons/gauss_calc"));

    _paramsLayout = new QGridLayout;
    _paramsLayout->setMargin(12);
    _paramsLayout->setHorizontalSpacing(12);
    _paramsLayout->setVerticalSpacing(0);

    makeParams();

    Ori::Layouts::LayoutV({
            makeToolbar(),
            makePlot(),
            _paramsLayout,
        })
        .setMargin(0)
        .setSpacing(0)
        .useFor(this);
/*
    _editorLambda->setValue(_units.si2lambda(_calc.lambda()));

    _calc.calc();

    populateUnitLabels();
    populateValues();
    updatePlot();*/

    restoreState();
    Ori::Wnd::moveToScreenCenter(this);
}

GaussCalculatorWindow::~GaussCalculatorWindow()
{
    storeState();
    qDeleteAll(_params);
}

QString GaussCalculatorWindow::stateFileName()
{
    Ori::Settings s;
    return s.settings()->fileName().section('.', 0, -2) + ".gauss.json";
}

void GaussCalculatorWindow::restoreState()
{
    QJsonObject root;

    auto fileName = stateFileName();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        qWarning() << "GaussCalculatorWindow: failed to load state" << fileName << file.errorString();
    else
        root = QJsonDocument::fromJson(file.readAll()).object();

    int w = root["window_width"].toInt();
    int h = root["window_height"].toInt();
    if (w == 0 || h == 0) w = 750, h = 400;
    resize(w, h);
}

void GaussCalculatorWindow::storeState()
{
    QJsonObject root;
    root["window_width"] = width();
    root["window_height"] = height();

    auto fileName = stateFileName();
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << "GaussCalculatorWindow: failed to save state" << fileName << file.errorString();
        return;
    }
    QTextStream(&file) << QJsonDocument(root).toJson();
}

void GaussCalculatorWindow::addParam(int row, int col, GaussCalcParam *param)
{
    _params.append(param);
    _paramsLayout->addWidget(param->editor(), row, col);
    connect(param, &GaussCalcParam::calcNeeded, this, &GaussCalculatorWindow::recalc);
}

void GaussCalculatorWindow::recalc()
{
    qDebug() << "CALC" << _calc.lambda();

    _calc.calc();

    for (auto p : _params)
        p->populate();
}

void GaussCalculatorWindow::makeParams()
{
#define FUNCS(toCalc, fromCalc)\
    std::mem_fun(&GaussCalculator::toCalc), std::mem_fun(&GaussCalculator::fromCalc)

    addParam(0, 0, new GaussCalcParam(&_calc,
        Z::Dims::linear(), QStringLiteral("lambda"), QStringLiteral("λ"),
        FUNCS(setLambda, lambda)));

    addParam(1, 0, new GaussCalcParam(&_calc,
        Z::Dims::none(), QStringLiteral("MI"), QStringLiteral("M²"),
        FUNCS(setM2, M2)));

    addParam(2, 0, new GaussCalcParam(&_calc,
        Z::Dims::linear(), QStringLiteral("w0"), QStringLiteral("ω<sub>0</sub>"),
        FUNCS(setW0, w0)));

    addParam(3, 0, new GaussCalcParam(&_calc,
        Z::Dims::linear(), QStringLiteral("z"), QStringLiteral("z"),
        FUNCS(setZ, z)));


    addParam(0, 1, new GaussCalcParam(&_calc,
        Z::Dims::linear(), QStringLiteral("z0"), QStringLiteral("z<sub>0</sub>"),
        FUNCS(setZ0, z0)));

    addParam(1, 1, new GaussCalcParam(&_calc,
        Z::Dims::angular(), QStringLiteral("Vs"), QStringLiteral("V<sub>s</sub>"),
        FUNCS(setVs, Vs)));

    addParam(2, 1, new GaussCalcParam(&_calc,
        Z::Dims::linear(), QStringLiteral("w"), QStringLiteral("ω"),
        FUNCS(setW, w)));

    addParam(3, 1, new GaussCalcParam(&_calc,
        Z::Dims::linear(), QStringLiteral("R"), QStringLiteral("R"),
        FUNCS(setR, R)));


    addParam(0, 2, new GaussCalcParam(&_calc,
        Z::Dims::linear(), QStringLiteral("q_re"), QStringLiteral("Re(q)"),
        FUNCS(setReQ, reQ)));

    addParam(1, 2, new GaussCalcParam(&_calc,
        Z::Dims::linear(), QStringLiteral("q_im"), QStringLiteral("Im(q)"),
        FUNCS(setImQ, imQ)));

    addParam(2, 2, new GaussCalcParam(&_calc,
        Z::Dims::linear(), QStringLiteral("q1_re"), QStringLiteral("Re(q<sup>-1</sup>)"),
        FUNCS(setReQ1, reQ1)));

    addParam(3, 2, new GaussCalcParam(&_calc,
        Z::Dims::linear(), QStringLiteral("q1_im"), QStringLiteral("Im(q<sup>-1</sup>)"),
        FUNCS(setImQ1, imQ1)));

#undef FUNCS
}

QWidget* GaussCalculatorWindow::makeToolbar()
{
    auto locks = new Ori::Widgets::ExclusiveActionGroup(this);
    locks->add(int(GaussCalculator::Lock::Waist), ":/toolbar/gauss_lock_waist", tr("Lock waist"));
    locks->add(int(GaussCalculator::Lock::Front), ":/toolbar/gauss_lock_front", tr("Lock front"));
    locks->setCheckedId(int(_calc.lock()));
    connect(locks, SIGNAL(checked(int)), this, SLOT(lockSelected(int)));

    auto zones = new Ori::Widgets::ExclusiveActionGroup(this);
    zones->add(int(GaussCalculator::Zone::Near), ":/toolbar/gauss_near_zone", tr("Use near-field zone"));
    zones->add(int(GaussCalculator::Zone::Far), ":/toolbar/gauss_far_zone", tr("Use far-field zone"));
    zones->setCheckedId(int(_calc.zone()));
    connect(zones, SIGNAL(checked(int)), this, SLOT(zoneSelected(int)));

    _plotPlusMinusZ = new Ori::Widgets::ExclusiveActionGroup(this);
    _plotPlusMinusZ->add(false, ":/toolbar/gauss_pos_z", tr("Plot 0 .. z"));
    _plotPlusMinusZ->add(true, ":/toolbar/gauss_full_z", tr("Plot -z .. z"));
    connect(_plotPlusMinusZ, SIGNAL(checked(int)), this, SLOT(updatePlot()));

    _plotPlusMinusW = new Ori::Widgets::ExclusiveActionGroup(this);
    _plotPlusMinusW->add(false, ":/toolbar/gauss_pos_w", tr("Plot 0 .. w"));
    _plotPlusMinusW->add(true, ":/toolbar/gauss_full_w", tr("Plot -w .. w"));
    connect(_plotPlusMinusW, SIGNAL(checked(int)), this, SLOT(updatePlot()));

    auto actionHelp = new QAction(QIcon(":/toolbar/help"), tr("Help"), this);

    auto toolbar = new Ori::Widgets::FlatToolBar;
    toolbar->addActions(locks->actions());
    toolbar->addSeparator();
    toolbar->addActions(zones->actions());
    toolbar->addSeparator();
    toolbar->addActions(_plotPlusMinusZ->actions());
    toolbar->addActions(_plotPlusMinusW->actions());
    toolbar->addSeparator();
    toolbar->addAction(actionHelp);
    return toolbar;
}

QWidget* GaussCalculatorWindow::makePlot()
{
    _plot = new Plot;
    _plot->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    _graphPlusW = _plot->addGraph();
    _graphMinusW = _plot->addGraph();
    return _plot;
}

void GaussCalculatorWindow::zoneSelected(int zone)
{
    _calc.setZone(GaussCalculator::Zone(zone));
    //qDebug() << ((_calc.zone() == BeamCalculator::Zone::Near)? "Zone: Near": "Zone: Far");
}

void GaussCalculatorWindow::lockSelected(int lock)
{
    _calc.setLock(GaussCalculator::Lock(lock));
    //qDebug() << ((_calc.lock() == BeamCalculator::Lock::Waist)? "Lock: Waist": "Lock: Front");
}

void GaussCalculatorWindow::updatePlot()
{/*
    _plotter.lambda = _units.lambda2si(_editorLambda->value());
    _plotter.w0 = _units.beamsize2si(_editorW0->value());
    _plotter.z_max = _units.linear2si(_editorZ->value());
    _plotter.M2 = _editorM2->value();
    _plotter.calculate();

    bool plotMinusZ = _plotPlusMinusZ->checkedId();
    bool plotMinusW = _plotPlusMinusW->checkedId();

    _graphPlusW->clearData();
    if (plotMinusZ)
        for (int i = _plotter.valuesZ.size()-1; i >= 0; i--)
            _graphPlusW->addData(-_plotter.valuesZ.at(i), _plotter.valuesW.at(i));
    _graphPlusW->addData(_plotter.valuesZ, _plotter.valuesW);
    _graphPlusW->rescaleAxes(false);

    _graphMinusW->clearData();
    _graphMinusW->setVisible(plotMinusW);
    if (plotMinusW)
    {
        if (plotMinusZ)
            for (int i = _plotter.valuesZ.size()-1; i >= 0; i--)
                _graphMinusW->addData(-_plotter.valuesZ.at(i), -_plotter.valuesW.at(i));
        for (int i = 0; i < _plotter.valuesZ.size(); i++)
            _graphMinusW->addData(_plotter.valuesZ.at(i), -_plotter.valuesW.at(i));
        _graphMinusW->rescaleAxes(true);
    }
    _plot->replot();
*/}















