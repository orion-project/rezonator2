#ifndef START_WINDOW_H
#define START_WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QBoxLayout;
class QJsonObject;
class QFileInfo;
QT_END_NAMESPACE

class CustomCssWidget : public QWidget
{
protected:
    // Paint event should overriden to apply styles heets
    void paintEvent(QPaintEvent*) override;
};

//------------------------------------------------------------------------------

class MruItemWidget : public CustomCssWidget
{
    Q_OBJECT

public:
    explicit MruItemWidget(const QFileInfo& fileInfo);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QString _filePath, _displayFilePath;
    QLabel* _filePathLabel;
};

//------------------------------------------------------------------------------

class MruStartPanel : public CustomCssWidget
{
    Q_OBJECT

public:
    explicit MruStartPanel();

private:
    void makeEmpty();
};

//------------------------------------------------------------------------------

class StartWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);

private:
    QWidget* actionsPanel();
    QWidget* tipsPanel();
    QWidget* toolsPanel();
    QWidget* panel(const QString& name, QBoxLayout* layout);
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
