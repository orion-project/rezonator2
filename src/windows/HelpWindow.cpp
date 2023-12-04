#include "HelpWindow.h"

#include "../app/CustomPrefs.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriDialogs.h"

#include <QDebug>
#include <QActionGroup>
#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QHelpContentWidget>
#include <QHelpEngine>
#include <QHelpIndexWidget>
#include <QHelpLink>
#include <QLineEdit>
#include <QTabBar>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>

using namespace Ori::Layouts;

namespace {

QHelpEngine* __engine = nullptr;
HelpWindow* __instance = nullptr;

bool isHttpUrl(const QUrl &url)
{
    return url.scheme().startsWith(QLatin1String("http"));
}

int initEngine()
{
    QString qhcDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (qhcDir.isEmpty())
        return 1;
    if (!QDir().mkpath(qhcDir))
        return 2;
    QString docPath = qApp->applicationDirPath() + "/rezonator.qch";
    QString nsName = QHelpEngineCore::namespaceName(docPath);
    if (nsName.isEmpty())
        return 3;
    if (nsName != "org.orion-project.rezonator")
        return 4;
    __engine = new QHelpEngine(qhcDir + "/rezonator.qhc", qApp);
    if (__engine->registeredDocumentations().contains(nsName))
        return 0;
    if (!__engine->registerDocumentation(docPath))
    {
        qWarning() << "Failed to register" << docPath << __engine->error();
        delete __engine;
        __engine = nullptr;
        return 5;
    }
    return 0;
}

} // namespace

//------------------------------------------------------------------------------
//                               HelpBrowser
//------------------------------------------------------------------------------

class HelpBrowser : public QTextBrowser
{
public:
    HelpBrowser() : QTextBrowser() {}

protected:
    void setSource(const QUrl &url)
    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        override
    #endif
    {
        if (isHttpUrl(url))
        {
            QDesktopServices::openUrl(url);
            return;
        }
        QTextBrowser::setSource(url);
    }

    QVariant loadResource(int type, const QUrl &url) override
    {
        if (url.scheme() == QLatin1String("qthelp"))
            return __engine->fileData(url);
        return QTextBrowser::loadResource(type, url);
    }
};

//------------------------------------------------------------------------------
//                               HelpWindow
//------------------------------------------------------------------------------

void HelpWindow::showContents(QWidget *parent)
{
    openWindow(parent);
    __instance->setSource("index.html");
    __instance->_actnContent->setChecked(true);
}

void HelpWindow::showIndex(QWidget *parent)
{
    openWindow(parent);
    __instance->_actnIndex->setChecked(true);
}

void HelpWindow::showTopic(const QString& topic, QWidget *parent)
{
    openWindow(parent);
    __instance->setSource(topic);
}

void HelpWindow::openWindow(QWidget *parent)
{
    if (!__engine)
        if (int err = initEngine(); err != 0)
            return Ori::Dlg::error(QString("Failed to initialize help system (code %1)").arg(err));
    if (!__instance)
        __instance = new HelpWindow(parent);
    __instance->show();
    __instance->activateWindow();
}

HelpWindow::HelpWindow(QWidget *parent) : QWidget{parent}
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon(":/window_icons/help"));
    setWindowTitle(tr("%1 Manual").arg(qApp->applicationName()));

    auto statusBar = new QStatusBar;

    _browser = new HelpBrowser;
    connect(_browser, QOverload<const QUrl&>::of(&QTextBrowser::highlighted), this, [statusBar](const QUrl& url){
        if (isHttpUrl(url))
            statusBar->showMessage(url.toString());
        else statusBar->clearMessage();
    });

    auto contentWidget = __engine->contentWidget();
    contentWidget->setAnimated(true);
    connect(contentWidget, &QHelpContentWidget::linkActivated, this, [this](const QUrl& url){
        _browser->setSource(url);
    });
    connect(contentWidget, &QHelpContentWidget::pressed, this, [this](const QModelIndex& index){
        _browser->setSource(__engine->contentModel()->contentItemAt(index)->url());
    });
    connect(_browser, &QTextBrowser::sourceChanged, this, [contentWidget](const QUrl& url){
        auto index = contentWidget->indexOf(url);
        if (!index.isValid()) return;
        contentWidget->scrollTo(index);
        contentWidget->setCurrentIndex(index);
    });

    auto indexWidget = __engine->indexWidget();
    connect(indexWidget, &QHelpIndexWidget::documentActivated, this, [this](const QHelpLink &document){
        _browser->setSource(document.url);
    });

    auto indexSearch = new QLineEdit;
    indexSearch->setClearButtonEnabled(true);
    indexSearch->setPlaceholderText(tr("Look for"));
    connect(indexSearch, &QLineEdit::textChanged, this, [indexWidget](const QString& text){
        indexWidget->filterIndices(text);
    });

    auto indexPanel = LayoutV({indexSearch, indexWidget}).setMargin(0).makeWidget();

    auto leftPanels = new QStackedWidget;
    leftPanels->addWidget(contentWidget);
    leftPanels->addWidget(indexPanel);

    auto actnGroup = new QActionGroup(this);
    _actnContent = new QAction(QIcon(":/toolbar/book"), tr("Content"), actnGroup);
    _actnIndex = new QAction(QIcon(":/toolbar/protocol"), tr("Index"), actnGroup);
    actnGroup->setExclusive(true);
    _actnContent->setCheckable(true);
    _actnContent->setChecked(true);
    _actnIndex->setCheckable(true);
    connect(_actnContent, &QAction::toggled, this, [leftPanels, contentWidget](bool on){
        if (!on) return;
        leftPanels->setCurrentIndex(0);
        contentWidget->setFocus();
    });
    connect(_actnIndex, &QAction::toggled, this, [leftPanels, indexSearch](bool on){
        if (!on) return;
        leftPanels->setCurrentIndex(1);
        indexSearch->setFocus();
    });

    _actnBack = new QAction(QIcon(":/toolbar/navigate_back"), tr("Back"), this);
    _actnBack->setShortcut(QKeySequence::MoveToPreviousPage);
    _actnForward = new QAction(QIcon(":/toolbar/navigate_fwd"), tr("Forward"), this);
    _actnForward->setShortcut(QKeySequence::MoveToNextPage);
    connect(_browser, &QTextBrowser::backwardAvailable, _actnBack, &QAction::setEnabled);
    connect(_browser, &QTextBrowser::forwardAvailable, _actnForward, &QAction::setEnabled);
    connect(_actnBack, &QAction::triggered, _browser, &QTextBrowser::backward);
    connect(_actnForward, &QAction::triggered, _browser, &QTextBrowser::forward);

    auto toolbar = new QToolBar;
    Ori::Gui::populate(toolbar, { Ori::Gui::textToolButton(_actnContent),
        Ori::Gui::textToolButton(_actnIndex), nullptr, _actnBack, _actnForward });

    _splitter = new QSplitter;
    _splitter->addWidget(LayoutV({toolbar, leftPanels}).setMargin(0).setSpacing(0).makeWidget());
    _splitter->addWidget(_browser);
    _splitter->setStretchFactor(0, 0);
    _splitter->setStretchFactor(1, 1);
    _splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto splitterLayout = LayoutV({_splitter}).boxLayout();
    splitterLayout->setContentsMargins(3, 3, 3, 0);

    LayoutV({splitterLayout, statusBar}).setSpacing(0).setMargin(0).useFor(this);

    auto obj = CustomPrefs::recentObj("help_window");
    CustomDataHelpers::restoreWindowSize(obj, this, 800, 600);
    _splitter->setSizes({obj["panel_w"].toInt(200), obj["browser_w"].toInt(600)});

    QTimer::singleShot(100, this, [contentWidget]{
        contentWidget->expandRecursively(QModelIndex(), 2);
    });
}

HelpWindow::~HelpWindow()
{
    auto sizes = _splitter->sizes();
    QJsonObject obj{{"panel_w", sizes.at(0)}, {"browser_w", sizes.at(1)}};
    CustomDataHelpers::storeWindowSize(obj, this);
    CustomPrefs::setRecentObj("help_window", obj);

    __instance = nullptr;
}

void HelpWindow::setSource(const QString& name)
{
    _browser->setSource(QUrl(QStringLiteral("qthelp://org.orion-project.rezonator/doc/") + name));
}
