#include "GrinLensWindow.h"

#include "CustomPrefs.h"
#include "HelpSystem.h"
#include "funcs/GrinCalculator.h"
#include "io/JsonUtils.h"
#include "widgets/ParamsEditor.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriSvgView.h"
#include "widgets/OriFlatToolBar.h"

#include <QActionGroup>
#include <QLabel>
#include <QTabWidget>
#include <QToolButton>

using namespace Ori::Layouts;

namespace {

GrinLensWindow* __instance = nullptr;

} // namespace

void GrinLensWindow::showWindow()
{
    if (!__instance)
        __instance = new GrinLensWindow;
    __instance->show();
    __instance->activateWindow();
}

GrinLensWindow::GrinLensWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("GRIN Lens Assessment"));
    setWindowIcon(QIcon(":/toolbar/grin"));

    _length =  new Z::Parameter(Z::Dims::linear(), QStringLiteral("L"), QStringLiteral("L"),
                                tr("Length"),
                                tr("Thickness of material. Must be a positive value."));
    _ior = new Z::Parameter(Z::Dims::none(), QStringLiteral("n0"), QStringLiteral("n0"),
                            tr("IOR"),
                            tr("Index of refraction at the optical axis. "
                               "Must be a positive value."));
    _ior2 = new Z::Parameter(Z::Dims::fixed(), QStringLiteral("n2"), QStringLiteral("n2"),
                             tr("IOR gradient"),
                             tr("Radial gradient of index of refraction. "
                                "Only positive gradients are supported."));
    _focus = new Z::Parameter(Z::Dims::linear(), QStringLiteral("F"), QStringLiteral("F"),
                              tr("Focal length"),
                              tr("Distance at wich parallel input rays get converged. "
                                 "Distance is measured from the exit face of the lens. "
                                 "Must be a positive value."));
    _length->setValue(100_mm);
    _ior->setValue(1.73);
    _ior2->setValue(Z::Value(6.80588, Z::Units::inv_m2()));
    _focus->setValue(1.45_m);
    _params << _length << _ior << _ior2 << _focus;

    _statusLabel = new QLabel;
    _statusLabel->setVisible(false);
    _statusLabel->setWordWrap(true);
    _statusLabel->setStyleSheet("QLabel{background:LightCoral;padding:6px}");

    ParamsEditor::Options opts(&_params);
    opts.autoApply = true;
    opts.auxControl = LayoutV({_statusLabel}).setMargin(3).makeWidget();
    auto editors = new ParamsEditor(opts);
    connect(editors, SIGNAL(paramChanged(Z::Parameter*, Z::Value)), this, SLOT(calculate(Z::Parameter*)));

    auto group = new QActionGroup(this);
    group->setExclusive(true);
    _actionCalcF = Ori::Gui::toggledAction(tr("Calc F from n2"), group, nullptr, ":/toolbar/grin_calc_f");
    _actionCalcN2 = Ori::Gui::toggledAction(tr("Calc n2 from F"), group, nullptr, ":/toolbar/grin_calc_n2");

    auto toolbar = new Ori::Widgets::FlatToolBar;
    auto actionHelp = Ori::Gui::action(tr("Help"), this, SLOT(showHelp()), ":/toolbar/help", QKeySequence::HelpContents);
    Ori::Gui::populate(toolbar, {
        Ori::Gui::textToolButton(_actionCalcF), Ori::Gui::textToolButton(_actionCalcN2), nullptr, actionHelp});

    auto tabs = new QTabWidget;
    tabs->addTab(editors, tr("Calc"));
    tabs->addTab(LayoutV({
        Ori::Widgets::SvgView::makeStatic(":/drawing/grin_lens")
    }).setMargin(3).makeWidget(), tr("Outline"));

    LayoutV({toolbar, LayoutV({tabs}).setMargin(6)}).setSpacing(0).setMargin(0).useFor(this);

    restoreState();
    calculate(_length);
}

GrinLensWindow::~GrinLensWindow()
{
    storeState();
    qDeleteAll(_params);
    __instance = nullptr;
}

void GrinLensWindow::restoreState()
{
    QJsonObject root = CustomDataHelpers::loadCustomData("grin");
    if (root["solve_n2"].toBool(true))
        _actionCalcN2->setChecked(true);
    else _actionCalcF->setChecked(true);
    for (auto p : _params)
        if (root.contains(p->alias())) {
            auto res = Z::IO::Json::readValue(root[p->alias()].toObject(), p->dim());
            if (res.ok()) p->setValue(res.value());
        }
    CustomDataHelpers::restoreWindowSize(root, this, 340, 280);
}

void GrinLensWindow::storeState()
{
    QJsonObject root;
    root["window_width"] = width();
    root["window_height"] = height();
    root["solve_n2"] = _actionCalcN2->isChecked();
    for (auto p : _params)
        root[p->alias()] = Z::IO::Json::writeValue(p->value());
    CustomDataHelpers::saveCustomData(root, "grin");
}

void GrinLensWindow::calculate(Z::Parameter *p)
{
    if (p == _length or p == _ior) {
        if (_actionCalcF->isChecked()) {
            calculateF();
        } else {
            calculateN2();
        }
    } else if (p == _ior2) {
        calculateF();
    } else if (p == _focus) {
        calculateN2();
    }
}

void GrinLensWindow::calculateN2()
{
    double L = _length->value().toSi();
    if (L <= 0)
        return showError(tr("L must be a positive value"));
    double n0 = _ior->value().toSi();
    if (n0 < 0)
        return showError(tr("n0 must be a positive value"));
    double F = _focus->value().toSi();
    if (F <= 0)
        return showError(tr("F must be a positive value"));
    double n2 = GrinCalculator::solve_n2(L, F, n0);
    qDebug() << "Calculated n2" << QString::number(n2, 'g', 16);
    auto unit = _ior2->value().unit();
    _ior2->setValue(Z::Value(unit->fromSi(n2), unit));
    showError(QString());
}

void GrinLensWindow::calculateF()
{
    double L = _length->value().toSi();
    if (L <= 0)
        return showError(tr("L must be a positive value"));
    double n0 = _ior->value().toSi();
    if (n0 < 0)
        return showError(tr("n0 must be a positive value"));
    double n2 = _ior2->value().toSi();
    if (n2 < 0)
        return showError(tr("n2 must be a positive value"));
    double F = GrinCalculator::calc_focus(L, n0, n2);
    qDebug() << "Calculated F" << QString::number(F, 'g', 16);
    auto unit = _focus->value().unit();
    _focus->setValue(Z::Value(unit->fromSi(F), unit));
    showError(QString());
}

void GrinLensWindow::showHelp()
{
    Z::HelpSystem::instance()->showTopic("calc_grin.html");
}

void GrinLensWindow::showError(const QString& err)
{
    _statusLabel->setText(err);
    _statusLabel->setVisible(!err.isEmpty());
}
