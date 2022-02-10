#include "HelpSystem.h"

#include "core/Format.h"
#include "core/Protocol.h"

#include "core/OriVersion.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriDialogs.h"
#include "widgets/OriLabels.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QPushButton>
#include <QStyle>
#include <QUrl>

using namespace Ori::Layouts;

namespace {

Z::HelpSystem* __instance = nullptr;

} // namespace

namespace Z {

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
    if (!startAssistant()) return;

    QByteArray commands;
    commands.append("show contents;");
    commands.append("expandToc 3;");
    commands.append("setSource qthelp://org.orion-project.rezonator/doc/index.html\n");
    _assistant->write(commands);
}

void HelpSystem::showIndex()
{
    if (!startAssistant()) return;

    QByteArray commands;
    commands.append("show index\n");
    _assistant->write(commands);
}

void HelpSystem::showTopic(const QString& topic)
{
    if (!startAssistant()) return;

    QByteArray commands;
    commands.append("setSource qthelp://org.orion-project.rezonator/doc/");
    commands.append(topic.toLocal8Bit());
    commands.append('\n');
    _assistant->write(commands);
}

bool HelpSystem::startAssistant()
{
    if (_assistant)
    {
        if (_assistant->state() == QProcess::Running) return true;

        delete _assistant;
        _assistant = nullptr;
    }

    QString appDir = qApp->applicationDirPath();
    QString helpFile = appDir + "/rezonator.qhc";
#ifdef Q_OS_WIN
    QString assistantFile = appDir + "/assistant.exe";
#else
    QString assistantFile = appDir + "/assistant";
#endif
    Z_INFO("Help file:" << helpFile);
    Z_INFO("Help viewer:" << assistantFile);

    if (!QFile::exists(assistantFile))
    {
        Z_ERROR("Help viewer not found");
        Ori::Dlg::error("Help viewer not found");
        return false;
    }

    if (!QFile::exists(helpFile))
    {
        Z_ERROR("Help file not found");
        Ori::Dlg::error("Help file not found");
        return false;
    }

    QProcess *process = new QProcess(this);
    process->start(assistantFile, {
                       "-collectionFile", helpFile,
                       "-style", qApp->style()->objectName(),
                       "-enableRemoteControl"
                   });
    if (!process->waitForStarted(5000))
    {
        Z_ERROR("Failed to start help viewer" << process->errorString());
        Ori::Dlg::error("Failed to start help viewer");
        delete process;
        return false;
    }
    Z_INFO("Help viewer PID:" << process->processId());
    connect(process, SIGNAL(finished(int)), this, SLOT(assistantFinished(int)));
    connect(process, &QProcess::readyReadStandardOutput, this, &HelpSystem::readStdout);
    connect(process, &QProcess::readyReadStandardError, this, &HelpSystem::readStderr);
    connect(qApp, &QApplication::aboutToQuit, this, &HelpSystem::closeAssistant);
    _assistant = process;
    return true;
}

void HelpSystem::readStdout()
{
    Z_INFO("Help viewer stdout:" << QString::fromLocal8Bit(_assistant->readAllStandardOutput()));
}

void HelpSystem::readStderr()
{
    Z_INFO("Help viewer stderr:" << QString::fromLocal8Bit(_assistant->readAllStandardError()));
}

void HelpSystem::assistantFinished(int exitCode)
{
    Z_INFO("Help viewer finished, exit code" << exitCode);
    if (_assistant)
    {
        _assistant->deleteLater();
        _assistant = nullptr;
    }
}

void HelpSystem::closeAssistant()
{
    if (!_assistant) return;
    if (_assistant->state() == QProcess::Running)
    {
        _assistant->terminate();
        _assistant->waitForFinished(5000);
    }
    delete _assistant;
    _assistant = nullptr;
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
    connect(_updateReply, &QNetworkReply::finished, [this](){
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
            [this](QNetworkReply::NetworkError){
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
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setWindowTitle(tr("About %1").arg(qApp->applicationName()));

    QPixmap bckgnd(":/style/about");
    w->setMaximumSize(bckgnd.size());
    w->setMinimumSize(bckgnd.size());
    w->resize(bckgnd.size());

    auto p = w->palette();
    p.setBrush(QPalette::Window, QBrush(bckgnd));
    w->setPalette(p);

    auto f = w->font();
#ifdef Q_OS_WIN
    f.setFamily("Consolas");
#endif
#ifdef Q_OS_MAC
    f.setFamily("Monaco"); // Menlo?
#endif
#ifdef Q_OS_LINUX
    f.setFamily("monospace");
#endif

    auto labelVersion = new QLabel(QString("%1.%2.%3").arg(APP_VER_MAJOR).arg(APP_VER_MINOR).arg(APP_VER_PATCH));
    f.setPixelSize(50);
    f.setBold(true);
    labelVersion->setFont(f);
    labelVersion->setStyleSheet("color:white");

    f.setPixelSize(34);
    auto labelCodename = new QLabel(APP_VER_CODENAME);
    labelCodename->setFont(f);
    labelCodename->setStyleSheet("color:white");

    f.setBold(false);
    f.setPixelSize(20);
    auto labelDate = new QLabel(BUILDDATE);
    labelDate->setFont(f);
    labelDate->setStyleSheet("color:white");

    auto labelQt = new Ori::Widgets::Label(QString("Powered by Qt %1").arg(QT_VERSION_STR));
    connect(labelQt, &Ori::Widgets::Label::clicked, []{ qApp->aboutQt(); });
    labelQt->setCursor(Qt::PointingHandCursor);
    labelQt->setStyleSheet("color:white");
    labelQt->setFont(f);

    auto makeInfo = [f](const QString& text){
        auto label = new QLabel(text);
        label->setStyleSheet("color:white");
        label->setFont(f);
        return label;
    };

    auto makeLink = [f](const QString& address, const QString& href = QString()) {
        auto label = new Ori::Widgets::Label(address);
        connect(label, &Ori::Widgets::Label::clicked, [address, href]{
            QDesktopServices::openUrl(QUrl(href.isEmpty() ? address : href));
        });
        label->setCursor(Qt::PointingHandCursor);
        label->setStyleSheet("color:white");
        label->setFont(f);
        return label;
    };

    f.setPixelSize(9);
    auto labelDescr = new QLabel(
        "reZonator is open-source laser resonator calculation tool. "
        "The program is provided as is with no warranty of any kind, "
        "including the warranty of design, merchantability and fitness for a particular purpose.");
    labelDescr->setWordWrap(true);
    labelDescr->setStyleSheet("color:#88FFFFFF");
    labelDescr->setFont(f);

    LayoutV({
        LayoutH({Stretch(), labelVersion, Space(4)}),
        LayoutH({Stretch(), labelCodename, Space(4)}),
        Space(4),
        LayoutH({Stretch(), labelDate, Space(4)}),
        Stretch(),
        LayoutH({Space(4), labelQt, Stretch()}),
        Space(4),
        LayoutH({Space(4), makeInfo(QString("Chunosov N.I. © 2006-%1").arg(APP_VER_YEAR)), Stretch()}),
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
