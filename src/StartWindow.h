#ifndef START_WINDOW_H
#define START_WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QBoxLayout;
class QJsonObject;
class QFileInfo;
QT_END_NAMESPACE

class MruItemWidget : public QWidget
{
   Q_OBJECT

public:
    explicit MruItemWidget(const QFileInfo& fileInfo);

protected:
    void paintEvent(QPaintEvent*);
};

class StartWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);

private:
    QWidget* actionsPanel();
    QWidget* mruPanel();
    QWidget* mruPanelEmpty();
    QWidget* tipsPanel();
    QWidget* toolsPanel();
    QWidget* panel(QBoxLayout* layout);
    QWidget* header(const QString& title);
    QWidget* button(const QString& iconPath, const QString& title, const char* slot);

private slots:
    void makeSchemaSW();
    void makeSchemaRR();
    void makeSchemaSP();
    void showGaussCalculator();
    void editStyleSheet();
    void showNextTip();
    void showPrevTip();

private:
    QLabel *_tipText, *_tipPreview;

    void showTip(const QJsonObject& tip);
};

#endif // START_WINDOW_H
