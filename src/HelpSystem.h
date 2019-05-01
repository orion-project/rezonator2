#ifndef Z_HELP_SYSTEM_H
#define Z_HELP_SYSTEM_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
class QProcess;
QT_END_NAMESPACE

namespace Z {

class HelpSystem : public QObject
{
    Q_OBJECT

public:
    static HelpSystem* instance();

    void setParent(QWidget* parent) { _parent = parent; }

    void showTopic(const QString& topic);

public slots:
    void showContents();
    void showIndex();
    void visitHomePage();
    void sendBugReport();
    void checkUpdates();
    void showAbout();

private slots:
    void assistantFinished(int exitCode);
    void readStdout();
    void readStderr();

private:
    HelpSystem();

    QWidget* _parent = nullptr;
    QProcess* _assistant = nullptr;
    QNetworkAccessManager* _updateChecker = nullptr;
    QNetworkReply* _updateReply = nullptr;

    bool startAssistant();
    void closeAssistant();
    void versionReceived(QByteArray versionData) const;
};

} // namespace Z

#endif // Z_HELP_SYSTEM_H
