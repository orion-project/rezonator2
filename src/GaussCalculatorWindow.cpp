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

void GaussPlotter::calculate()
{
    valuesZ.clear();
    valuesW.clear();
    if (qAbs(maxZ-0) <= std::numeric_limits<double>::epsilon())
        return;
    const double z0 = M_PI * w0*w0 / lambda;
    const double sqrZ0 = z0 * z0;
    const double sqrW0 = w0 * w0;
    const double step = maxZ / double(points);
    double z = 0;
    while (z <= maxZ)
    {
        double w = sqrt(sqrW0 * (1 + z*z / sqrZ0));
        valuesZ.append(unitZ->fromSi(z));
        valuesW.append(unitW->fromSi(w));
        z += step;
    }
    if (valuesZ.last() < unitZ->fromSi(maxZ))
    {
        z = maxZ;
        double w = sqrt(sqrW0 * (1 + z*z / sqrZ0));
        valuesZ.append(unitZ->fromSi(z));
        valuesW.append(unitW->fromSi(w));
    }
}

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

    _plotPlusMinusZ->setCheckedId(root["plot_minus_z"].toInt());
    _plotPlusMinusW->setCheckedId(root["plot_minus_w"].toInt());
    _calcModeLock->setCheckedId(root["lock_mode"].toInt());
    _calcModeZone->setCheckedId(root["zone_mode"].toInt());
    _calc.setLock(GaussCalculator::Lock(root["lock_mode"].toInt()));
    _calc.setZone(GaussCalculator::Zone(root["zone_mode"].toInt()));
}

void GaussCalculatorWindow::storeState()
{
    QJsonObject root;
    root["window_width"] = width();
    root["window_height"] = height();
    root["plot_minus_z"] = _plotPlusMinusZ->checkedId();
    root["plot_minus_w"] = _plotPlusMinusW->checkedId();
    root["lock_mode"] = int(_calc.lock());
    root["zone_mode"] = int(_calc.zone());

    auto fileName = stateFileName();
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << "GaussCalculatorWindow: failed to save state" << fileName << file.errorString();
        return;
    }
    QTextStream(&file) << QJsonDocument(root).toJson();
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
    _calcModeLock = new Ori::Widgets::ExclusiveActionGroup(this);
    _calcModeLock->add(int(GaussCalculator::Lock::Waist), ":/toolbar/gauss_lock_waist", tr("Lock waist"));
    _calcModeLock->add(int(GaussCalculator::Lock::Front), ":/toolbar/gauss_lock_front", tr("Lock front"));
    _calcModeLock->setCheckedId(int(_calc.lock()));
    connect(_calcModeLock, SIGNAL(checked(int)), this, SLOT(lockSelected(int)));

    _calcModeZone = new Ori::Widgets::ExclusiveActionGroup(this);
    _calcModeZone->add(int(GaussCalculator::Zone::Near), ":/toolbar/gauss_near_zone", tr("Use near-field zone"));
    _calcModeZone->add(int(GaussCalculator::Zone::Far), ":/toolbar/gauss_far_zone", tr("Use far-field zone"));
    _calcModeZone->setCheckedId(int(_calc.zone()));
    connect(_calcModeZone, SIGNAL(checked(int)), this, SLOT(zoneSelected(int)));

    _plotPlusMinusZ = new Ori::Widgets::ExclusiveActionGroup(this);
    _plotPlusMinusZ->add(false, ":/toolbar/gauss_pos_z", tr("Plot 0 .. z"));
    _plotPlusMinusZ->add(true, ":/toolbar/gauss_full_z", tr("Plot -z .. z"));
    connect(_plotPlusMinusZ, SIGNAL(checked(int)), this, SLOT(updatePlot()));

    _plotPlusMinusW = new Ori::Widgets::ExclusiveActionGroup(this);
    _plotPlusMinusW->add(false, ":/toolbar/gauss_pos_w", tr("Plot 0 .. w"));
    _plotPlusMinusW->add(true, ":/toolbar/gauss_full_w", tr("Plot -w .. w"));
    connect(_plotPlusMinusW, SIGNAL(checked(int)), this, SLOT(updatePlot()));

    auto actionHelp = new QAction(QIcon(":/toolbar/help"), tr("Help"), this);
    actionHelp->setEnabled(false); // TODO:NEXT-VER

    auto toolbar = new Ori::Widgets::FlatToolBar;
    toolbar->addActions(_calcModeLock->actions());
    toolbar->addSeparator();
    toolbar->addActions(_calcModeZone->actions());
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
    _plot->legend->setVisible(false);
    _plot->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    _plot->xAxis->setNumberPrecision(6);
    _plot->yAxis->setNumberPrecision(6);

    _graphAngle1 = _plot->addGraph();
    _graphAngle2 = _plot->addGraph();
    _graphAngle3 = _plot->addGraph();
    _graphAngle4 = _plot->addGraph();

    _graphPlusW = _plot->addGraph();
    _graphMinusW = _plot->addGraph();

    QPen anglePen(Qt::gray, 1, Qt::DashLine);
    _graphAngle1->setPen(anglePen);
    _graphAngle2->setPen(anglePen);
    _graphAngle3->setPen(anglePen);
    _graphAngle4->setPen(anglePen);

    return _plot;
}

void GaussCalculatorWindow::zoneSelected(int zone)
{
    _calc.setZone(GaussCalculator::Zone(zone));
}

void GaussCalculatorWindow::lockSelected(int lock)
{
    _calc.setLock(GaussCalculator::Lock(lock));
}

void GaussCalculatorWindow::recalc()
{
    _calc.calc();

    for (auto p : _params)
        p->populate();

    updatePlot();
}

void GaussCalculatorWindow::updatePlot()
{
    _plotter.lambda = _calc.lambda();
    _plotter.w0 = _calc.w0();
    _plotter.maxZ = _calc.z();
    _plotter.MI = _calc.MI();
    _plotter.unitW = _w->value().unit();
    _plotter.unitZ = _z->value().unit();
    _plotter.calculate();

    bool plotMinusZ = _plotPlusMinusZ->checkedId();
    bool plotMinusW = _plotPlusMinusW->checkedId();

    _graphPlusW->clearData();
    _graphMinusW->clearData();
    _graphAngle1->clearData();
    _graphAngle2->clearData();
    _graphAngle3->clearData();
    _graphAngle4->clearData();
    _graphMinusW->setVisible(plotMinusW);

    auto addData = [&](QCPGraph *g, double factorZ, double factorW) {
        for (int i = 0; i < _plotter.valuesZ.size(); i++)
            g->addData(factorZ * _plotter.valuesZ.at(i), factorW * _plotter.valuesW.at(i));
    };

    if (plotMinusZ)
        addData(_graphPlusW, -1, 1);
    addData(_graphPlusW, 1, 1);
    if (plotMinusW)
    {
        if (plotMinusZ)
            addData(_graphMinusW, -1, -1);
        addData(_graphMinusW, 1, -1);
    }

    double anglePointZ = _plotter.unitZ->fromSi(_plotter.maxZ);
    double anglePointW = _plotter.unitW->fromSi(_plotter.maxZ * tan(_calc.Vs()));
    _graphAngle1->addData(0, 0);
    _graphAngle1->addData(anglePointZ, anglePointW);
    if (plotMinusZ)
    {
        _graphAngle2->addData(0, 0);
        _graphAngle2->addData(-anglePointZ, anglePointW);
    }
    if (plotMinusZ && plotMinusW)
    {
        _graphAngle3->addData(0, 0);
        _graphAngle3->addData(-anglePointZ, -anglePointW);
    }
    if (plotMinusW)
    {
        _graphAngle4->addData(0, 0);
        _graphAngle4->addData(anglePointZ, -anglePointW);
    }

    double maxZ = _z->value().value();
    double minZ = plotMinusZ ? -maxZ : 0;
    double maxW = _w->value().value() * 1.1;
    double minW = plotMinusW ? -maxW : 0;

    _plot->setLimitsX(minZ, maxZ, false);
    _plot->setLimitsY(minW, maxW, false);
    _plot->replot();
}
