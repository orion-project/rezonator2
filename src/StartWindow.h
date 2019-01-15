#ifndef START_WINDOW_H
#define START_WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QBoxLayout;
QT_END_NAMESPACE

class StartWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);

private:
    QWidget* makeActionsPanel();
    QWidget* makeMruPanel();
    QWidget* makeTipsPanel();
    QWidget* makeToolsPanel();
    QWidget* makePanel(QBoxLayout* layout);
    QWidget* makeHeader(const QString& title);
    QWidget* makeButton(const QString& iconPath, const QString& title, const char* slot);

    void makeNewSchema();

private slots:
    void toolGaussCalc();
    void editStyleSheet();
};

#endif // START_WINDOW_H
