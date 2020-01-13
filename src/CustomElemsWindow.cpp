#include "CustomElemsWindow.h"

#include "CustomPrefs.h"
#include "helpers/OriWindows.h"

#include <QIcon>

namespace {

CustomElemsWindow* __instance = nullptr;

} // namespace

void CustomElemsWindow::showWindow()
{
    if (!__instance)
        __instance = new CustomElemsWindow;
    __instance->show();
    __instance->activateWindow();
}

CustomElemsWindow::CustomElemsWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Custom Elements"));
    setWindowIcon(QIcon(":/window_icons/catalog"));

    restoreState();
    Ori::Wnd::moveToScreenCenter(this);
}

CustomElemsWindow::~CustomElemsWindow()
{
    storeState();
    __instance = nullptr;
}

void CustomElemsWindow::restoreState()
{
    QJsonObject root = CustomDataHelpers::loadCustomData("elems");

    CustomDataHelpers::restoreWindowSize(root, this, 750, 400);
}

void CustomElemsWindow::storeState()
{
    QJsonObject root;
    root["window_width"] = width();
    root["window_height"] = height();

    CustomDataHelpers::saveCustomData(root, "elems");
}
