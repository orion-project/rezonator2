#include "AdjustmentWindow.h"

#include "helpers/OriLayouts.h"

#include <QApplication>
#include <QVBoxLayout>

namespace {
AdjustmentWindow* __instance = nullptr;
}

//------------------------------------------------------------------------------
//                             AdjusterWidget
//------------------------------------------------------------------------------

AdjusterWidget::AdjusterWidget(Z::Parameter *param, QWidget *parent) : QWidget(parent), _param(param)
{
    _testLabel = new QLabel(param->value().str());
    Ori::Layouts::LayoutH({_testLabel}).useFor(this);
    _param->addListener(this);
}

AdjusterWidget::~AdjusterWidget()
{
    _param->removeListener(this);
}

void AdjusterWidget::parameterChanged(Z::ParameterBase*)
{
    // TODO: show vaue
    _testLabel->setText(_param->value().str());
}

//------------------------------------------------------------------------------
//                            AdjusterListWidget
//------------------------------------------------------------------------------

AdjusterListWidget::AdjusterListWidget(QWidget *parent) : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
}

void AdjusterListWidget::add(AdjusterWidget* w)
{
    qobject_cast<QVBoxLayout*>(layout())->addWidget(w);
}

//------------------------------------------------------------------------------
//                             AdjustmentWindow
//------------------------------------------------------------------------------

void AdjustmentWindow::adjust(Schema* schema, Z::Parameter* param)
{
    if (!__instance)
    {
        __instance = new AdjustmentWindow(schema, qApp->activeWindow());
        // TODO: restore adjusters
    }
    __instance->show();
    __instance->activateWindow();
    __instance->addAdjuster(param);
}

AdjustmentWindow::AdjustmentWindow(Schema *schema, QWidget *parent)
    : QWidget(parent, Qt::Tool), SchemaToolWindow(schema)
{
    __instance = this;

    setWindowTitle(tr("Adjustment"));
    setAttribute(Qt::WA_DeleteOnClose);

    _adjustersWidget = new AdjusterListWidget;

    Ori::Layouts::LayoutV({
        Ori::Layouts::LayoutV({_adjustersWidget}).setMargin(3)
    }).setMargin(0).setSpacing(0).useFor(this);
}

AdjustmentWindow::~AdjustmentWindow()
{
    __instance = nullptr;
    // TODO: store adjusters
}

void AdjustmentWindow::elementDeleting(Schema*, Element* elem)
{
    for (auto param : elem->params())
        deleteAdjuster(param);
}

void AdjustmentWindow::customParamDeleting(Schema*, Z::Parameter* param)
{
    deleteAdjuster(param);
}

void AdjustmentWindow::addAdjuster(Z::Parameter* param)
{
    if (!param)
    {
        // TODO: check if empty and show stub
        return;
    }

    for (auto adj : _adjusters)
        if (adj.param == param)
        {
            adj.widget->setFocus();
            return;
        }
    AdjusterItem adjuster;
    adjuster.param = param;
    adjuster.widget = new AdjusterWidget(param);
    _adjustersWidget->add(adjuster.widget);
    adjuster.widget->setFocus();
}

void AdjustmentWindow::deleteAdjuster(Z::Parameter* param)
{
    for (int i = 0; i < _adjusters.size(); i++)
        if (_adjusters.at(i).param == param)
        {
            _adjusters.removeAt(i);
            delete _adjusters.at(i).widget;
            break;
        }
}

