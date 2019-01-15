#include "StartWindow.h"
#include "ProjectWindow.h"
#include "GaussCalculatorWindow.h"
#include "core/CommonTypes.h"
#include "widgets/Appearance.h"
#include "helpers/OriWindows.h"
#include "helpers/OriLayouts.h"

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QResource>
#include <QToolButton>
#include <QPlainTextEdit>

using namespace Ori::Layouts;

StartWindow::StartWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle(qApp->applicationName());
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

    setStyleSheet(QString::fromLatin1(reinterpret_cast<const char*>(
        QResource(":/style/StartWindow").data())));
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
    return makePanel(LayoutV({
        makeHeader(tr("Actions")),
        makeButton(":/toolbar/schema_open", tr("Open Schema File"), nullptr),
        makeButton(":/toolbar/schema_open", tr("Open Example Schema"), nullptr), // TODO make different icon
        makeButton(TripTypes::info(TripType::SW).iconPath(), tr("Create Standing Wave Resonator"), nullptr),
        makeButton(TripTypes::info(TripType::RR).iconPath(), tr("Create Ring Resonator"), nullptr),
        makeButton(TripTypes::info(TripType::SP).iconPath(), tr("Create Single-pass System"), nullptr),
        Stretch()
    }).boxLayout());
}

QWidget* StartWindow::makeMruPanel()
{
    return makePanel(LayoutV({
        makeHeader(tr("Open Recent Schema")),
        Stretch()
    }).boxLayout());
}

QWidget* StartWindow::makeTipsPanel()
{
    return makePanel(LayoutV({
        makeHeader(tr("Whether You Know What")),
        Stretch()
    }).boxLayout());
}

QWidget* StartWindow::makeToolsPanel()
{
    return makePanel(LayoutV({
        makeHeader(tr("Tools")),
        makeButton(":/toolbar/gauss_calculator", tr("Gauss Calculator"), SLOT(toolGaussCalc())),
        makeButton(":/toolbar/protocol", tr("Edit Stylesheet"), SLOT(editStyleSheet())),
        Stretch()
    }).boxLayout());
}

QWidget* StartWindow::makePanel(QBoxLayout* layout)
{
    auto panel = new QWidget;
    panel->setProperty("role", "panel");
    panel->setLayout(layout);
    return panel;
}

QWidget* StartWindow::makeHeader(const QString& title)
{
    auto label = new QLabel(title);
    label->setProperty("role", "header");
//    auto font = label->font();
//    font.setPointSize(font.pointSize() + 3);
//    font.setBold(true);
//    label->setFont(font);
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

void StartWindow::editStyleSheet()
{
    auto editor = new QPlainTextEdit;
    auto font = editor->font();
    Z::Gui::adjustCodeEditorFont(font);
    editor->setFont(font);

    editor->setPlainText(this->styleSheet());
    auto applyButton = new QPushButton("Apply");
    connect(applyButton, &QPushButton::clicked, [this, editor]{
         this->setStyleSheet(editor->toPlainText());
    });

    auto wnd = LayoutV({
        editor,
        LayoutH({
            Stretch(),
            applyButton
        }).setMargin(6)
    }).setMargin(3).setSpacing(0).makeWidget();
    wnd->setAttribute(Qt::WA_DeleteOnClose);
    wnd->setWindowTitle("Stylesheet Editor");
    wnd->setWindowIcon(QIcon(":/toolbar/protocol"));
    wnd->resize(600, 600);
    wnd->show();
}
