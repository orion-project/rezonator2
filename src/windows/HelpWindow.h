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
    static void showContents(QWidget *parent = nullptr);
    static void showIndex(QWidget *parent = nullptr);
    static void showTopic(const QString& topic, QWidget *parent = nullptr);

private:
    static void openWindow(QWidget *parent);

    explicit HelpWindow(QWidget *parent);
    ~HelpWindow();

    QHelpEngine *_engine;
    QTextBrowser *_browser;
    QSplitter *_splitter;
    QAction *_actnContent, *_actnIndex, *_actnBack, *_actnForward;

    void setSource(const QString& name);
};

#endif // HELP_WINDOW_H
