#include "StartWindow.h"
#include "ProjectWindow.h"
#include "GaussCalculatorWindow.h"
#include "core/CommonTypes.h"
#include "widgets/Appearance.h"
#include "helpers/OriWindows.h"
#include "helpers/OriLayouts.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QResource>
#include <QToolButton>

using namespace Ori::Layouts;

namespace {
    const int TIP_IMG_PREVIEW_H = 100;
    const int TIP_IMG_PREVIEW_W = 200;
    QJsonArray __tips;
}

StartWindow::StartWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle(qApp->applicationName());
    Ori::Wnd::setWindowIcon(this, ":/window_icons/main");
    setAttribute(Qt::WA_DeleteOnClose);

    LayoutH({
        Stretch(),
        LayoutV({
            Stretch(),
            LayoutH({
                Stretch(),
                actionsPanel(),
                mruPanel(),
                toolsPanel(),
                Stretch(),
            }).setSpacing(20),
            tipsPanel(),
            Stretch(),
        }).setSpacing(20),
        Stretch(),
    }).setMargin(20).useFor(this);

    setStyleSheet(QString::fromLatin1(reinterpret_cast<const char*>(
        QResource(":/style/StartWindow").data())));

    showNextTip();
}

QWidget* StartWindow::actionsPanel()
{
    return panel(LayoutV({
        header(tr("Actions")),
        button(":/toolbar/schema_open", tr("Open Schema File"), nullptr),
        button(":/toolbar/schema_open", tr("Open Example Schema"), nullptr), // TODO make different icon
        button(TripTypes::info(TripType::SW).iconPath(), tr("Make Standing-Wave Resonator"), SLOT(makeSchemaSW())),
        button(TripTypes::info(TripType::RR).iconPath(), tr("Make Ring Resonator"), SLOT(makeSchemaRR())),
        button(TripTypes::info(TripType::SP).iconPath(), tr("Make Single-Pass System"), SLOT(makeSchemaSP())),
        Stretch()
    }).boxLayout());
}

QWidget* StartWindow::mruPanel()
{
    auto emptyLabel = new QLabel(tr("There are no recently opened files yet."));
    //emptyLabel->setWordWrap(true);
    emptyLabel->setObjectName("mru_empty_stub");
    emptyLabel->setAlignment(Qt::AlignHCenter);

    return panel(LayoutV({
        header(tr("Open Recent Schema")),
        Stretch(),
        emptyLabel,
        Stretch(),
    }).boxLayout());
}

QWidget* StartWindow::tipsPanel()
{
    QJsonParseError error;
    QByteArray data = reinterpret_cast<const char*>(QResource(":/tips/list").data());
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (doc.isNull())
    {
        qWarning() << "Unable to load tips from resources" << error.errorString();
        return new QWidget;
    }
    __tips = doc.object()["items"].toArray();
    if (__tips.isEmpty())
    {
        qWarning() << "Unable to get array of tips from resources";
        return new QWidget;
    }

    _tipText = new QLabel;
    _tipText->setObjectName("tip_text");
    _tipText->setWordWrap(true);

    _tipPreview = new QLabel;

    auto prevButton = new QToolButton;
    prevButton->setProperty("role", "tip_button");
    prevButton->setText(tr("Prev Tip"));
    prevButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    connect(prevButton, SIGNAL(clicked()), this, SLOT(showPrevTip()));

    auto nextButton = new QToolButton;
    nextButton->setProperty("role", "tip_button");
    nextButton->setText(tr("Next Tip"));
    nextButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    connect(nextButton, SIGNAL(clicked()), this, SLOT(showNextTip()));

    return panel(LayoutH({
        LayoutV({
            header(tr("Whether You Know What")),
            _tipText,
            Stretch(),
            LayoutH({
                prevButton,
                nextButton,
                Stretch()
            }),
        }),
        _tipPreview
    }).boxLayout());
}

QWidget* StartWindow::toolsPanel()
{
    return panel(LayoutV({
        header(tr("Tools")),
        button(":/toolbar/gauss_calculator", tr("Gauss Calculator"), SLOT(showGaussCalculator())),
        button(":/toolbar/protocol", tr("Edit Stylesheet"), SLOT(editStyleSheet())),
        button(":/toolbar/options", tr("Edit Settings"), nullptr),
        Stretch()
    }).boxLayout());
}

QWidget* StartWindow::panel(QBoxLayout* layout)
{
    auto panel = new QWidget;
    panel->setProperty("role", "panel");
    panel->setLayout(layout);
    return panel;
}

QWidget* StartWindow::header(const QString& title)
{
    auto label = new QLabel(title);
    label->setProperty("role", "header");
    return label;
}

QWidget* StartWindow::button(const QString& iconPath, const QString& title, const char* slot)
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

void StartWindow::showGaussCalculator()
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

namespace {
    void makeSchema(TripType tripType)
    {
        auto s = new Schema();
        s->events().disable();
        s->setTripType(tripType);
        s->events().enable();
        (new ProjectWindow(s))->show();
    }
}

void StartWindow::makeSchemaSW()
{
    close();
    makeSchema(TripType::SW);
}

void StartWindow::makeSchemaRR()
{
    close();
    makeSchema(TripType::RR);
}

void StartWindow::makeSchemaSP()
{
    close();
    makeSchema(TripType::SP);
}

void StartWindow::showNextTip()
{
    if (__tips.isEmpty()) return;

    // TODO
    showTip(__tips.first().toObject());
}

void StartWindow::showPrevTip()
{
    if (__tips.isEmpty()) return;

    // TODO
}

void StartWindow::showTip(const QJsonObject& tip)
{
    auto imagePath = tip["image"].toString();
    if (!imagePath.isEmpty())
    {
        auto preview = QPixmap(imagePath)
                .scaled(TIP_IMG_PREVIEW_W,
                        TIP_IMG_PREVIEW_H,
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation);
        _tipPreview->setPixmap(preview);
    }
    else _tipPreview->clear();

    _tipText->setText(tip["text"].toString());
}
