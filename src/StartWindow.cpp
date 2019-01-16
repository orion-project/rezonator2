#include "StartWindow.h"
#include "CommonData.h"
#include "ProjectWindow.h"
#include "GaussCalculatorWindow.h"
#include "core/CommonTypes.h"
#include "tools/OriMruList.h"
#include "widgets/Appearance.h"
#include "helpers/OriWindows.h"
#include "helpers/OriLayouts.h"

#include <QApplication>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QResource>
#include <QToolButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStyleOption>

using namespace Ori::Layouts;

namespace {
    const int TIP_IMG_PREVIEW_H = 100;
    const int TIP_IMG_PREVIEW_W = 200;
    const int MAX_MRU_PATH_LEN = 60;
    QJsonArray __tips;
}

MruItemWidget::MruItemWidget(const QFileInfo& fileInfo) : QWidget()
{
    auto labelFileName = new QLabel(fileInfo.baseName());
    labelFileName->setProperty("role", "mru_file_name");

    auto filePath = fileInfo.filePath();
    auto labelFilePath = new QLabel;
    if (filePath.length() > MAX_MRU_PATH_LEN)
    {
        filePath = "..." + filePath.right(MAX_MRU_PATH_LEN);
        labelFilePath->setToolTip(fileInfo.filePath());
    }
    labelFilePath->setText(filePath);
    labelFilePath->setProperty("role", "mru_file_path");

    LayoutV({labelFileName, labelFilePath}).setMargin(5).setSpacing(0).useFor(this);

    QSizePolicy policy;
    policy.setControlType(QSizePolicy::ToolButton);
    setSizePolicy(policy);
}

// Paint event should overriden to apply styles heets
void MruItemWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
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
    auto items = CommonData::instance()->mruList()->items();
    if (items.isEmpty())
        return mruPanelEmpty();

    QVector<QFileInfo> files;
    for (const QString& item : items)
    {
        QFileInfo file(item);
        if (file.exists()) files << file;
    }
    if (files.isEmpty())
        return mruPanelEmpty();

    auto layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    for (const QFileInfo& file : files)
        layout->addWidget(new MruItemWidget(file));

    auto mruWidget = new QWidget;
    mruWidget->setLayout(layout);
    mruWidget->setObjectName("mru_widget");

    auto mruScroll = new QScrollArea;
    mruScroll->setWidget(mruWidget);
    mruScroll->horizontalScrollBar()->setDisabled(true);
    mruScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    return panel(LayoutV({
         header(tr("Recent")),
         mruScroll,
         Stretch()
    }).boxLayout());
}

QWidget* StartWindow::mruPanelEmpty()
{
    auto emptyLabel = new QLabel(tr("There are no recently opened files yet."));
    emptyLabel->setObjectName("mru_empty_stub");

    return panel(LayoutV({
        header(tr("Recent")),
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
            header(tr("Tips")),
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
