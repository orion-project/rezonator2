#include "HelpSystem.h"

#include "../app/Appearance.h"
#include "../app/AppSettings.h"
#include "../core/Format.h"
#include "../core/Protocol.h"
#include "../windows/HelpWindow.h"

#include "core/OriVersion.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriDialogs.h"
#include "tools/OriUpdater.h"
#include "widgets/OriLabels.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDialog>
#include <QFile>
#include <QFormLayout>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpression>
#include <QStyle>
#include <QTextBrowser>
#include <QTimer>
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
        QDesktopServices::openUrl(QUrl(Z::Strs::homePageUrl() + "/help/index.html"));
        return;
    }
    if (startAssistant())
    {
        QByteArray commands;
        commands.append("show contents;");
        commands.append("expandToc 3;");
        commands.append("setSource qthelp://org.orion-project.rezonator/doc/index.html\n");
        _assistant->write(commands);
        return;
    }
    HelpWindow::showContents();
}

void HelpSystem::showIndex()
{
    if (AppSettings::instance().useOnlineHelp)
    {
        QDesktopServices::openUrl(QUrl(Z::Strs::homePageUrl() + "/help/genindex.html"));
        return;
    }
    if (startAssistant())
    {
        QByteArray commands;
        commands.append("show index\n");
        _assistant->write(commands);
        return;
    }
    HelpWindow::showIndex();
}

void HelpSystem::showTopic(const QString& helpTopic)
{
    QString topic = helpTopic;
    if (!topic.endsWith(".html", Qt::CaseInsensitive))
        topic += ".html";

    if (AppSettings::instance().useOnlineHelp)
    {
        QDesktopServices::openUrl(QUrl(Z::Strs::homePageUrl() + "/help/" + topic));
        return;
    }
    if (startAssistant())
    {
        QByteArray commands;
        commands.append("setSource qthelp://org.orion-project.rezonator/doc/");
        commands.append(topic.toLocal8Bit());
        commands.append('\n');
        _assistant->write(commands);
        return;
    }
    HelpWindow::showTopic(topic);
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
    Z_INFO("Help file:" << helpFile);

#ifdef Q_OS_WIN
    QString assistantExe = appDir + "/assistant.exe";
#else
    QString assistantExe = appDir + "/assistant";
#endif
    QFile assistantLink(appDir + "/assistant.txt");
    if (assistantLink.exists() && assistantLink.open(QIODevice::ReadOnly))
        assistantExe = QString::fromLocal8Bit(assistantLink.readAll()).trimmed();
    Z_INFO("Help viewer:" << assistantExe);

    if (!QFile::exists(assistantExe))
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
    process->start(assistantExe, {
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
    QDesktopServices::openUrl(QUrl(Z::Strs::homePageUrl()));
}

class UpdateStatusStore : public Ori::IUpdateStatusStore
{
    void saveChecked() override { AppSettings::instance().saveUpdateChecked(); }
};

void HelpSystem::checkUpdates(bool silent)
{
    if (!_updateChecker)
    {
        _updateChecker = new Ori::UpdateChecker({
            .currentVersion = Ori::Version(APP_VER_MAJOR, APP_VER_MINOR, APP_VER_PATCH),
            .versionFileUrl = "http://rezonator.orion-project.org/files/version.xml",
            .historyFileUrl = "http://rezonator.orion-project.org/history.json",
            .githubUsername = "orion-project",
            .githubProject = "rezonator2",
            .statusStore = std::shared_ptr<Ori::IUpdateStatusStore>(new UpdateStatusStore),
        });
    }
    _updateChecker->check(silent);
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

    auto labelVersion = makeLabel("version_label", Z::Strs::appVersion());
    auto labelDate = makeLabel("date_label", BUILDDATE);

    auto labelQt = makeLabel({}, QString("Powered by Qt %1").arg(QT_VERSION_STR));
    connect(labelQt, &Ori::Widgets::Label::clicked, []{ qApp->aboutQt(); });
    labelQt->setCursor(Qt::PointingHandCursor);

    auto labelInfo = makeLabel({}, QString("Chunosov N.I. © %1-%2").arg(APP_VER_YEAR_0).arg(APP_VER_YEAR));

    auto makeLink = [makeLabel](const QString& address, const QString& href = QString()) {
        auto label = makeLabel({}, address);
        connect(label, &Ori::Widgets::Label::clicked, [address, href]{
            QDesktopServices::openUrl(QUrl(href.isEmpty() ? address : href));
        });
        label->setCursor(Qt::PointingHandCursor);
        return label;
    };

    LayoutV({
        LayoutH({Stretch(), labelVersion}),
        LayoutH({Stretch(), labelDate}),
        Stretch(),
        labelQt,
        labelInfo,
        makeLink(Z::Strs::email(), QString("mailto:%1").arg(Z::Strs::email())),
        makeLink(Z::Strs::homePageUrl()),
        makeLink(Z::Strs::sourcesUrl()),
    }).setMargin(12).setSpacing(0).useFor(w);

    Z::Gui::applyStylesheet(w);
    w->exec();
}

void HelpSystem::checkUpdatesAuto()
{
    if (_updateChecker)
        return;
    auto interval = AppSettings::instance().updateCheckInterval;
    if (interval == UpdateCheckInterval::None)
        return;
    bool elapsed = true;
    auto prev = AppSettings::instance().updateLastCheckDate;
    if (prev.isValid()) {
        auto now = QDate::currentDate();
        switch (interval) {
        case UpdateCheckInterval::None:
            break;
        case UpdateCheckInterval::Daily:
            elapsed = now.dayOfYear() != prev.dayOfYear() || now.year() != prev.year();
            break;
        case UpdateCheckInterval::Weekly:
            elapsed = now.weekNumber() != prev.weekNumber() || now.year() != prev.year();
            break;
        case UpdateCheckInterval::Monthly:
            elapsed = now.month() != prev.month() || now.year() != prev.year();
            break;
        }
    }
    if (elapsed)
        QTimer::singleShot(AppSettings::instance().updateCheckDelayMs, this, [this]{ checkUpdates(true); });
}

} // namespace Z
