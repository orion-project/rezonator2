#include "StartWindow.h"

#include "../app/Appearance.h"
#include "../app/AppSettings.h"
#include "../app/HelpSystem.h"
#include "../app/PersistentState.h"
#include "../app/ProjectOperations.h"
#include "../core/CommonTypes.h"
#include "../tools/CalculatorWindow.h"
#include "../tools/GaussCalculatorWindow.h"
#include "../tools/LensmakerWindow.h"
#include "../windows/ProjectWindow.h"

#include "helpers/OriWindows.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriDialogs.h"
#include "tools/OriMruList.h"
#include "widgets/OriLabels.h"
#include "widgets/OriFlatToolBar.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFileInfo>
#include <QInputDialog>
#include <QJsonDocument>
#include <QLabel>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRandomGenerator>
#include <QResource>
#include <QScrollArea>
#include <QScrollBar>
#include <QStyleOption>
#include <QTimer>
#include <QToolButton>
#include <QMovie>

using namespace Ori::Layouts;

namespace {

void adjustTipImagePosition(QLabel* tipImage)
{
    if (!tipImage->isVisible()) return;

    auto parent = qobject_cast<QWidget*>(tipImage->parent());
    if (!parent) return;

    auto parentSize = parent->size();
    auto labelSize = tipImage->size();
    tipImage->move(parentSize.width() - labelSize.width() - 10,
                   parentSize.height() - labelSize.height() - 10);
}

QWidget* makeHeader(const QString& title)
{
    auto label = new QLabel(title);
    label->setProperty("role", "header");
    return label;
}

template<class Obj, typename Func>
QWidget* makeButton(const QString& iconPath, const QString& title, Obj* receiver, Func slot)
{
    auto button = new QToolButton;
    button->setProperty("role", "command");
    button->setIconSize(QSize(24, 24));
    button->setIcon(QIcon(iconPath));
    button->setText(title);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->connect(button, &QToolButton::clicked, receiver, std::move(slot));
    return button;
}

} // namespace

//------------------------------------------------------------------------------
//                               CustomCssWidget
//------------------------------------------------------------------------------

void CustomCssWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//------------------------------------------------------------------------------
//                               MruStartItem
//------------------------------------------------------------------------------

MruStartItem::MruStartItem(const QFileInfo& fileInfo)
{
    _filePath = fileInfo.filePath();

    // There is some weird logic when Qt calculates the size of scroll area,
    // so we need to add some spaces to accout width of vertical scroll bar.
    _displayFilePath = _filePath + "      ";

    auto labelFileName = new QLabel(fileInfo.completeBaseName());
    labelFileName->setProperty("role", "mru_file_name");

    _filePathLabel = new QLabel;
    _filePathLabel->setText(_displayFilePath);
    _filePathLabel->setProperty("role", "mru_file_path");

    LayoutV({labelFileName, _filePathLabel}).setMargin(5).setSpacing(0).useFor(this);
}

void MruStartItem::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    QFontMetrics metrix(_filePathLabel->font());
    int width = _filePathLabel->width();
    if (metrix.horizontalAdvance(_filePath) > width)
    {
        _filePathLabel->setText(metrix.elidedText(_filePath, Qt::ElideLeft, width));
        setToolTip(_filePath);
    }
    else
    {
        _filePathLabel->setText(_displayFilePath);
        setToolTip("");
    }
}

void MruStartItem::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);

    emit onFileOpen(_filePath);
}

//------------------------------------------------------------------------------
//                              MruStartPanel
//------------------------------------------------------------------------------

MruStartPanel::MruStartPanel() : CustomCssWidget()
{
    setProperty("role", "panel");
    setObjectName("panel_mru");

    auto mruList = ProjectWindow::createMruList(this);

    auto items = mruList->items();
    if (items.isEmpty())
    {
        makeEmpty();
        return;
    }

    QVector<QFileInfo> files;
    for (const QString& item : std::as_const(items))
    {
        QFileInfo file(item);
        if (file.exists()) files << file;
    }
    if (files.isEmpty())
    {
        makeEmpty();
        return;
    }

    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    foreach (const QFileInfo& file, files)
    {
        auto itemWidget = new MruStartItem(file);
        connect(itemWidget, &MruStartItem::onFileOpen, this, &MruStartPanel::openFile);
        layout->addWidget(itemWidget);
    }
    layout->addStretch();

    auto mruWidget = new QWidget;
    mruWidget->setLayout(layout);
    mruWidget->setObjectName("mru_widget");
    mruWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);

    auto mruScroll = new QScrollArea;
    mruScroll->setObjectName("mru_scroll_area");
    mruScroll->setWidget(mruWidget);
    mruScroll->horizontalScrollBar()->setDisabled(true);
    mruScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mruScroll->setWidgetResizable(true);

    LayoutV({
         makeHeader(tr("Recent")),
         mruScroll,
         Stretch()
    }).setMargin(10).setSpacing(10).useFor(this);
}

void MruStartPanel::makeEmpty()
{
    auto emptyLabel = new QLabel(tr("There are no recently opened files yet."));
    emptyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    emptyLabel->setObjectName("mru_empty_stub");

    LayoutV({
        makeHeader(tr("Recent")),
        Stretch(),
        emptyLabel,
        Stretch(),
    }).useFor(this);
}

void MruStartPanel::openFile(const QString& fileName)
{
    ProjectWindow::openProject(fileName, {.addToMru = true});

    // close _after_ work, otherwise signal handler can crash
    emit closeRequested();
}

//------------------------------------------------------------------------------
//                             TipsStartPanel
//------------------------------------------------------------------------------

TipsStartPanel::TipsStartPanel(QLabel *tipImage) : CustomCssWidget()
{
    setProperty("role", "panel");
    setObjectName("panel_tips");

    _tipText = new QLabel;
    _tipText->setObjectName("tip_text");
    _tipText->setWordWrap(true);
    connect(_tipText, &QLabel::linkActivated, [](const QString& link){
        QDesktopServices::openUrl(link);
    });

    _tipImage = tipImage;
    _tipImage->setVisible(false);

    auto tipPreview = new Ori::Widgets::Label;
    tipPreview->setObjectName("tip_preview");
    tipPreview->setVisible(false);
    tipPreview->setCursor(Qt::PointingHandCursor);
    tipPreview->setToolTip(tr("Click to enlarge"));
    connect(tipPreview, &Ori::Widgets::Label::clicked, this, &TipsStartPanel::enlargePreview);
    _tipPreview = tipPreview;

    auto buttonsLayout = new QHBoxLayout;
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(5);

    auto prevButton = new QToolButton;
    prevButton->setProperty("role", "tip_button");
    prevButton->setText(tr("Prev Tip"));
    prevButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    connect(prevButton, &QToolButton::clicked, this, &TipsStartPanel::showPrevTip);
    buttonsLayout->addWidget(prevButton);

    auto nextButton = new QToolButton;
    nextButton->setProperty("role", "tip_button");
    nextButton->setText(tr("Next Tip"));
    nextButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    connect(nextButton, &QToolButton::clicked, this, &TipsStartPanel::showNextTip);
    buttonsLayout->addWidget(nextButton);

    if (AppSettings::instance().isDevMode)
    {
        auto chooseTipButton = new QToolButton;
        chooseTipButton->setProperty("role", "tip_button");
        chooseTipButton->setText(tr("Choose Tip"));
        chooseTipButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
        connect(chooseTipButton, &QToolButton::clicked, this, &TipsStartPanel::chooseAndShowTip);
        buttonsLayout->addWidget(chooseTipButton);
    }

    buttonsLayout->addStretch();

    _enlargeTip = new Ori::Widgets::Label(tr("Click to enlarge →"));
    _enlargeTip->setCursor(Qt::PointingHandCursor);
    buttonsLayout->addWidget(_enlargeTip);
    connect(_enlargeTip, &Ori::Widgets::Label::clicked, this, &TipsStartPanel::enlargePreview);

    LayoutH({
        LayoutV({
            makeHeader(tr("You know what?")),
            _tipText,
            Stretch(),
            buttonsLayout,
        }).setMargin(0).setSpacing(10),
        _tipPreview
    }).setMargin(10).setSpacing(10).useFor(this);

    loadTips();
    showNextTip();
}

// TODO: when will be a lot of tips: check if it takes a while and move to separate thread
void TipsStartPanel::loadTips()
{
    QFile file(":/tips/list");
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok)
    {
        qWarning() << "Unable to load tips from resources" << file.errorString();
        return;
    }
    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (doc.isNull())
    {
        _tipText->setText("ERROR: Unable to load tips from resources: " + error.errorString());
        return;
    }
    _tips = doc.object();

    // Randomize tips order
    auto keys = _tips.keys();
    while (!keys.isEmpty())
    {
        int index = QRandomGenerator::system()->generate() % keys.size();
        auto key = keys.at(index);
        keys.removeAt(index);
        auto tip = _tips[key].toObject();
        if (tip["todo"].toBool()) continue;
        _ids << key;
    }
    _index = _ids.isEmpty() ? -1 : 0;
}

void TipsStartPanel::showNextTip()
{
    if (_index < 0) return;

    _index++;
    if (_index > _ids.size()-1) _index = 0;

    showTip(_tips[_ids.at(_index)].toObject());
}

void TipsStartPanel::showPrevTip()
{
    if (_index < 0) return;

    _index--;
    if (_index < 0) _index = _ids.size()-1;

    showTip(_tips[_ids.at(_index)].toObject());
}

void TipsStartPanel::showTip(const QJsonObject &tip)
{
    static QString tipImagesPath(":/tips/");

    if (_tipImage->isVisible()) closeTipImage();

    auto imageFile = tip["image"].toString();
    if (!imageFile.isEmpty())
    {
        auto previewFile = tip["preview"].toString();
        if (previewFile.isEmpty())
            previewFile = imageFile;
        QString previewPath = tipImagesPath + previewFile;
        auto preview = QPixmap(previewPath);
        if (preview.isNull())
            qWarning() << "Tip image not found" << previewPath;
        if (preview.width() != TIP_IMG_PREVIEW_W ||
            preview.height() != TIP_IMG_PREVIEW_H)
            preview = preview.scaled(TIP_IMG_PREVIEW_W,
                                     TIP_IMG_PREVIEW_H,
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation);
        _tipPreview->setPixmap(preview);
        _tipPreview->setVisible(true);
        _enlargeTip->setVisible(true);
        _imagePath = tipImagesPath + imageFile;
    }
    else
    {
        _enlargeTip->setVisible(false);
        _tipPreview->setVisible(false);
        _tipPreview->clear();
        _imagePath.clear();
    }

    _tipText->setText(tip["text"].toString());
}

void TipsStartPanel::enlargePreview()
{
    static QString gif(".gif");
    if (_imagePath.endsWith(gif, Qt::CaseInsensitive))
    {
        if (!_movie)
        {
            _movie = new QMovie(this);
            connect(_movie, &QMovie::started, this, &TipsStartPanel::handleMovieStarted);
        }
        _movie->setFileName(_imagePath);
        if (_movie->isValid())
        {
            _tipImage->setMovie(_movie);
            _movie->start();
        }
        else qWarning() << "Tip image not found" << _imagePath;
    }
    else
    {
        QPixmap pixmap(_imagePath);
        if (!pixmap.isNull())
        {
            _tipImage->setPixmap(pixmap);
            showTipImage(pixmap);
        }
        else qWarning() << "Tip image not found" << _imagePath;
    }
}

void TipsStartPanel::handleMovieStarted()
{
    showTipImage(_movie->currentPixmap());
}

void TipsStartPanel::showTipImage(const QPixmap& pixmap)
{
    auto imageSize = pixmap.size();
    _tipImage->resize(imageSize.width() + 20, imageSize.height() + 20);
    _tipImage->setVisible(true);
    adjustTipImagePosition(_tipImage);
}

void TipsStartPanel::closeTipImage()
{
    if (_movie) _movie->stop();
    _tipImage->setVisible(false);
    _tipImage->clear();
}

void TipsStartPanel::chooseAndShowTip()
{
    auto id = QInputDialog::getText(this, "", "Enter tip id");
    if (id.isEmpty()) return;
    showTip(_tips[id].toObject());
}

//------------------------------------------------------------------------------
//                            ActionsStartPanel
//------------------------------------------------------------------------------

ActionsStartPanel::ActionsStartPanel() : CustomCssWidget()
{
    setProperty("role", "panel");
    setObjectName("panel_actions");

    LayoutV({
        makeHeader(tr("Open")),
        makeButton(":/toolbar/schema_open", tr("Open Schema File"), this, &ActionsStartPanel::openSchemaFile),
        makeButton(":/toolbar/schema_sample", tr("Open Example"), this, &ActionsStartPanel::openSchemaExample),
        makeHeader(tr("Create")),
        makeButton(TripTypes::info(TripType::SW).iconPath(), tr("Standing-Wave Resonator"), this, &ActionsStartPanel::makeSchemaSW),
        makeButton(TripTypes::info(TripType::RR).iconPath(), tr("Ring Resonator"), this, &ActionsStartPanel::makeSchemaRR),
        makeButton(TripTypes::info(TripType::SP).iconPath(), tr("Single-Pass System"), this, &ActionsStartPanel::makeSchemaSP),
        Stretch()
    }).setMargin(10).setSpacing(10).useFor(this);
};

void ActionsStartPanel::openSchemaFile()
{
    auto fileName = ProjectOperations::getOpenFileName(this);
    if (fileName.isEmpty()) return;
    ProjectWindow::openProject(fileName, {.addToMru = true});
    emit closeRequested();
}

void ActionsStartPanel::openSchemaExample()
{
    auto fileName = ProjectOperations::selectSchemaExample();
    if (fileName.isEmpty()) return;
    ProjectWindow::openProject(fileName, {.isExample = true});

    // close _after_ work, otherwise signal handler can crash
    emit closeRequested();
}

void ActionsStartPanel::makeSchemaSW()
{
    ProjectWindow::createProject(TripType::SW);

    // close _after_ work, otherwise signal handler can crash
    emit closeRequested();
}

void ActionsStartPanel::makeSchemaRR()
{
    ProjectWindow::createProject(TripType::RR);

    // close _after_ work, otherwise signal handler can crash
    emit closeRequested();
}

void ActionsStartPanel::makeSchemaSP()
{
    ProjectWindow::createProject(TripType::SP);

    // close _after_ work, otherwise signal handler can crash
    emit closeRequested();
}

//------------------------------------------------------------------------------
//                               ToolsStartPanel
//------------------------------------------------------------------------------

ToolsStartPanel::ToolsStartPanel() : CustomCssWidget()
{
    setProperty("role", "panel");
    setObjectName("panel_tools");

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);
    layout->addWidget(makeHeader(tr("Tools")));
    layout->addWidget(makeButton(":/toolbar/gauss_calculator", tr("Gauss Calculator"), this, []{ GaussCalculatorWindow::showWindow(); }));
    layout->addWidget(makeButton(":/window_icons/calculator", tr("Formula Calculator"), this, []{ CalculatorWindow::showWindow(); }));
    layout->addWidget(makeButton(":/window_icons/lens", tr("Lensmaker"), this, []{ LensmakerWindow::showWindow(); }));
    //layout->addWidget(makeButton(":/toolbar/iris", tr("Iris"), this, []{ IrisWindow::showWindow(); }));
    layout->addWidget(makeButton(":/toolbar/help", tr("Manual"), this, []{ Z::HelpSystem::instance()->showContents(); }));
    layout->addWidget(makeButton(":/toolbar/update", tr("Check Updates"), this, []{ Z::HelpSystem::instance()->checkUpdates(); }));

    layout->addStretch();
}

//------------------------------------------------------------------------------
//                               StartWindow
//------------------------------------------------------------------------------

StartWindow::StartWindow(QWidget *parent) : QWidget(parent)
{
    setObjectName("start_window");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(qApp->applicationName());
    Ori::Wnd::setWindowIcon(this, ":/window_icons/main");

    auto tipImage = new Ori::Widgets::Label;
    tipImage->setObjectName("tip_image");
    tipImage->setCursor(Qt::PointingHandCursor);
    _tipImage = tipImage;

    auto actionsPanel = new ActionsStartPanel;
    connect(actionsPanel, &ActionsStartPanel::closeRequested, this, &StartWindow::close);

    auto mruPanel = new MruStartPanel;
    connect(mruPanel, &MruStartPanel::closeRequested, this, &StartWindow::close);

    auto toolsPanel = new ToolsStartPanel;

    auto tipsPanel = new TipsStartPanel(tipImage);

    connect(tipImage, &Ori::Widgets::Label::clicked, this, [tipsPanel]{
        tipsPanel->closeTipImage();
    });

    LayoutH({
        Stretch(),
        LayoutV({
            Stretch(),
            LayoutH({
                Stretch(),
                actionsPanel,
                mruPanel,
                toolsPanel,
                Stretch(),
            }).setSpacing(20),
            tipsPanel,
            Stretch(),
        }).setSpacing(20),
        Stretch(),
    }).setMargin(60).useFor(this);


    _toolbar = new Ori::Widgets::FlatToolBar;
    _toolbar->setIconSize({24, 24});
    if (AppSettings::instance().isDevMode)
        _toolbar->addAction(QIcon(":/toolbar/palette"), tr("Edit App Style Sheet"), this, []{ Z::Gui::editAppStyleSheet(); });
    _toolbar->addAction(QIcon(":/toolbar/settings"), tr("Settings"), this, []{ AppSettings::instance().edit(); });
    //_toolbar->addAction(QIcon(":/toolbar/help"), tr("Show Manual"), []{ Z::HelpSystem::instance()->showContents(); });
    _toolbar->addAction(QIcon(":/toolbar/info"), tr("About"), []{ Z::HelpSystem::instance()->showAbout(); });
    _toolbar->setParent(this);
    _toolbar->adjustSize();

    // Should be after all widgets to overlay them
    tipImage->setParent(this);

    PersistentState::restoreWindowGeometry("start", this, {950, 700});
    
    Z::HelpSystem::instance()->checkUpdatesAuto();
}

StartWindow::~StartWindow()
{
    PersistentState::storeWindowGeometry("start", this);
}

void StartWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    adjustTipImagePosition(_tipImage);

    _toolbar->move(event->size().width() - _toolbar->width() - 10, 10);
}
