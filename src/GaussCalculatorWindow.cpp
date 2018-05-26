#include "GaussCalculatorWindow.h"

#include "widgets/Plot.h"
#include "widgets/ParamEditor.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWindows.h"
#include "helpers/OriWidgets.h"
#include "tools/OriSettings.h"
#include "widgets/OriActions.h"
#include "widgets/OriFlatToolBar.h"

#include <QGridLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QIcon>
#include <QTimer>


class GaussPlotter
{
public:
    double lambda;
    double w0;
    double maxZ;
    double MI;
    Z::Unit unitZ;
    Z::Unit unitW;

    const int points = 50;

    QVector<double> valuesZ;
    QVector<double> valuesW;
    QVector<double> valuesW_aux;

    bool isHyper() const
    {
        return qAbs(MI - 1.0) > std::numeric_limits<double>::epsilon();
    }

    void calculate()
    {
        valuesZ.clear();
        valuesW.clear();
        valuesW_aux.clear();

        if (qAbs(maxZ - 0) <= std::numeric_limits<double>::epsilon())
            return;

        const bool is_hyper = isHyper();
        const double z0_gauss = M_PI * w0*w0 / lambda;
        const double z0_hyper = M_PI * w0*w0 / lambda / MI;
        const double sqrZ0_gauss = z0_gauss * z0_gauss;
        const double sqrZ0_hyper = z0_hyper * z0_hyper;
        const double sqrW0 = w0 * w0;
        const double step = maxZ / double(points);
        double z = 0;

        auto calcPoint = [&](){
            valuesZ.append(unitZ->fromSi(z));
            double w_gauss = sqrt(sqrW0 * (1 + z*z / sqrZ0_gauss));
            if (is_hyper)
            {
                double w_hyper = sqrt(sqrW0 * (1 + z*z / sqrZ0_hyper));
                valuesW.append(unitW->fromSi(w_hyper));
                valuesW_aux.append(unitW->fromSi(w_gauss));
            }
            else
                valuesW.append(unitW->fromSi(w_gauss));
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
//                              GaussGraphsContainer
//--------------------------------------------------------------------------------

class GaussGraphsContainer
{
public:
    void addItems(Plot* plot, int count, const QPen& pen, const QBrush& brush)
    {
        for (int i = 0; i < count; i++)
        {
            QCPGraph *g = plot->addGraph();
            g->setPen(pen);
            g->setBrush(brush);
            _items.append(g);
        }
    }

    void clearData()
    {
        for (QCPGraph *g : _items)
            g->clearData();
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
};

//--------------------------------------------------------------------------------
//                                GaussGraphAngle
//--------------------------------------------------------------------------------

class GaussGraphV
{
public:
    GaussGraphV(Plot* plot, GaussPlotter *plotter, GaussCalculator *calc) : _plotter(plotter), _calc(calc)
    {
        _graphs.addItems(plot, 4, QPen(Qt::gray, 1, Qt::DashLine), Qt::NoBrush);
    }

    void update(const GaussPlotOptions& options)
    {
        _graphs.clearData();
        if (!options.plotV) return;

        double z = _plotter->unitZ->fromSi(_plotter->maxZ);
        double w = _plotter->unitW->fromSi(_plotter->maxZ * tan(_calc->Vs()));

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
//                                   GaussGraphW
//--------------------------------------------------------------------------------

class GaussGraphW
{
public:
    GaussGraphW(Plot* plot, GaussPlotter *plotter) : _plotter(plotter)
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
        double M = isHyper ? sqrt(_plotter->MI) : 1;

        update(_positiveGraphs, 1, M);
        if (options.plotMinusZ)
            update(_positiveGraphs, -1, M);
        if (options.plotMinusY)
        {
            update(_negativeGraphs, 1, M);
            if (options.plotMinusZ)
                update(_negativeGraphs, -1, M);
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

    void update(const GraphSet& set, const double& signZ, const double& M)
    {
        bool isHyper = M != 1;
        for (int i = 0; i < _plotter->valuesZ.size(); i++)
        {
            double z = signZ * _plotter->valuesZ.at(i);
            double w = set.sign * _plotter->valuesW.at(i);
            set.main->addData(z, w);
            if (isHyper)
            {
                set.gauss->addData(z, set.sign * _plotter->valuesW_aux.at(i));
                set.equiv->addData(z, w/M);
            }
        }
    }
};

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

    _plotter.reset(new GaussPlotter);
    _graphV.reset(new GaussGraphV(_plot, _plotter.get(), &_calc));
    _graphW.reset(new GaussGraphW(_plot, _plotter.get()));

    restoreState();
    Ori::Wnd::moveToScreenCenter(this);
    QTimer::singleShot(0, [this]{ this->recalc(); });
}

GaussCalculatorWindow::~GaussCalculatorWindow()
{
    storeState();
    qDeleteAll(_paramEditors);
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
    _plotWR->setCheckedId(root["plot_wr"].toInt());
    _plotV->setChecked(root.contains("plot_v") ? root["plot_v"].toBool() : true);
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
    root["plot_wr"] = _plotWR->checkedId();
    root["plot_v"] = _plotV->isChecked();

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
        _paramEditors.append(editor);
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

    _plotWR = new Ori::Widgets::ExclusiveActionGroup(this);
    _plotWR->add(int(PLOT_W), ":/toolbar/plot_w", tr("Plot Beam Radius"));
    _plotWR->add(int(PLOT_R), ":/toolbar/plot_r", tr("Plot Favefront ROC"));
    connect(_plotWR, SIGNAL(checked(int)), this, SLOT(updatePlot()));
    _plotV = Ori::Gui::toggledAction(tr("Plot Beam Angle"), this, SLOT(updatePlot()), ":/toolbar/plot_v");

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
    toolbar->addActions(_plotWR->actions());
    toolbar->addAction(_plotV);
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

    for (GaussCalcParamEditor *editor : _paramEditors)
        editor->getValueFromCalculator();

    updatePlot();
}

void GaussCalculatorWindow::updatePlot()
{
    _plotter->lambda = _calc.lambda();
    _plotter->w0 = _calc.w0();
    _plotter->maxZ = _calc.z();
    _plotter->MI = qAbs(_calc.MI());
    _plotter->unitW = _w->value().unit();
    _plotter->unitZ = _z->value().unit();
    _plotter->calculate();

    GaussPlotOptions o;
    o.plotV = _plotV->isChecked();
    o.plotW = _plotWR->checkedId() == int(PLOT_W);
    o.plotR = _plotWR->checkedId() == int(PLOT_R);
    o.plotMinusZ = _plotPlusMinusZ->checkedId();
    o.plotMinusY = _plotPlusMinusW->checkedId();

    //_graphR->update(o);
    _graphV->update(o);
    _graphW->update(o);

    double maxZ = _z->value().value();
    double minZ = o.plotMinusZ ? -maxZ : 0;
    double maxW = _w->value().value() * 1.1;
    double minW = o.plotMinusY ? -maxW : 0;
    _plot->setLimitsX(minZ, maxZ, false);
    _plot->setLimitsY(minW, maxW, false);
    _plot->replot();
}
