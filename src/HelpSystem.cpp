#include "HelpSystem.h"

#include "core/Format.h"
#include "core/Protocol.h"

#include "helpers/OriDialogs.h"
#include "core/OriVersion.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QPushButton>
#include <QStyle>
#include <QUrl>

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
    commands.append("setSource qthelp://org.orion-project.rezonator/doc/" % topic % '\n');
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
    connect(_updateReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [this](QNetworkReply::NetworkError){
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
    auto title = tr("About %1").arg(qApp->applicationName());
    auto text = tr(
                "<h2>{app} {app_ver}</h2>"
                "<p>Built: {build_date}"
                "<p>Copyright: Chunosov N.&nbsp;I. © 2006-{app_year}"
                "<p>Web: <a href='{www}'>{www}</a>"
                "<p>E-mail: <a href='mailto://{email}'>{email}</a>"
                "<p>Powered by "
                "<a href='http://qt.io'>Qt</a>, "
                "<a href='http://www.qcustomplot.com'>QCustomPlot</a>, "
                "<a href='http://lua.org'>Lua</a>"
                "<p>{app} is open-source laser resonator calculation tool, its "
                "source code is at <a href='{www_sources}'>{www_sources}</a>."
                "<p>The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING "
                "THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."
                )
            .replace("{app}", Z::Strs::appName())
            .replace("{app_ver}", Z::Strs::appVersion())
            .replace("{app_year}", Z::Strs::appVersionYear())
            .replace("{build_date}", Z::Strs::appVersionDate())
            .replace("{www}", Z::Strs::homepage())
            .replace("{email}", Z::Strs::email())
            .replace("{www_sources}", Z::Strs::sourcepage());
    QMessageBox about(QMessageBox::NoIcon, title, text, QMessageBox::Ok, _parent);
    about.setIconPixmap(QPixmap(":/window_icons/main").
        scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    auto button = about.addButton(tr("About Qt"), QMessageBox::ActionRole);
    connect(button, SIGNAL(clicked()), qApp, SLOT(aboutQt()));
    about.exec();
}

} // namespace Z
