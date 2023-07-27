#include "StartWindow.h"

#include "Appearance.h"
#include "AppSettings.h"
#include "CalculatorWindow.h"
#include "CommonData.h"
#include "GaussCalculatorWindow.h"
#include "GrinLensWindow.h"
#include "HelpSystem.h"
#include "LensmakerWindow.h"
#include "ProjectOperations.h"
#include "ProjectWindow.h"
#include "core/CommonTypes.h"

#include "helpers/OriWindows.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriTheme.h"
#include "helpers/OriDialogs.h"
#include "tools/OriMruList.h"
#include "tools/OriSettings.h"
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
}

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
//                               StartPanel
//------------------------------------------------------------------------------

StartPanel::StartPanel(const QString& objectName)
{
    setProperty("role", "panel");
    setObjectName(objectName);
}

QWidget* StartPanel::makeHeader(const QString& title)
{
    auto label = new QLabel(title);
    label->setProperty("role", "header");
    return label;
}

QWidget* StartPanel::makeButton(const QString& iconPath, const QString& title, const char* slot)
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

//------------------------------------------------------------------------------
//                              MruStartPanel
//------------------------------------------------------------------------------

MruStartPanel::MruStartPanel() : StartPanel("panel_mru")
{
    auto items = CommonData::instance()->mruList()->items();
    if (items.isEmpty())
    {
        makeEmpty();
        return;
    }

    QVector<QFileInfo> files;
    for (const QString& item : items)
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

void MruStartPanel::openFile(const QString& filePath)
{
    auto projectWindow = new ProjectWindow(new Schema());
    projectWindow->show();
    projectWindow->operations()->openSchemaFile(filePath);
    CommonData::instance()->mruList()->append(filePath);
    emit onClose();
}

//------------------------------------------------------------------------------
//                             TipsStartPanel
//------------------------------------------------------------------------------

TipsStartPanel::TipsStartPanel(QLabel *tipImage) : StartPanel("panel_tips")
{
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
        if (preview.width() != TIP_IMG_PREVIEW_W or
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
        _tipImage->setMovie(_movie);
        _movie->start();
    }
    else
    {
        QPixmap pixmap(_imagePath);
        _tipImage->setPixmap(pixmap);
        showTipImage(pixmap);
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

ActionsStartPanel::ActionsStartPanel() : StartPanel("panel_actions")
{
    LayoutV({
        makeHeader(tr("Open")),
        makeButton(":/toolbar/schema_open", tr("Open Schema File"), SLOT(openSchemaFile())),
        makeButton(":/toolbar/schema_sample", tr("Open Example"), SLOT(openSchemaExample())),
        makeHeader(tr("Create")),
        makeButton(TripTypes::info(TripType::SW).iconPath(), tr("Standing-Wave Resonator"), SLOT(makeSchemaSW())),
        makeButton(TripTypes::info(TripType::RR).iconPath(), tr("Ring Resonator"), SLOT(makeSchemaRR())),
        makeButton(TripTypes::info(TripType::SP).iconPath(), tr("Single-Pass System"), SLOT(makeSchemaSP())),
        Stretch()
    }).setMargin(10).setSpacing(10).useFor(this);
};

void ActionsStartPanel::openSchemaFile()
{
    auto fileName = ProjectOperations::getOpenFileName(this);
    if (fileName.isEmpty()) return;
    emit onClose();
    auto projectWindow = new ProjectWindow(new Schema());
    projectWindow->show();
    projectWindow->operations()->openSchemaFile(fileName);
}

void ActionsStartPanel::openSchemaExample()
{
    auto fileName = ProjectOperations::selectSchemaExample();
    if (fileName.isEmpty()) return;
    emit onClose();
    auto projectWindow = new ProjectWindow(new Schema());
    projectWindow->show();
    projectWindow->operations()->openExampleFile(fileName);
}

namespace {
void makeNewSchema(TripType tripType)
{
    (new ProjectWindow(ProjectOperations::createDefaultSchema(tripType)))->show();
}
}

void ActionsStartPanel::makeSchemaSW()
{
    makeNewSchema(TripType::SW);
    emit onClose();
}

void ActionsStartPanel::makeSchemaRR()
{
    makeNewSchema(TripType::RR);
    emit onClose();
}

void ActionsStartPanel::makeSchemaSP()
{
    makeNewSchema(TripType::SP);
    emit onClose();
}

//------------------------------------------------------------------------------
//                               ToolsStartPanel
//------------------------------------------------------------------------------

ToolsStartPanel::ToolsStartPanel() : StartPanel("panel_tools")
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);
    layout->addWidget(makeHeader(tr("Tools")));
    layout->addWidget(makeButton(":/toolbar/gauss_calculator", tr("Gauss Calculator"), SLOT(showGaussCalculator())));
    layout->addWidget(makeButton(":/window_icons/calculator", tr("Formula Calculator"), SLOT(showCalculator())));
    layout->addWidget(makeButton(":/window_icons/lens", tr("Lensmaker"), SLOT(showLensmaker())));
    layout->addWidget(makeButton(":/toolbar/settings", tr("Settings"), SLOT(editSettings())));
    layout->addWidget(makeButton(":/toolbar/help", tr("Manual"), SLOT(showManual())));
    layout->addWidget(makeButton(":/toolbar/update", tr("Check Updates"), SLOT(checkUpdates())));
    layout->addStretch();
}

void ToolsStartPanel::showGaussCalculator()
{
    GaussCalculatorWindow::showWindow();
}

void ToolsStartPanel::showCalculator()
{
    CalculatorWindow::showWindow();
}

void ToolsStartPanel::showGrinLens()
{
    GrinLensWindow::showWindow();
}

void ToolsStartPanel::checkUpdates()
{
    Z::HelpSystem::instance()->checkUpdates();
}

void ToolsStartPanel::showLensmaker()
{
    LensmakerWindow::showWindow();
}

void ToolsStartPanel::showManual()
{
    Z::HelpSystem::instance()->showContents();
}

void ToolsStartPanel::editSettings()
{
    AppSettings::instance().edit();
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
    connect(actionsPanel, &ActionsStartPanel::onClose, this, &StartWindow::close);

    auto mruPanel = new MruStartPanel;
    connect(mruPanel, &ActionsStartPanel::onClose, this, &StartWindow::close);

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
    }).setMargin(20).useFor(this);


    _toolbar = new Ori::Widgets::FlatToolBar;
    _toolbar->setIconSize({24, 24});
    if (AppSettings::instance().isDevMode)
        _toolbar->addAction(QIcon(":/toolbar/protocol"), tr("Edit Stylesheet"), this, &StartWindow::editStyleSheet);
    //_toolbar->addAction(QIcon(":/toolbar/help"), tr("Show Manual"), []{ Z::HelpSystem::instance()->showContents(); });
    _toolbar->addAction(QIcon(":/toolbar/info"), tr("About"), []{ Z::HelpSystem::instance()->showAbout(); });
    _toolbar->setParent(this);
    _toolbar->adjustSize();

    // Should be after all widgets to overlay them
    tipImage->setParent(this);

    Ori::Settings s;
    s.beginGroup("View");
    s.restoreWindowGeometry("startWindow", this);
}

StartWindow::~StartWindow()
{
    Ori::Settings s;
    s.beginGroup("View");
    s.storeWindowGeometry("startWindow", this);
}

void StartWindow::editStyleSheet()
{
    auto editor = new QPlainTextEdit;
    editor->setFont(Z::Gui::CodeEditorFont().get());
    editor->setPlainText(Ori::Theme::loadRawStyleSheet());

    auto applyButton = new QPushButton("Apply");
    connect(applyButton, &QPushButton::clicked, [editor]{
        qApp->setStyleSheet(Ori::Theme::makeStyleSheet(editor->toPlainText()));
    });

    auto saveButton = new QPushButton("Save");
    connect(saveButton, &QPushButton::clicked, [editor]{
        auto res = Ori::Theme::saveRawStyleSheet(editor->toPlainText());
        if (!res.isEmpty()) Ori::Dlg::error(res);
    });

    auto wnd = LayoutV({
        editor,
        LayoutH({
            Stretch(),
            applyButton,
            saveButton,
        }).setMargin(6)
    }).setMargin(3).setSpacing(0).makeWidget();
    wnd->setAttribute(Qt::WA_DeleteOnClose);
    wnd->setWindowTitle("Stylesheet Editor");
    wnd->setWindowIcon(QIcon(":/toolbar/protocol"));
    wnd->resize(600, 600);
    wnd->show();
}

void StartWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    adjustTipImagePosition(_tipImage);

    _toolbar->move(event->size().width() - _toolbar->width() - 10, 10);
}
