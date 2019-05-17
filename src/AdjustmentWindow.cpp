#include "AdjustmentWindow.h"

namespace {
AdjustmentWindow* __instance = nullptr;
}

void AdjustmentWindow::open(Schema* schema, QWidget* parent)
{
    if (!__instance)
    {
        __instance = new AdjustmentWindow(schema, parent);
        // TODO: restore adjusters
    }
    __instance->show();
    __instance->activateWindow();
}

AdjustmentWindow::AdjustmentWindow(Schema *schema, QWidget *parent)
    : QWidget(parent, Qt::Tool), SchemaToolWindow(schema)
{
    __instance = this;

    setWindowTitle(tr("Adjustment"));
}

AdjustmentWindow::~AdjustmentWindow()
{
    __instance = nullptr;
    // TODO: store adjusters
}
