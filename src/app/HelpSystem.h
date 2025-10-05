#ifndef Z_HELP_SYSTEM_H
#define Z_HELP_SYSTEM_H

#include <QObject>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QAction;
class QNetworkAccessManager;
class QNetworkReply;
class QProcess;
QT_END_NAMESPACE

namespace Ori {
class UpdateChecker;
}

namespace Z {

class HelpSystem : public QObject
{
    Q_OBJECT

public:
    static HelpSystem* instance();

    static QAction* makeHelpAction(QObject *parent, const QString& topic);

    void setParent(QWidget* parent) { _parent = parent; }

    void showTopic(const QString& topic);
    static void topic(const QString& topic) { instance()->showTopic(topic); }

public slots:
    void showContents();
    void showIndex();
    void visitHomePage();
    void sendBugReport();
    void checkUpdates(bool silent = false);
    void checkUpdatesAuto();
    void showAbout();

private slots:
    void assistantFinished(int exitCode);
    void readStdout();
    void readStderr();

private:
    HelpSystem();

    QWidget* _parent = nullptr;
    QProcess* _assistant = nullptr;
    QPointer<Ori::UpdateChecker> _updateChecker;

    bool startAssistant();
    void closeAssistant();
    void versionReceived(QByteArray data);
    void historyReceived(QByteArray data);
    void finishUpdater();
};

} // namespace Z

#endif // Z_HELP_SYSTEM_H
