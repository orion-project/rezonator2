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
#include <QTimer>

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

GaussCalcParamEditor::GaussCalcParamEditor(Z::Parameter *param,
                                           GaussCalculator *calc,
                                           GaussCalcGetValueFunc getValue,
                                           GaussCalcSetValueFunc setValue,
                                           bool invertedUnit)
    : QObject(),
      _calc(calc),
      _param(param),
      _getValueFromCalculator(getValue),
      _setValueToCalculator(setValue),
      _invertedUnit(invertedUnit)
{
    _editor = new ParamEditor(ParamEditor::Options(_param));
    connect(_editor, SIGNAL(valueEdited(double)), this, SLOT(paramEdited()));
    connect(_editor, SIGNAL(unitChanged(Z::Unit)), this, SLOT(paramEdited()));
}

GaussCalcParamEditor::~GaussCalcParamEditor()
{
    delete _editor;
    delete _param;
}

void GaussCalcParamEditor::populate()
{
    double valueSi = _getValueFromCalculator(_calc);
    Z::Unit unit = _param->value().unit();
    double value = _invertedUnit
            ? unit->toSi(valueSi)
            : unit->fromSi(valueSi);
    _param->setValue(Z::Value(value, unit));
}

void GaussCalcParamEditor::paramEdited()
{
    _editor->apply();
    const Z::Value& v = _param->value();
    double valueSi = _invertedUnit
            ? v.unit()->fromSi(v.value())
            : v.unit()->toSi(v.value());
    _setValueToCalculator(_calc, valueSi);
    emit calcNeeded();
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
    setWindowTitle(tr("Gaussian Beam Calculator"));
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

    restoreState();
    Ori::Wnd::moveToScreenCenter(this);

    QTimer::singleShot(0, [this]{ this->recalc(); });
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

void GaussCalculatorWindow::recalc()
{
    _calc.calc();

    for (auto p : _params)
        p->populate();

    updatePlot();
}

void GaussCalculatorWindow::makeParams()
{
    #define FUNC(func_name) std::mem_fun(&GaussCalculator::func_name)

    auto addParam = [](Z::Dim dim, const QString& alias, const QString& name, const Z::Value& initialValue) {
        auto param = new Z::Parameter(dim, alias, name);
        param->setValue(initialValue);
        return param;
    };

    auto addEditor = [&](int row, int col, GaussCalcParamEditor *editor) {
        _params.append(editor);
        _paramsLayout->addWidget(editor->editor(), row, col);
        connect(editor, &GaussCalcParamEditor::calcNeeded, this, &GaussCalculatorWindow::recalc);
    };

    using namespace Z::Dims;
    _lambda = addParam(linear(), QStringLiteral("lambda"), QStringLiteral("λ"), 980_nm);
    _MI = addParam(none(), QStringLiteral("MI"), QStringLiteral("M²"), 1);
    _w0 = addParam(linear(), QStringLiteral("w0"), QStringLiteral("ω<sub>0</sub>"), 100_mkm);
    _z = addParam(linear(), QStringLiteral("z"), QStringLiteral("z"), 100_mm);
    _z0 = addParam(linear(), QStringLiteral("z0"), QStringLiteral("z<sub>0</sub>"), 0_mm);
    _Vs = addParam(angular(), QStringLiteral("Vs"), QStringLiteral("V<sub>s</sub>"), 0_deg);
    _w = addParam(linear(), QStringLiteral("w"), QStringLiteral("ω"), 0_mkm);
    _R = addParam(linear(), QStringLiteral("R"), QStringLiteral("R"), 0_mm);
    _reQ = addParam(linear(), QStringLiteral("q_re"), QStringLiteral("re(q)"), 0_mm);
    _imQ = addParam(linear(), QStringLiteral("q_im"), QStringLiteral("im(q)"), 0_mm);
    _reQ1 = addParam(linear(), QStringLiteral("q1_re"), QStringLiteral("re(1/q)"), 0_mm);
    _imQ1 = addParam(linear(), QStringLiteral("q1_im"), QStringLiteral("im(1/q)"), 0_mm);

    GaussCalculator *c = &_calc;
    addEditor(0, 0, new GaussCalcParamEditor(_lambda, c, FUNC(lambda), FUNC(setLambda)));
    addEditor(1, 0, new GaussCalcParamEditor(_MI, c, FUNC(MI), FUNC(setMI)));
    addEditor(2, 0, new GaussCalcParamEditor(_w0, c, FUNC(w0), FUNC(setW0)));
    addEditor(3, 0, new GaussCalcParamEditor(_z, c, FUNC(z), FUNC(setZ)));
    addEditor(0, 1, new GaussCalcParamEditor(_z0, c, FUNC(z0), FUNC(setZ0)));
    addEditor(1, 1, new GaussCalcParamEditor(_Vs, c, FUNC(Vs), FUNC(setVs)));
    addEditor(2, 1, new GaussCalcParamEditor(_w, c, FUNC(w), FUNC(setW)));
    addEditor(3, 1, new GaussCalcParamEditor(_R, c, FUNC(R), FUNC(setR)));
    addEditor(0, 2, new GaussCalcParamEditor(_reQ, c, FUNC(reQ), FUNC(setReQ)));
    addEditor(1, 2, new GaussCalcParamEditor(_imQ, c, FUNC(imQ), FUNC(setImQ)));
    addEditor(2, 2, new GaussCalcParamEditor(_reQ1, c, FUNC(reQ1), FUNC(setReQ1), true));
    addEditor(3, 2, new GaussCalcParamEditor(_imQ1, c, FUNC(imQ1), FUNC(setImQ1), true));

    #undef FUNC
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
    qDebug() << ((_calc.zone() == GaussCalculator::Zone::Near)? "Zone: Near": "Zone: Far");
}

void GaussCalculatorWindow::lockSelected(int lock)
{
    _calc.setLock(GaussCalculator::Lock(lock));
    qDebug() << ((_calc.lock() == GaussCalculator::Lock::Waist)? "Lock: Waist": "Lock: Front");
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















