#ifndef HELP_WINDOW_H
#define HELP_WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QHelpEngine;
class QSplitter;
class QTextBrowser;
QT_END_NAMESPACE

class HelpWindow : public QWidget
{
    Q_OBJECT

public:
    static void showContents();
    static void showIndex();
    static void showTopic(const QString& topic);

private:
    static void openWindow();

    explicit HelpWindow(QHelpEngine *engine);
    ~HelpWindow();

    QTextBrowser *_browser;
    QSplitter *_splitter;
    QAction *_actnContent, *_actnIndex, *_actnBack, *_actnForward;

    void setSource(const QString& name);

private:
    QHelpEngine* _engine;
};

#endif // HELP_WINDOW_H
