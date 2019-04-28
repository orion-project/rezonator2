#ifndef Z_HELP_SYSTEM_H
#define Z_HELP_SYSTEM_H

#include <QObject>

QT_BEGIN_NAMESPACE
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

    bool startAssistant();
    void closeAssistant();
};

} // namespace Z

#endif // Z_HELP_SYSTEM_H
