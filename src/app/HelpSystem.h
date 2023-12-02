#ifndef Z_HELP_SYSTEM_H
#define Z_HELP_SYSTEM_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QAction;
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

    static QAction* makeHelpAction(QObject *parent, const QString& topic);

    void setParent(QWidget* parent) { _parent = parent; }

    void showTopic(const QString& topic);

public slots:
    void showContents();
    void showIndex();
    void visitHomePage();
    void sendBugReport();
    void checkUpdates();
    void showAbout();

private:
    HelpSystem();

    QWidget* _parent = nullptr;
    QNetworkAccessManager* _updateChecker = nullptr;
    QNetworkReply* _updateReply = nullptr;

    void versionReceived(QByteArray versionData) const;
};

} // namespace Z

#endif // Z_HELP_SYSTEM_H
