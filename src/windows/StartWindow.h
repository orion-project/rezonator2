#ifndef START_WINDOW_H
#define START_WINDOW_H

#include <QWidget>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
class QLabel;
class QBoxLayout;
class QFileInfo;
class QMovie;
class QToolBar;
QT_END_NAMESPACE

namespace Ori {
    namespace Widgets {
        class Label;
    }
}

class CustomCssWidget : public QWidget
{
protected:
    // Paint event should overriden to apply styles sheets
    void paintEvent(QPaintEvent*) override;
};

//------------------------------------------------------------------------------

class MruStartItem : public CustomCssWidget
{
    Q_OBJECT

public:
    explicit MruStartItem(const QFileInfo& fileInfo);

signals:
    void onFileOpen(const QString& filePath);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

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

signals:
    void closeRequested();

private:
    void makeEmpty();
    void openFile(const QString& fileName);
};

//------------------------------------------------------------------------------

class TipsStartPanel : public CustomCssWidget
{
    Q_OBJECT

public:
    explicit TipsStartPanel(QLabel* tipImage);

    void closeTipImage();

private:
    QMovie* _movie = nullptr;
    QJsonObject _tips;
    QLabel *_tipText, *_tipPreview, *_tipImage;
    Ori::Widgets::Label *_enlargeTip;
    QString _imagePath;
    QStringList _ids;
    int _index = -1;

    const int TIP_IMG_PREVIEW_H = 150;
    const int TIP_IMG_PREVIEW_W = 300;

    void loadTips();
    void showNextTip();
    void showPrevTip();
    void showTip(const QJsonObject& tip);
    void chooseAndShowTip();
    void enlargePreview();
    void handleMovieStarted();
    void showTipImage(const QPixmap& pixmap);
};

//------------------------------------------------------------------------------

class ActionsStartPanel : public CustomCssWidget
{
    Q_OBJECT

public:
    explicit ActionsStartPanel();

signals:
    void closeRequested();

private slots:
    void openSchemaFile();
    void openSchemaExample();
    void makeSchemaSW();
    void makeSchemaRR();
    void makeSchemaSP();
};

//------------------------------------------------------------------------------

class ToolsStartPanel : public CustomCssWidget
{
    Q_OBJECT

public:
    explicit ToolsStartPanel();
};

//------------------------------------------------------------------------------

class StartWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);
    ~StartWindow() override;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QLabel *_tipImage;
    QToolBar *_toolbar;

    void editStyleSheet();
};

#endif // START_WINDOW_H
