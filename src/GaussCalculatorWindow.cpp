#include "GaussCalculatorWindow.h"

#include "AppSettings.h"
#include "CalculatorWindow.h"
#include "CustomPrefs.h"
#include "HelpSystem.h"
#include "funcs/GaussCalculator.h"
#include "widgets/ParamEditor.h"
#include "widgets/PlotHelpers.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWindows.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriActions.h"
#include "widgets/OriFlatToolBar.h"

#include "qcpl_plot.h"

#include <QGridLayout>
#include <QJsonObject>
#include <QIcon>
#include <QTimer>

namespace {

enum PlotMode { PLOT_W, PLOT_R };

GaussCalculatorWindow* __instance = nullptr;

} // namespace

const int points = 50;

class GaussPlotter
{
public:
    double lambda = std::numeric_limits<double>::quiet_NaN();
    double w0 = std::numeric_limits<double>::quiet_NaN();
    double maxZ = std::numeric_limits<double>::quiet_NaN();
    double MI = std::numeric_limits<double>::quiet_NaN();
    Z::Unit unitZ = nullptr;
    Z::Unit unitY = nullptr;

    QVector<double> valuesZ;
    QVector<double> valuesMain;
    QVector<double> valuesGauss;
    QVector<double> valuesEquiv;

    void cleanData()
    {
        valuesZ.clear();
        valuesMain.clear();
        valuesGauss.clear();
        valuesEquiv.clear();
    }

    bool isHyper() const
    {
        return qAbs(MI - 1.0) > std::numeric_limits<double>::epsilon();
    }

    void calculateW()
    {
        const bool is_hyper = isHyper();
        const double sqrW0 = w0 * w0;
        const double sqrW0_equiv = sqrW0 / MI;
        const double z0_gauss = M_PI * sqrW0 / lambda;
        const double z0_hyper = M_PI * sqrW0_equiv / lambda;
        const double sqrZ0_gauss = z0_gauss * z0_gauss;
        const double sqrZ0_hyper = z0_hyper * z0_hyper;
        const double step = maxZ / double(points);
        double z = 0;

        auto calcPoint = [&](){
            double w_gauss = sqrt(sqrW0 * (1 + z*z / sqrZ0_gauss));
            if (is_hyper)
            {
                double w_hyper = sqrt(sqrW0 * (1 + z*z / sqrZ0_hyper));
                double w_equiv = sqrt(sqrW0_equiv * (1 + z*z / sqrZ0_hyper));
                valuesEquiv.append(unitY->fromSi(w_equiv));
                valuesMain.append(unitY->fromSi(w_hyper));
                valuesGauss.append(unitY->fromSi(w_gauss));
            }
            else
                valuesMain.append(unitY->fromSi(w_gauss));
            valuesZ.append(unitZ->fromSi(z));
        };

        while (z <= maxZ)
        {
            calcPoint();
            z += step;
        }
        if (valuesZ.last() < unitZ->fromSi(maxZ))
        {
            z = maxZ;
            calcPoint();
        }
    }

    void calculateR()
    {
        const bool is_hyper = isHyper();
        const double z0_gauss = M_PI * w0*w0 / lambda;
        const double z0_hyper = z0_gauss / MI;
        const double sqrZ0_gauss = z0_gauss * z0_gauss;
        const double sqrZ0_hyper = z0_hyper * z0_hyper;
        double z;

        auto calcPoint = [&](){
            double R_gauss = z * (1.0 + sqrZ0_gauss / (z*z));
            if (is_hyper)
            {
                double R_hyper = z * (1 + sqrZ0_hyper / (z*z));
                valuesMain.append(unitY->fromSi(R_hyper));
                valuesGauss.append(unitY->fromSi(R_gauss));
            }
            else
                valuesMain.append(unitY->fromSi(R_gauss));
            valuesZ.append(unitZ->fromSi(z));
        };

        // Wavefront ROC is changed faster than beam radius
        // so we should calc additional points to make graph more smooth.
        double step = z0_gauss / double(2*points);
        z = step;
        while (z <= z0_gauss)
        {
            calcPoint();
            z += step;
        }
        z = z0_gauss;
        calcPoint();
        step = (maxZ - z0_gauss) / double(points);
        while (z <= maxZ)
        {
            z += step;
            calcPoint();
        }
        if (valuesZ.last() < unitZ->fromSi(maxZ))
        {
            z = maxZ;
            calcPoint();
        }
    }
};

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
    (_calc->*_setValueToCalculator)(paramValueSI());
    connect(_editor, SIGNAL(valueEdited(double)), this, SLOT(paramEdited()));
    connect(_editor, SIGNAL(unitChanged(Z::Unit)), this, SLOT(paramEdited()));
}

GaussCalcParamEditor::~GaussCalcParamEditor()
{
    delete _editor;
    delete _param;
}

void GaussCalcParamEditor::getValueFromCalculator()
{
    double valueSi = (_calc->*_getValueFromCalculator)();
    Z::Unit unit = _param->value().unit();
    double value = _invertedUnit
            ? unit->toSi(valueSi)
            : unit->fromSi(valueSi);
    _param->setValue(Z::Value(value, unit));
}

void GaussCalcParamEditor::paramEdited()
{
    _editor->apply();
    (_calc->*_setValueToCalculator)(paramValueSI());
    emit calcNeeded();
}

double GaussCalcParamEditor::paramValueSI() const
{
    const Z::Value& v = _param->value();
    return _invertedUnit
            ? v.unit()->fromSi(v.value())
            : v.unit()->toSi(v.value());
}

//--------------------------------------------------------------------------------
//                              GaussGraphsContainer
//--------------------------------------------------------------------------------

class GaussGraphsContainer
{
public:
    void addItems(QCPL::Plot* plot, int count, const QPen& pen, const QBrush& brush)
    {
        for (int i = 0; i < count; i++)
        {
            QCPGraph *g = plot->addGraph();
            g->setPen(pen);
            g->setBrush(brush);
            g->selectionDecorator()->setBrush(brush);
            _items.append(g);
        }
    }

    void clearData()
    {
        foreach (QCPGraph *g, _items)
            g->data()->clear();
    }

    QCPGraph* operator[](int index) { return _items[index]; }

private:
    QVector<QCPGraph*> _items;
};

//--------------------------------------------------------------------------------
//                                GaussPlotOptions
//--------------------------------------------------------------------------------

struct GaussPlotOptions
{
    bool plotMinusZ;
    bool plotMinusY;
    bool plotW;
    bool plotR;
    bool plotV;
    bool plotZ0;
};

//--------------------------------------------------------------------------------
//                                 GaussGraphV
//--------------------------------------------------------------------------------

class GaussGraphZ0
{
public:
    GaussGraphZ0(QCPL::Plot* plot, GaussPlotter *plotter, GaussCalculator *calc) : _plotter(plotter), _calc(calc)
    {
        QPen penMain(Qt::gray, 2, Qt::DashLine);
        QPen penGauss(Qt::gray, 1, Qt::DashLine);
        _main.pos = makeLine(plot, penMain);
        _main.neg = makeLine(plot, penMain);
        _gauss.pos = makeLine(plot, penGauss);
        _gauss.neg = makeLine(plot, penGauss);
    }

    void update(const GaussPlotOptions& options)
    {
        _main.pos->setVisible(options.plotZ0);
        _main.neg->setVisible(options.plotZ0 && options.plotMinusZ);
        _gauss.pos->setVisible(false);
        _gauss.neg->setVisible(false);
        if (!options.plotZ0) return;
        double z0 = _plotter->unitZ->fromSi(_calc->z0());
        _main.pos->point1->setCoords(z0, 0);
        _main.pos->point2->setCoords(z0, 1);
        _main.neg->point1->setCoords(-z0, 0);
        _main.neg->point2->setCoords(-z0, 1);
        if (_plotter->isHyper())
        {
            _gauss.pos->setVisible(true);
            _gauss.neg->setVisible(options.plotMinusZ);
            z0 *= _plotter->MI;
            _gauss.pos->point1->setCoords(z0, 0);
            _gauss.pos->point2->setCoords(z0, 1);
            _gauss.neg->point1->setCoords(-z0, 0);
            _gauss.neg->point2->setCoords(-z0, 1);
        }
    }

private:
    GaussPlotter *_plotter;
    GaussCalculator *_calc;
    struct LineSet { QCPItemStraightLine *pos, *neg; };
    LineSet _main, _gauss;

    static QCPItemStraightLine* makeLine(QCPL::Plot* plot, const QPen& pen)
    {
        QCPItemStraightLine *line = new QCPItemStraightLine(plot);
        line->setPen(pen);
        QPen selectedPen(pen);
        selectedPen.setColor(Qt::black);
        line->setSelectedPen(selectedPen);
        return line;
    }
};

//--------------------------------------------------------------------------------
//                                 GaussGraphV
//--------------------------------------------------------------------------------

class GaussGraphV
{
public:
    GaussGraphV(QCPL::Plot* plot, GaussPlotter *plotter, GaussCalculator *calc) : _plotter(plotter), _calc(calc)
    {
        _graphs.addItems(plot, 4, QPen(Qt::gray, 1, Qt::DashLine), Qt::NoBrush);
    }

    void update(const GaussPlotOptions& options)
    {
        _graphs.clearData();
        if (!options.plotV) return;

        double z = _plotter->unitZ->fromSi(_plotter->maxZ);
        double w = _plotter->unitY->fromSi(_plotter->maxZ * tan(_calc->Vs()));

        update(_graphs[0], z, w);
        if (options.plotMinusZ)
            update(_graphs[1], -z, w);
        if (options.plotMinusZ && options.plotMinusY)
            update(_graphs[2], -z, -w);
        if (options.plotMinusY)
            update(_graphs[3], z, -w);
    }

private:
    void update(QCPGraph *g, const double& x, const double& y)
    {
        g->addData(0, 0);
        g->addData(x, y);
    }

    GaussPlotter *_plotter;
    GaussCalculator *_calc;
    GaussGraphsContainer _graphs;
};

//--------------------------------------------------------------------------------
//                                 GaussGraphW
//--------------------------------------------------------------------------------

class GaussGraphW
{
public:
    GaussGraphW(QCPL::Plot* plot, GaussPlotter *plotter) : _plotter(plotter)
    {
        QColor fillColor(255, 0, 0, 30);
        _graphs.addItems(plot, 2, QPen(Qt::red), fillColor);
        _graphs.addItems(plot, 2, Qt::NoPen, fillColor);
        _graphs.addItems(plot, 2, QPen(Qt::red, 1, Qt::DashLine), Qt::NoBrush);
        _positiveGraphs = { +1, _graphs[0], _graphs[2], _graphs[4] };
        _negativeGraphs = { -1, _graphs[1], _graphs[3], _graphs[5] };
    }

    void update(const GaussPlotOptions& options)
    {
        _graphs.clearData();
        if (!options.plotW) return;

        bool isHyper = _plotter->isHyper();

        update(_positiveGraphs, 1, isHyper);
        if (options.plotMinusZ)
            update(_positiveGraphs, -1, isHyper);
        if (options.plotMinusY)
        {
            update(_negativeGraphs, 1, isHyper);
            if (options.plotMinusZ)
                update(_negativeGraphs, -1, isHyper);
        }
    }

private:
    GaussPlotter *_plotter;
    GaussGraphsContainer _graphs;

    struct GraphSet
    {
        double sign;
        QCPGraph *main, *gauss, *equiv;
    };
    GraphSet _positiveGraphs, _negativeGraphs;

    void update(const GraphSet& set, const double& signZ, bool isHyper)
    {
        for (int i = 0; i < _plotter->valuesZ.size(); i++)
        {
            double z = signZ * _plotter->valuesZ.at(i);
            set.main->addData(z, set.sign * _plotter->valuesMain.at(i));
            if (isHyper)
            {
                set.gauss->addData(z, set.sign * _plotter->valuesGauss.at(i));
                set.equiv->addData(z, set.sign * _plotter->valuesEquiv.at(i));
            }
        }
    }
};

//--------------------------------------------------------------------------------
//                                 GaussGraphR
//--------------------------------------------------------------------------------

class GaussGraphR
{
public:
    GaussGraphR(QCPL::Plot* plot, GaussPlotter *plotter) : _plotter(plotter)
    {
        QColor fillColor(0, 0, 255, 30);
        _graphs.addItems(plot, 2, QPen(Qt::blue), fillColor);
        _graphs.addItems(plot, 2, Qt::NoPen, fillColor);
        _positiveGraphs = { +1, _graphs[0], _graphs[2] };
        _negativeGraphs = { -1, _graphs[1], _graphs[3] };
    }

    void update(const GaussPlotOptions& options)
    {
        _graphs.clearData();
        if (!options.plotR) return;

        bool isHyper = _plotter->isHyper();

        update(_positiveGraphs, 1, isHyper);
        if (options.plotMinusY && options.plotMinusZ)
            update(_negativeGraphs, -1, isHyper);
    }

private:
    GaussPlotter *_plotter;
    GaussGraphsContainer _graphs;

    struct GraphSet
    {
        double sign;
        QCPGraph *main, *gauss;
    };
    GraphSet _positiveGraphs, _negativeGraphs;

    void update(const GraphSet& set, const double& signZ, bool isHyper)
    {
        for (int i = 0; i < _plotter->valuesZ.size(); i++)
        {
            double z = signZ * _plotter->valuesZ.at(i);
            set.main->addData(z, set.sign * _plotter->valuesMain.at(i));
            if (isHyper)
                set.gauss->addData(z, set.sign * _plotter->valuesGauss.at(i));
        }
    }
};

//--------------------------------------------------------------------------------
//                              GaussCalculatorWindow
//--------------------------------------------------------------------------------

void GaussCalculatorWindow::showWindow()
{
    if (!__instance)
        __instance = new GaussCalculatorWindow;
    __instance->show();
    __instance->activateWindow();
}

GaussCalculatorWindow::GaussCalculatorWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Gaussian Beam Calculator"));
    setWindowIcon(QIcon(":/window_icons/gauss_calc"));

    _calc.reset(new GaussCalculator);
    _plotter.reset(new GaussPlotter);

    QGridLayout *paramsLayout = new QGridLayout;
    paramsLayout->setMargin(12);
    paramsLayout->setHorizontalSpacing(12);
    paramsLayout->setVerticalSpacing(0);
    makeParams(paramsLayout);

    _plot = new QCPL::Plot;
    _plot->legend->setVisible(false);
    _plot->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    _plot->addTextVarX(QStringLiteral("{unit}"), tr("Unit of measurement"), [this]{
        return _z->value().unit()->name(); });
    _plot->setDefaultTitleX("Axial distance, {unit}");
    _plot->setFormatterTextX("Axial distance, {unit}");

    _plot->addTextVarY(QStringLiteral("{unit}"), tr("Unit of measurement"), [this]{
        return (_plotWR->checkedId() == 0 ? _w : _R )->value().unit()->name(); });
    _plot->addTextVarY(QStringLiteral("{mode}"), tr(""), [this]{
        return _plotWR->checkedId() == 0 ? QStringLiteral("Beam radius") : QStringLiteral("Radius of curvatue"); });
    _plot->setDefaultTitleY("{mode}, {unit}");
    _plot->setFormatterTextY("{mode}, {unit}");

    _warning = new QLabel;
    _warning->setVisible(false);
    _warning->setWordWrap(true);
    _warning->setStyleSheet("background: Crimson; color: white; padding: 0.5em; font-size: 10pt");

    Ori::Layouts::LayoutV({
            makeToolbar(),
            _plot,
            _warning,
            paramsLayout,
        })
        .setMargin(0)
        .setSpacing(0)
        .useFor(this);

    _graphZ0.reset(new GaussGraphZ0(_plot, _plotter.get(), _calc.get()));
    _graphV.reset(new GaussGraphV(_plot, _plotter.get(), _calc.get()));
    _graphW.reset(new GaussGraphW(_plot, _plotter.get()));
    _graphR.reset(new GaussGraphR(_plot, _plotter.get()));

    restoreState();
    Ori::Wnd::moveToScreenCenter(this);

    _updatesEnabled = true;
    _calc->setRef(GaussCalculator::Ref::W0);
    QTimer::singleShot(0, this, [this]{ this->recalc(); });
}

GaussCalculatorWindow::~GaussCalculatorWindow()
{
    storeState();
    qDeleteAll(_paramEditors);
    __instance = nullptr;
}

void GaussCalculatorWindow::restoreState()
{
    QJsonObject root = CustomDataHelpers::loadCustomData("gauss");

    CustomDataHelpers::restoreWindowSize(root, this, 750, 400);

    _plotPlusMinusZ->setCheckedId(root["plot_minus_z"].toInt());
    _plotPlusMinusW->setCheckedId(root["plot_minus_w"].toInt());
    _calcModeLock->setCheckedId(root["lock_mode"].toInt());
    _calcModeZone->setCheckedId(root["zone_mode"].toInt());
    _calc->setLock(GaussCalculator::Lock(root["lock_mode"].toInt()));
    _calc->setZone(GaussCalculator::Zone(root["zone_mode"].toInt()));
    _plotWR->setCheckedId(root["plot_wr"].toInt());
    _plotV->setChecked(root.contains("plot_v") ? root["plot_v"].toBool() : true);
    _plotZ0->setChecked(root.contains("plot_z0") ? root["plot_z0"].toBool() : true);
    _plot->setFormatterTextX(root["x_title"].toString());
    _plot->setFormatterTextY(root["y_title"].toString());
}

void GaussCalculatorWindow::storeState()
{
    QJsonObject root;
    root["window_width"] = width();
    root["window_height"] = height();
    root["plot_minus_z"] = _plotPlusMinusZ->checkedId();
    root["plot_minus_w"] = _plotPlusMinusW->checkedId();
    root["lock_mode"] = int(_calc->lock());
    root["zone_mode"] = int(_calc->zone());
    root["plot_wr"] = _plotWR->checkedId();
    root["plot_v"] = _plotV->isChecked();
    root["plot_z0"] = _plotZ0->isChecked();
    root["x_title"] = _plot->formatterTextX();
    root["y_title"] = _plot->formatterTextY();

    CustomDataHelpers::saveCustomData(root, "gauss");
}

void GaussCalculatorWindow::makeParams(QGridLayout *paramsLayout)
{
    #define FUNC(func_name) &GaussCalculator::func_name

    auto addParam = [this](Z::Dim dim, const QString& alias, const QString& name, const Z::Value& initialValue) {
        auto param = new Z::Parameter(dim, alias, name);
        param->setValue(initialValue);
        _params.append(param);
        return param;
    };

    auto addEditor = [this, paramsLayout](int row, int col, GaussCalcParamEditor *editor) {
        _paramEditors.append(editor);
        paramsLayout->addWidget(editor->editor(), row, col);
        connect(editor, &GaussCalcParamEditor::calcNeeded, this, &GaussCalculatorWindow::recalc);
    };

    using namespace Z::Dims;
    _lambda = addParam(linear(), QStringLiteral("lambda"), QStringLiteral("λ"), 980_nm);
    _MI = addParam(none(), QStringLiteral("MI"), QStringLiteral("M²"), 1);
    _w0 = addParam(linear(), QStringLiteral("w0"), QStringLiteral("w<sub>0</sub>"), 100_mkm);
    _z = addParam(linear(), QStringLiteral("z"), QStringLiteral("z"), 100_mm);
    _z0 = addParam(linear(), QStringLiteral("z0"), QStringLiteral("z<sub>0</sub>"), 0_mm);
    _Vs = addParam(angular(), QStringLiteral("Vs"), QStringLiteral("V<sub>s</sub>"), 0_deg);
    _w = addParam(linear(), QStringLiteral("w"), QStringLiteral("w"), 0_mkm);
    _R = addParam(linear(), QStringLiteral("R"), QStringLiteral("R"), 0_mm);
    _reQ = addParam(linear(), QStringLiteral("q_re"), QStringLiteral("re(q)"), 0_mm);
    _imQ = addParam(linear(), QStringLiteral("q_im"), QStringLiteral("im(q)"), 0_mm);
    _reQ1 = addParam(linear(), QStringLiteral("q1_re"), QStringLiteral("re(1/q)"), 0_mm);
    _imQ1 = addParam(linear(), QStringLiteral("q1_im"), QStringLiteral("im(1/q)"), 0_mm);

    GaussCalculator *c = _calc.get();
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
    _calcModeLock->setCheckedId(int(_calc->lock()));
    connect(_calcModeLock, SIGNAL(checked(int)), this, SLOT(lockSelected(int)));

    _calcModeZone = new Ori::Widgets::ExclusiveActionGroup(this);
    _calcModeZone->add(int(GaussCalculator::Zone::Near), ":/toolbar/gauss_near_zone", tr("Use near-field zone"));
    _calcModeZone->add(int(GaussCalculator::Zone::Far), ":/toolbar/gauss_far_zone", tr("Use far-field zone"));
    _calcModeZone->setCheckedId(int(_calc->zone()));
    connect(_calcModeZone, SIGNAL(checked(int)), this, SLOT(zoneSelected(int)));

    _plotPlusMinusZ = new Ori::Widgets::ExclusiveActionGroup(this);
    _plotPlusMinusZ->add(false, ":/toolbar/gauss_pos_z", tr("Plot 0 .. z"));
    _plotPlusMinusZ->add(true, ":/toolbar/gauss_full_z", tr("Plot -z .. z"));
    connect(_plotPlusMinusZ, SIGNAL(checked(int)), this, SLOT(updatePlot()));

    _plotPlusMinusW = new Ori::Widgets::ExclusiveActionGroup(this);
    _plotPlusMinusW->add(false, ":/toolbar/gauss_pos_w", tr("Plot 0 .. w"));
    _plotPlusMinusW->add(true, ":/toolbar/gauss_full_w", tr("Plot -w .. w"));
    connect(_plotPlusMinusW, SIGNAL(checked(int)), this, SLOT(updatePlot()));

    _plotWR = new Ori::Widgets::ExclusiveActionGroup(this);
    _plotWR->add(int(PLOT_W), ":/toolbar/plot_w", tr("Plot Beam Radius"));
    _plotWR->add(int(PLOT_R), ":/toolbar/plot_r", tr("Plot Favefront ROC"));
    connect(_plotWR, SIGNAL(checked(int)), this, SLOT(updatePlot()));
    _plotV = Ori::Gui::toggledAction(tr("Plot Beam Angle"), this, SLOT(updatePlot()), ":/toolbar/plot_v");
    _plotZ0 = Ori::Gui::toggledAction(tr("Plot Rayleigh Distance"), this, SLOT(updatePlot()), ":/toolbar/plot_z0");

    auto actnAutolimits = Ori::Gui::action(tr("Fit to Graphs"), _plot, SLOT(autolimits()), ":/toolbar/limits_auto");
    auto actnTitleX = Ori::Gui::action(tr("X-axis Title..."), _plot, SLOT(titleDlgX()), ":/toolbar/title_x");
    auto actnTitleY = Ori::Gui::action(tr("Y-axis Title..."), _plot, SLOT(titleDlgY()), ":/toolbar/title_y");
    auto actionCopyImg = Ori::Gui::action(tr("Copy Plot Image"), _plot, SLOT(copyPlotImage()), ":/toolbar/copy_img", QKeySequence::Copy);
    auto actionCopyTbl = Ori::Gui::action(tr("Copy Graph Data"), this, SLOT(copyGraphData()), ":/toolbar/copy_table");
    auto actionCalc = Ori::Gui::action(tr("Formula Calculator"), this, SLOT(showCalculator()), ":/window_icons/calculator");
    auto actionHelp = Ori::Gui::action(tr("Help"), this, SLOT(showHelp()), ":/toolbar/help", QKeySequence::HelpContents);

    auto toolbar = new Ori::Widgets::FlatToolBar;
    toolbar->setIconSize(AppSettings::instance().toolbarIconSize());
    Ori::Gui::populate(toolbar, {
        _calcModeLock, nullptr, _calcModeZone, nullptr, _plotPlusMinusZ, _plotPlusMinusW, nullptr,
        _plotWR, _plotV, _plotZ0, nullptr, actnAutolimits, actnTitleX, actnTitleY, nullptr,
        actionCopyImg, actionCopyTbl, nullptr, actionCalc, nullptr, actionHelp
    });
    return toolbar;
}

void GaussCalculatorWindow::zoneSelected(int zone)
{
    _calc->setZone(GaussCalculator::Zone(zone));
}

void GaussCalculatorWindow::lockSelected(int lock)
{
    _calc->setLock(GaussCalculator::Lock(lock));
}

void GaussCalculatorWindow::recalc()
{
    _calc->calc();

    // When recalc is initiated by changing a parameter
    // do not take value of this parameter from calculator
    // as it will interfere user input.
    auto changingEditor = sender();
    foreach (GaussCalcParamEditor *editor, _paramEditors)
        if (editor != changingEditor)
            editor->getValueFromCalculator();

    updatePlot();
}

void GaussCalculatorWindow::updatePlot()
{
    if (!_updatesEnabled) return;

    PlotMode mode = PlotMode(_plotWR->checkedId());

    _plotter->MI = qAbs(_calc->MI());
    _plotter->w0 = _calc->w0();
    _plotter->maxZ = _calc->z();
    _plotter->lambda = _calc->lambda();
    _plotter->unitZ = _z->value().unit();
    _plotter->unitY = (mode == PLOT_W ? _w : _R)->value().unit();
    _plotter->cleanData();
    if (qAbs(_calc->z()) > std::numeric_limits<double>::epsilon())
    {
        if (mode == PLOT_W)
            _plotter->calculateW();
        else _plotter->calculateR();
    }

    GaussPlotOptions o;
    o.plotZ0 = _plotZ0->isChecked();
    o.plotV = _plotV->isChecked();
    o.plotW = mode == PLOT_W;
    o.plotR = mode == PLOT_R;
    o.plotMinusZ = _plotPlusMinusZ->checkedId();
    o.plotMinusY = _plotPlusMinusW->checkedId();

    _graphZ0->update(o);
    _graphV->update(o);
    _graphW->update(o);
    _graphR->update(o);

    double maxZ = _z->value().value();
    double minZ = o.plotMinusZ ? -maxZ : 0;
    double maxY = (mode == PLOT_W ? _w : _R)->value().value() * 1.1;
    double minY = o.plotMinusY ? -maxY : 0;
    _plot->setLimitsX(minZ, maxZ, false);
    _plot->setLimitsY(minY, maxY, false);
    _plot->updateTitles();
    _plot->replot();


    QString warning;
    foreach (auto param, _params)
    {
        if (qIsNaN(param->value().value()))
        {
            warning = tr("Some values became NaNs. This means there is no solution for the given set of parameters. "
                "It can be irrecoverable state so it's better to reopen the window to start with default values.");
            break;
        }
    }
    _warning->setText(warning);
    _warning->setVisible(!warning.isEmpty());
}

void GaussCalculatorWindow::showCalculator()
{
    CalculatorWindow::showWindow();
}

void GaussCalculatorWindow::showHelp()
{
    Z::HelpSystem::instance()->showTopic("calc_gauss.html");
}

void GaussCalculatorWindow::copyGraphData()
{
    auto g = _plot->selectedGraph();
    if (!g)
    {
        Ori::Dlg::info(tr("Please select a graph first"));
        return;
    }
    PlotHelpers::toClipboard(g);
}
