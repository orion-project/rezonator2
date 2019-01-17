#include "StartWindow.h"

#include "AppSettingsDialog.h"
#include "CommonData.h"
#include "GaussCalculatorWindow.h"
#include "ProjectOperations.h"
#include "ProjectWindow.h"
#include "core/CommonTypes.h"
#include "widgets/Appearance.h"

#include "helpers/OriWindows.h"
#include "helpers/OriLayouts.h"
#include "tools/OriMruList.h"
#include "tools/OriSettings.h"
#include "widgets/OriLabels.h"

#include <QApplication>
#include <QFileInfo>
#include <QJsonDocument>
#include <QLabel>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QResource>
#include <QScrollArea>
#include <QScrollBar>
#include <QStyleOption>
#include <QToolButton>
#include <QMovie>

using namespace Ori::Layouts;

namespace {
    void makeSchema(TripType tripType)
    {
        auto s = new Schema();
        s->events().disable();
        s->setTripType(tripType);
        s->events().enable();
        (new ProjectWindow(s, QString()))->show();
    }

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
    opt.init(this);
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

    auto labelFileName = new QLabel(fileInfo.baseName());
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
    if (metrix.width(_filePath) > width)
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
    layout->setMargin(0);
    layout->setSpacing(0);
    for (const QFileInfo& file : files)
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
    emit onClose();
    (new ProjectWindow(nullptr, filePath))->show();
}

//------------------------------------------------------------------------------
//                             TipsStartPanel
//------------------------------------------------------------------------------

TipsStartPanel::TipsStartPanel(QLabel *tipImage) : StartPanel("panel_tips")
{
    _tipText = new QLabel;
    _tipText->setObjectName("tip_text");
    _tipText->setWordWrap(true);

    _tipImage = tipImage;
    _tipImage->setVisible(false);

    auto tipPreview = new Ori::Widgets::Label;
    tipPreview->setVisible(false);
    tipPreview->setCursor(Qt::PointingHandCursor);
    tipPreview->setToolTip(tr("Click to enlarge"));
    connect(tipPreview, &Ori::Widgets::Label::clicked, this, &TipsStartPanel::enlargePreview);
    _tipPreview = tipPreview;

    auto prevButton = new QToolButton;
    prevButton->setProperty("role", "tip_button");
    prevButton->setText(tr("Prev Tip"));
    prevButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    connect(prevButton, &QToolButton::clicked, this, &TipsStartPanel::showPrevTip);

    auto nextButton = new QToolButton;
    nextButton->setProperty("role", "tip_button");
    nextButton->setText(tr("Next Tip"));
    nextButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    connect(nextButton, &QToolButton::clicked, this, &TipsStartPanel::showNextTip);

    LayoutH({
        LayoutV({
            makeHeader(tr("Tips")),
            _tipText,
            Stretch(),
            LayoutH({
                prevButton,
                nextButton,
                Stretch()
            }).setMargin(0).setSpacing(5),
        }).setMargin(0).setSpacing(10),
        _tipPreview
    }).setMargin(10).setSpacing(10).useFor(this);

    loadTips();
    showNextTip();
}

// TODO: when will be a lot of tips: check if it takes a while and move to separate thread
void TipsStartPanel::loadTips()
{
    QJsonParseError error;
    QByteArray data = reinterpret_cast<const char*>(QResource(":/tips/list").data());
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
        int index = qrand() % keys.size();
        _ids << keys.at(index);
        keys.removeAt(index);
    }
    _index = _ids.isEmpty() ? -1 : 0;
}

void TipsStartPanel::showNextTip()
{
    if (_index < 0) return;

    _index++;
    if (_index > _ids.size()-1) _index = 0;

    showTip();
}

void TipsStartPanel::showPrevTip()
{
    if (_index < 0) return;

    _index--;
    if (_index < 0) _index = _ids.size()-1;

    showTip();
}

void TipsStartPanel::showTip()
{
    if (_tipImage->isVisible()) closeTipImage();

    auto tip = _tips[_ids.at(_index)].toObject();

    auto imageFile = tip["image"].toString();
    if (!imageFile.isEmpty())
    {
        static QString path(":/tips/");
        _imagePath = path + imageFile;
        auto preview = QPixmap(_imagePath)
                .scaled(TIP_IMG_PREVIEW_W,
                        TIP_IMG_PREVIEW_H,
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation);
        _tipPreview->setPixmap(preview);
        _tipPreview->setVisible(true);
    }
    else
    {
        _imagePath.clear();
        _tipPreview->clear();
        _tipPreview->setVisible(false);
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

//------------------------------------------------------------------------------
//                            ActionsStartPanel
//------------------------------------------------------------------------------

ActionsStartPanel::ActionsStartPanel() : StartPanel("panel_actions")
{
    LayoutV({
        makeHeader(tr("Actions")),
        makeButton(":/toolbar/schema_open", tr("Open Schema File"), SLOT(openSchemaFile())),
        makeButton(":/toolbar/schema_open", tr("Open Example Schema"), SLOT(openSchemaExample())), // TODO make different icon
        makeButton(TripTypes::info(TripType::SW).iconPath(), tr("Make Standing-Wave Resonator"), SLOT(makeSchemaSW())),
        makeButton(TripTypes::info(TripType::RR).iconPath(), tr("Make Ring Resonator"), SLOT(makeSchemaRR())),
        makeButton(TripTypes::info(TripType::SP).iconPath(), tr("Make Single-Pass System"), SLOT(makeSchemaSP())),
        Stretch()
    }).setMargin(10).setSpacing(10).useFor(this);
};

void ActionsStartPanel::openSchemaFile()
{
    auto fileName = ProjectOperations::getOpenFileName(this);
    if (fileName.isEmpty()) return;
    emit onClose();
    (new ProjectWindow(nullptr, fileName))->show();
}

void ActionsStartPanel::openSchemaExample()
{
    auto fileName = ProjectOperations::selectSchemaExample();
    if (fileName.isEmpty()) return;
    emit onClose();
    (new ProjectWindow(nullptr, fileName))->show();
}

void ActionsStartPanel::makeSchemaSW()
{
    emit onClose();
    makeSchema(TripType::SW);
}

void ActionsStartPanel::makeSchemaRR()
{
    emit onClose();
    makeSchema(TripType::RR);
}

void ActionsStartPanel::makeSchemaSP()
{
    emit onClose();
    makeSchema(TripType::SP);
}

//------------------------------------------------------------------------------
//                               ToolsStartPanel
//------------------------------------------------------------------------------

ToolsStartPanel::ToolsStartPanel() : StartPanel("panel_tools")
{
    LayoutV({
        makeHeader(tr("Tools")),
        makeButton(":/toolbar/gauss_calculator", tr("Gauss Calculator"), SLOT(showGaussCalculator())),
        makeButton(":/toolbar/protocol", tr("Edit Stylesheet"), SLOT(editStyleSheet())),
        makeButton(":/toolbar/options", tr("Edit Settings"), SLOT(editAppSettings())),
        Stretch()
    }).setMargin(10).setSpacing(10).useFor(this);
}

void ToolsStartPanel::showGaussCalculator()
{
    GaussCalculatorWindow::showCalcWindow();
}

void ToolsStartPanel::editStyleSheet()
{
    emit onEditStyleSheet();
}

void ToolsStartPanel::editAppSettings()
{
    Z::Dlg::editAppSettings(this);
}

//------------------------------------------------------------------------------
//                               StartWindow
//------------------------------------------------------------------------------

StartWindow::StartWindow(QWidget *parent) : QWidget(parent)
{
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
    connect(toolsPanel, &ToolsStartPanel::onEditStyleSheet, [this](){
        this->editStyleSheet();
    });

    auto tipsPanel = new TipsStartPanel(tipImage);

    connect(tipImage, &Ori::Widgets::Label::clicked, [tipsPanel]{
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

    // Should be after all widgets to overlay them
    tipImage->setParent(this);

    loadStyleSheet();

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

void StartWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    adjustTipImagePosition(_tipImage);
}

void StartWindow::loadStyleSheet()
{
    QFile file(":/style/StartWindow");
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok)
    {
        qWarning() << "Unable to load style from resources" << file.errorString();
        return;
    }
    QByteArray data = file.readAll();
    if (data.isEmpty())
    {
        qWarning() << "Unable to load style from resources: read data is empty";
        return;
    }
    QString styleSheet = data;
    if (styleSheet.isEmpty())
    {
        qWarning() << "Unable to load style from resources: data can't be converter to string";
        return;
    }
    setStyleSheet(styleSheet);
}
