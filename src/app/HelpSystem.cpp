#include "HelpSystem.h"

#include "../app/AppSettings.h"
#include "../core/Format.h"
#include "../windows/HelpWindow.h"

#include "core/OriVersion.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriDialogs.h"
#include "widgets/OriLabels.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDialog>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStyle>
#include <QUrl>

using namespace Ori::Layouts;

namespace {

Z::HelpSystem* __instance = nullptr;

} // namespace

namespace Z {

QAction* HelpSystem::makeHelpAction(QObject *parent, const QString& topic)
{
    auto a = new QAction(tr("Help"), parent);
    a->setIcon(QIcon(":/toolbar/help"));
    a->setShortcut(QKeySequence::HelpContents);
    a->connect(a, &QAction::triggered, [topic]{
        QString t(topic);
        if (!topic.endsWith(QStringLiteral(".html")))
            t += QStringLiteral(".html");
        instance()->showTopic(t);
    });
    return a;
}

HelpSystem::HelpSystem() : QObject()
{
}

HelpSystem* HelpSystem::instance()
{
    if (!__instance)
        __instance = new HelpSystem();
    return __instance;
}

void HelpSystem::showContents()
{
    if (AppSettings::instance().useOnlineHelp)
    {
        QDesktopServices::openUrl(QUrl(Z::Strs::homepage() + "/help/index.html"));
        return;
    }

    HelpWindow::showContents();
}

void HelpSystem::showIndex()
{
    if (AppSettings::instance().useOnlineHelp)
    {
        QDesktopServices::openUrl(QUrl(Z::Strs::homepage() + "/help/genindex.html"));
        return;
    }

    HelpWindow::showIndex();
}

void HelpSystem::showTopic(const QString& topic)
{
    if (AppSettings::instance().useOnlineHelp)
    {
        QDesktopServices::openUrl(QUrl(Z::Strs::homepage() + "/help/" + topic));
        return;
    }

    HelpWindow::showTopic(topic);
}

void HelpSystem::visitHomePage()
{
    QDesktopServices::openUrl(QUrl(Z::Strs::homepage()));
}

void HelpSystem::checkUpdates()
{
    if (_updateChecker)
    {
        qDebug() << "Check is already in progress";
        return;
    }
    _updateChecker = new QNetworkAccessManager(this);
    _updateReply = _updateChecker->get(QNetworkRequest(QUrl(Z::Strs::versionFileUrl())));
    connect(_updateReply, &QNetworkReply::finished, this, [this](){
        if (!_updateReply) return;
        auto versionData = _updateReply->readAll();
        _updateReply->deleteLater();
        _updateReply = nullptr;
        _updateChecker->deleteLater();
        _updateChecker = nullptr;
        versionReceived(versionData);
    });
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    connect(_updateReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
#else
    connect(_updateReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
#endif
            this, [this](QNetworkReply::NetworkError){
        auto errorMsg =_updateReply->errorString();
        qCritical() << "Network error" << errorMsg;
        _updateReply->deleteLater();
        _updateReply = nullptr;
        _updateChecker->deleteLater();
        _updateChecker = nullptr;
        Ori::Dlg::error(tr("Failed to get version information"));
    });
}

void HelpSystem::versionReceived(QByteArray versionData) const
{
    auto versionStr = QString::fromLatin1(versionData);
    Ori::Version serverVersion(versionStr);
    Ori::Version currentVersion(APP_VER_MAJOR, APP_VER_MINOR, APP_VER_PATCH);
    if (currentVersion >= serverVersion)
        Ori::Dlg::info(tr("<p>You are using version %1"
                          "<p>Version on the server is %2"
                          "<p>You are using the most recent version of %3")
                       .arg(Z::Strs::appVersion(), versionStr, Z::Strs::appName()));
    else Ori::Dlg::info(tr("<p>You are using version %1"
                           "<p>Version on the server is <b>%2</b>"
                           "<p>There is a newer version of %3"
                           "<p><a href='%4'>Open download page</a>")
                        .arg(Z::Strs::appVersion(), versionStr, Z::Strs::appName(), Z::Strs::downloadPage()));
}

void HelpSystem::sendBugReport()
{
    QDesktopServices::openUrl(QUrl(Z::Strs::newIssueUrl()));
}

void HelpSystem::showAbout()
{
    auto w = new QDialog;
    w->setObjectName("about_dlg");
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setWindowTitle(tr("About %1").arg(qApp->applicationName()));

    auto makeLabel = [](const QString& name, const QString& text){
        auto label = new Ori::Widgets::Label(text);
        label->setObjectName(name);
        label->setProperty("role", "about_dlg");
        return label;
    };

    auto labelVersion = makeLabel("version_label", QString("%1.%2.%3").arg(APP_VER_MAJOR).arg(APP_VER_MINOR).arg(APP_VER_PATCH));
    auto labelCodename = makeLabel("codename_label", APP_VER_CODENAME);
    auto labelDate = makeLabel("date_label", BUILDDATE);

    auto labelQt = makeLabel({}, QString("Powered by Qt %1").arg(QT_VERSION_STR));
    connect(labelQt, &Ori::Widgets::Label::clicked, []{ qApp->aboutQt(); });
    labelQt->setCursor(Qt::PointingHandCursor);

    auto labelInfo = makeLabel({}, QString("Chunosov N.I. © 2006-%1").arg(APP_VER_YEAR));

    auto makeLink = [makeLabel](const QString& address, const QString& href = QString()) {
        auto label = makeLabel({}, address);
        connect(label, &Ori::Widgets::Label::clicked, [address, href]{
            QDesktopServices::openUrl(QUrl(href.isEmpty() ? address : href));
        });
        label->setCursor(Qt::PointingHandCursor);
        return label;
    };

    LayoutV({
        LayoutH({Stretch(), labelVersion, Space(4)}),
        LayoutH({Stretch(), labelCodename, Space(4)}),
        Space(4),
        LayoutH({Stretch(), labelDate, Space(4)}),
        Stretch(),
        LayoutH({Space(4), labelQt, Stretch()}),
        Space(4),
        LayoutH({Space(4), labelInfo, Stretch()}),
        Space(4),
        LayoutH({Space(4), makeLink(Z::Strs::email(), QString("mailto:%1").arg(Z::Strs::email())), Stretch()}),
        Space(4),
        LayoutH({Space(4), makeLink(Z::Strs::homepage()), Stretch()}),
        Space(4),
        LayoutH({Space(4), makeLink(Z::Strs::sourcepage()), Stretch()}),
        Space(4),
    }).setMargin(12).setSpacing(0).useFor(w);

    w->exec();
}

} // namespace Z
