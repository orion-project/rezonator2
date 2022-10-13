#include "GrinLensWindow.h"

#include "Appearance.h"
#include "CustomPrefs.h"
#include "funcs/GrinCalculator.h"
#include "io/JsonUtils.h"
#include "widgets/ParamsEditor.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriSvgView.h"

#include <QActionGroup>
#include <QLabel>
#include <QTabWidget>
#include <QToolBar>
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
                             tr("Radial gradient of index of refraction."));
    _focus = new Z::Parameter(Z::Dims::linear(), QStringLiteral("F"), QStringLiteral("F"),
                              tr("Focal length"),
                              tr("Distance at that parallel input rays get converged (for positive lens). "
                                 "Distance is measured from the exit face of the lens."));
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
    opts.applyMode = ParamsEditor::Options::ApplyInstant;
    opts.ownParams = true;
    opts.auxControl = LayoutV({_statusLabel}).setMargin(3).makeWidget();
    auto editors = new ParamsEditor(opts);
    connect(editors, SIGNAL(paramChanged(Z::Parameter*, Z::Value)), this, SLOT(calculate(Z::Parameter*)));

    auto group = new QActionGroup(this);
    _actionCalcF = Ori::Gui::toggledAction(tr("Calc F from n2"), group, nullptr, ":/toolbar/grin_calc_f");
    _actionCalcN2 = Ori::Gui::toggledAction(tr("Calc n2 from F"), group, nullptr, ":/toolbar/grin_calc_n2");

    auto toolbar = Z::Gui::makeToolbar({ Ori::Gui::textToolButton(_actionCalcF),
                                         Ori::Gui::textToolButton(_actionCalcN2),
                                       }, "calc_grin");

    _outline = new Ori::Widgets::SvgView;

    auto tabs = Z::Gui::makeBorderlessTabs();
    tabs->addTab(editors, tr("Calc"));
    tabs->addTab(LayoutV({_outline}).setMargin(3).makeWidget(), tr("Outline"));

    LayoutV({toolbar, tabs}).setSpacing(0).setMargin(0).useFor(this);

    restoreState();
    calculate(_length);
}

GrinLensWindow::~GrinLensWindow()
{
    storeState();
    __instance = nullptr;
}

void GrinLensWindow::restoreState()
{
    QJsonObject root = CustomDataHelpers::loadCustomData("grin");
    if (root["solve_n2"].toBool(true))
        _actionCalcN2->setChecked(true);
    else _actionCalcF->setChecked(true);
    _restoring = true;
    foreach (auto p, _params)
    {
        auto res = Z::IO::Json::readValue(root[p->alias()].toObject(), p->dim());
        if (res.ok()) p->setValue(res.value());
    }
    _restoring = false;
    CustomDataHelpers::restoreWindowSize(root, this, 340, 280);
}

void GrinLensWindow::storeState()
{
    QJsonObject root;
    CustomDataHelpers::storeWindowSize(root, this);
    root["solve_n2"] = _actionCalcN2->isChecked();
    foreach (auto p, _params)
        root[p->alias()] = Z::IO::Json::writeValue(p->value());
    CustomDataHelpers::saveCustomData(root, "grin");
}

void GrinLensWindow::calculate(Z::Parameter *p)
{
    if (_restoring)
        return;
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
        return showError(tr("L must be positive"));
    double n0 = _ior->value().toSi();
    if (n0 < 0)
        return showError(tr("n0 must be positive"));
    double F = _focus->value().toSi();
    double n2 = GrinCalculator::solve_n2(L, F, n0);
    //qDebug() << "Calculated n2" << QString::number(n2, 'g', 16);
    auto unit = _ior2->value().unit();
    _ior2->setValue(Z::Value(unit->fromSi(n2), unit));
    showError(QString());
    updateOutline(F < 0);
}

void GrinLensWindow::calculateF()
{
    double L = _length->value().toSi();
    if (L <= 0)
        return showError(tr("L must be positive"));
    double n0 = _ior->value().toSi();
    if (n0 < 0)
        return showError(tr("n0 must be positive"));
    double n2 = _ior2->value().toSi();
    double F = GrinCalculator::calc_focus(L, n0, n2);
    //qDebug() << "Calculated F" << QString::number(F, 'g', 16);
    auto unit = _focus->value().unit();
    _focus->setValue(Z::Value(unit->fromSi(F), unit));
    showError(QString());
    updateOutline(F < 0);
}

void GrinLensWindow::showError(const QString& err)
{
    _statusLabel->setText(err);
    _statusLabel->setVisible(!err.isEmpty());
}

void GrinLensWindow::updateOutline(bool neg)
{
    if (neg) {
        if (_outlineKind >= 0) {
            _outlineKind = -1;
            _outline->load(":/drawing/grin_lens_neg");
        }
    } else {
        if (_outlineKind <= 0) {
            _outlineKind = 1;
            _outline->load(":/drawing/grin_lens");
        }
    }
}
