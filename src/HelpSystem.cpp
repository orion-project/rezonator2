#include "HelpSystem.h"

#include "core/Format.h"

#include "helpers/OriDialogs.h"

#include <QApplication>
#include <QDesktopServices>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>

namespace {

Z::HelpSystem* __instance = nullptr;

}

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

}

void HelpSystem::showIndex()
{

}

void HelpSystem::visitHomePage()
{
    QDesktopServices::openUrl(QUrl(Z::Strs::homepage()));
}

void HelpSystem::checkUpdates()
{
    // TODO:NEXT-VER
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
            .replace("{app}", qApp->applicationName())
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


namespace Help {

void show(const QString& topic)
{
    Ori::Dlg::info(topic); // TODO: show help
}

} // namespace Help
} // namespace Z
