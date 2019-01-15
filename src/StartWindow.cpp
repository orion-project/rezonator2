#include "StartWindow.h"
#include "ProjectWindow.h"
#include "GaussCalculatorWindow.h"
#include "core/CommonTypes.h"
#include "helpers/OriWindows.h"
#include "helpers/OriLayouts.h"

#include <QLabel>
#include <QPushButton>
#include <QToolButton>

using namespace Ori::Layouts;

StartWindow::StartWindow(QWidget *parent) : QWidget(parent)
{
    Ori::Wnd::setWindowIcon(this, ":/window_icons/main");
    setAttribute(Qt::WA_DeleteOnClose);

    LayoutV({
        LayoutH({
            makeActionsPanel(),
            makeMruPanel(),
            makeToolsPanel()
        }),
        makeTipsPanel()
    }).useFor(this);
}

QWidget* StartWindow::makeButton(const QString& iconPath, const QString& title, const char* slot)
{
    auto button = new QToolButton;
    button->setIconSize(QSize(24, 24));
    button->setIcon(QIcon(iconPath));
    button->setText(title);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    if (slot)
        connect(button, SIGNAL(clicked()), this, slot);
    return button;
}

QWidget* StartWindow::makeActionsPanel()
{
    return LayoutV({
        makeHeader(tr("Actions")),
        makeButton(":/toolbar/schema_open", tr("Open Schema File"), nullptr),
        makeButton(":/toolbar/schema_open", tr("Open Example Schema"), nullptr), // TODO make different icon
        makeButton(TripTypes::info(TripType::SW).iconPath(), tr("Create Standing Wave Resonator"), nullptr),
        makeButton(TripTypes::info(TripType::RR).iconPath(), tr("Create Ring Resonator"), nullptr),
        makeButton(TripTypes::info(TripType::SP).iconPath(), tr("Create Single-pass System"), nullptr),
        Stretch()
    }).makeWidget();
}

QWidget* StartWindow::makeMruPanel()
{
    return LayoutV({
        makeHeader(tr("Open Recent Schema")),
        Stretch()
    }).makeWidget();
}

QWidget* StartWindow::makeTipsPanel()
{
    return LayoutV({
        makeHeader(tr("Whether You Know What")),
        Stretch()
    }).makeWidget();
}

QWidget* StartWindow::makeToolsPanel()
{
    auto buttonGauss = makeButton(":/toolbar/gauss_calculator", tr("Gauss Calculator"), SLOT(toolGaussCalc()));

    return LayoutV({
        makeHeader(tr("Tools")),
        buttonGauss,
        Stretch()
    }).makeWidget();
}

QWidget* StartWindow::makeHeader(const QString& title)
{
    auto label = new QLabel(title);
    auto font = label->font();
    font.setPointSize(font.pointSize() + 3);
    font.setBold(true);
    label->setFont(font);
    return label;
}

void StartWindow::makeNewSchema()
{
    close();
    (new ProjectWindow)->show();
}

void StartWindow::toolGaussCalc()
{
    GaussCalculatorWindow::showCalcWindow();
}
