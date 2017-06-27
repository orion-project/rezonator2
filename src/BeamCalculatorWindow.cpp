#include "BeamCalculatorWindow.h"
#include "widgets/Plot.h"
#include "../orion/Actions.h"
#include "../orion/FlatToolBar.h"
#include "../orion/ValueEdit.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QIcon>
#include <QVector>

void showBeamCalculatorWindow()
{
    auto w = new BeamCalculatorWindow();
    w->resize(750, 400);
    w->show();
}

////////////////////////////////////////////////////////////////////////////////

void BeamPlotter::calculate()
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
}

////////////////////////////////////////////////////////////////////////////////

BeamCalculatorWindow::BeamCalculatorWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Beam Calculator"));
    setWindowIcon(QIcon(":/window_icons/beamcalc"));

    _units.setUnit(Z::Units::Dim_Linear, Z::Units::mm);
    _units.setUnit(Z::Units::Dim_Angle, Z::Units::deg);
    _units.setUnit(Z::Units::Dim_Beamsize, Z::Units::mkm);
    _units.setUnit(Z::Units::Dim_Lambda, Z::Units::nm);
    _plotter.units = &_units;

    auto layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(makeToolbar());
    layout->addWidget(makePlot());
    layout->addLayout(makeParamEditors());
    setLayout(layout);

    _editorLambda->setValue(_units.si2lambda(_calc.lambda()));

    _calc.calc();

    populateUnitLabels();
    populateValues();
    updatePlot();
}

QWidget* BeamCalculatorWindow::makeToolbar()
{
    auto locks = new Ori::Gui::ExclusiveActionGroup(this);
    locks->add(int(BeamCalculator::Lock::Waist), ":/toolbar/gauss_lock_waist", tr("Lock waist"));
    locks->add(int(BeamCalculator::Lock::Front), ":/toolbar/gauss_lock_front", tr("Lock front"));
    locks->setCheckedId(int(_calc.lock()));
    connect(locks, SIGNAL(checked(int)), this, SLOT(lockSelected(int)));

    auto zones = new Ori::Gui::ExclusiveActionGroup(this);
    zones->add(int(BeamCalculator::Zone::Near), ":/toolbar/gauss_near_zone", tr("Use near-field zone"));
    zones->add(int(BeamCalculator::Zone::Far), ":/toolbar/gauss_far_zone", tr("Use far-field zone"));
    zones->setCheckedId(int(_calc.zone()));
    connect(zones, SIGNAL(checked(int)), this, SLOT(zoneSelected(int)));

    _plotPlusMinusZ = new Ori::Gui::ExclusiveActionGroup(this);
    _plotPlusMinusZ->add(false, ":/toolbar/gauss_pos_z", tr("Plot 0 .. z"));
    _plotPlusMinusZ->add(true, ":/toolbar/gauss_full_z", tr("Plot -z .. z"));
    connect(_plotPlusMinusZ, SIGNAL(checked(int)), this, SLOT(updatePlot()));

    _plotPlusMinusW = new Ori::Gui::ExclusiveActionGroup(this);
    _plotPlusMinusW->add(false, ":/toolbar/gauss_pos_w", tr("Plot 0 .. w"));
    _plotPlusMinusW->add(true, ":/toolbar/gauss_full_w", tr("Plot -w .. w"));
    connect(_plotPlusMinusW, SIGNAL(checked(int)), this, SLOT(updatePlot()));

    auto actionHelp = new QAction(QIcon(":/toolbar/help"), tr("Help"), this);

    auto toolbar = new Ori::Gui::FlatToolBar;
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

QWidget* BeamCalculatorWindow::makePlot()
{
    _plot = new Plot;
    _graphPlusW = _plot->addGraph();
    _graphMinusW = _plot->addGraph();
    return _plot;
}

QLayout* BeamCalculatorWindow::makeParamEditors()
{
    auto layout = new QGridLayout;
    layout->setMargin(12);
    layout->setHorizontalSpacing(4);
    layout->setVerticalSpacing(6);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 0);
    layout->setColumnStretch(3, 0);
    layout->setColumnStretch(4, 1);
    layout->setColumnStretch(5, 0);
    layout->setColumnStretch(6, 0);
    layout->setColumnStretch(7, 1);
    layout->setColumnStretch(8, 0);

    layout->addWidget(makeLabel("λ", false),                          0, 0);
    layout->addWidget(_editorLambda  = makeEditor(),                  0, 1);
    layout->addWidget(_labelLambda = new QLabel,                      0, 2);

    layout->addWidget(makeLabel("ω<sub>0</sub>", false),              1, 0);
    layout->addWidget(_editorW0 = makeEditor(),                       1, 1);
    layout->addWidget(_labelW0 = new QLabel,                          1, 2);

    layout->addWidget(makeLabel("M²", false),                         2, 0);
    layout->addWidget(_editorM2 = makeEditor(),                       2, 1);
    layout->addWidget(_labelM2 = new QLabel,                          2, 2);


    layout->addWidget(makeLabel("z<sub>0</sub>"),                     0, 3);
    layout->addWidget(_editorZ0 = makeEditor(),                       0, 4);
    layout->addWidget(_labelZ0 = new QLabel,                          0, 5);

    layout->addWidget(makeLabel("V<sub>s</sub>"),                     1, 3);
    layout->addWidget(_editorVs = makeEditor(),                       1, 4);
    layout->addWidget(_labelVs = new QLabel,                          1, 5);

    layout->addWidget(makeLabel("z"),                                 2, 3);
    layout->addWidget(_editorZ = makeEditor(),                        2, 4);
    layout->addWidget(_labelZ = new QLabel,                           2, 5);


    layout->addWidget(makeLabel("ω"),                                 0, 6);
    layout->addWidget(_editorW = makeEditor(),                        0, 7);
    layout->addWidget(_labelW = new QLabel,                           0, 8);

    layout->addWidget(makeLabel("R"),                                 1, 6);
    layout->addWidget(_editorR = makeEditor(),                        1, 7);
    layout->addWidget(_labelR = new QLabel,                           1, 8);

    layout->addWidget(makeLabel("q"),                                 2, 6);
    layout->addLayout(makeEditor(&_editorReQ, &_editorImQ),           2, 7);
    layout->addWidget(_labelQ = new QLabel(),                         2, 8);

    layout->addWidget(makeLabel("1/q"),                               3, 6);
    layout->addLayout(makeEditor(&_editorReQ1, &_editorImQ1),         3, 7);
    layout->addWidget(_labelQ1 = new QLabel(),                        3, 8);

    return layout;
}

QLabel* BeamCalculatorWindow::makeLabel(const char* text, bool indentLeft)
{
    QLabel* label = new QLabel(QString::fromUtf8(text));
    QFont f = label->font();
    f.setFamily("Times New Roman");
    f.setPointSize(12);
    label->setFont(f);
    label->setContentsMargins(indentLeft? 20: 0, 0, 0, 0);
    label->setAlignment(Qt::AlignRight);
    return label;
}

ValueEdit* BeamCalculatorWindow::makeEditor()
{
    auto editor = new ValueEdit;
    connect(editor, SIGNAL(valueEdited(double)), this, SLOT(valueChanged(double)));
    return editor;
}

QLayout *BeamCalculatorWindow::makeEditor(ValueEdit** re, ValueEdit** im)
{
    auto label = new QLabel(" + i ");
    QFont f = label->font();
    f.setFamily("Times New Roman");
    f.setItalic(true);
    f.setPointSize(12);
    label->setFont(f);

    auto layout = new QHBoxLayout;
    layout->addWidget(*re = makeEditor());
    layout->addWidget(label);
    layout->addWidget(*im = makeEditor());
    layout->setMargin(0);
    layout->setSpacing(0);
    return layout;
}

void BeamCalculatorWindow::populateUnitLabels()
{
    auto linearUnits = Z::Units::unitNameTr(_units.linear());
    auto beamsizeUnits = Z::Units::unitNameTr(_units.beamsize());
    auto lambdaUnits = Z::Units::unitNameTr(_units.lambda());
    auto angleUnits = Z::Units::unitNameTr(_units.angular());
    _labelLambda->setText(lambdaUnits);
    _labelW0->setText(beamsizeUnits);
    _labelZ0->setText(linearUnits);
    _labelVs->setText(angleUnits);
    _labelZ->setText(linearUnits);
    _labelW->setText(beamsizeUnits);
    _labelR->setText(linearUnits);
    _labelQ->setText(linearUnits);
    _labelQ1->setText(linearUnits + "<sup>-1</sup>");
}

void BeamCalculatorWindow::valueChanged(double value)
{
    // NB! В случае с обратным КПП здесь нужно именно si2linear, а не наоборот,
    // т.к. обратный КПП измеряется в обратных единицах. Перевод значения
    // из неких обратных единиц A в обратные единицы В формально тоже самое, что
    // перевод этого же значения из прямых единиц В в прямые единицы А
    // (прямой перевод обратных единиц = обратный перевод прямых единиц)
    auto editor = sender();
    if (editor == _editorLambda) _calc.setLambda(_units.lambda2si(value));
    else if (editor == _editorW0) _calc.setW0(_units.beamsize2si(value));
    else if (editor == _editorM2) _calc.setM2(value);
    else if (editor == _editorZ0) _calc.setZ0(_units.linear2si(value));
    else if (editor == _editorVs) _calc.setVs(_units.angle2si(value));
    else if (editor == _editorZ) _calc.setZ(_units.linear2si(value));
    else if (editor == _editorW) _calc.setW(_units.beamsize2si(value));
    else if (editor == _editorR) _calc.setR(_units.linear2si(value));
    else if (editor == _editorReQ) _calc.setReQ(_units.linear2si(value));
    else if (editor == _editorImQ) _calc.setImQ(_units.linear2si(value));
    else if (editor == _editorReQ1) _calc.setReQ1(_units.si2linear(value));
    else if (editor == _editorImQ1) _calc.setImQ1(_units.si2linear(value));
    _calc.calc();
    populateValues(editor);
    updatePlot();
}

void BeamCalculatorWindow::populateValues(QObject* exclude)
{
    // NB! В случае с обратным КПП здесь нужно именно linear2si, а не наоборот.
    // См. примечание к процедуре BeamCalculatorWindow::valueChanged()
    if (exclude != _editorW0) _editorW0->setValue(_units.si2beamsize(_calc.w0()));
    if (exclude != _editorM2) _editorM2->setValue(_calc.M2());
    if (exclude != _editorZ0) _editorZ0->setValue(_units.si2linear(_calc.z0()));
    if (exclude != _editorVs) _editorVs->setValue(_units.si2angle(_calc.Vs()));
    if (exclude != _editorZ) _editorZ->setValue(_units.si2linear(_calc.z()));
    if (exclude != _editorW) _editorW->setValue(_units.si2beamsize(_calc.w()));
    if (exclude != _editorR) _editorR->setValue(_units.si2linear(_calc.R()));
    if (exclude != _editorReQ) _editorReQ->setValue(_units.si2linear(_calc.reQ()));
    if (exclude != _editorImQ) _editorImQ->setValue(_units.si2linear(_calc.imQ()));
    if (exclude != _editorReQ1) _editorReQ1->setValue(_units.linear2si(_calc.reQ1()));
    if (exclude != _editorImQ1) _editorImQ1->setValue(_units.linear2si(_calc.imQ1()));
}

void BeamCalculatorWindow::zoneSelected(int zone)
{
    _calc.setZone(BeamCalculator::Zone(zone));
    //qDebug() << ((_calc.zone() == BeamCalculator::Zone::Near)? "Zone: Near": "Zone: Far");
}

void BeamCalculatorWindow::lockSelected(int lock)
{
    _calc.setLock(BeamCalculator::Lock(lock));
    //qDebug() << ((_calc.lock() == BeamCalculator::Lock::Waist)? "Lock: Waist": "Lock: Front");
}

void BeamCalculatorWindow::updatePlot()
{
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
}















