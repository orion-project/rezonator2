#ifndef CURSOR_PANEL_H
#define CURSOR_PANEL_H

#include <QTextBrowser>

class PlotFunction;
class QCPCursor;

class CursorPanel : public QTextBrowser
{
    Q_OBJECT

public:
    enum Mode { Both, Vertical, Horizontal };
    Q_ENUM(Mode)

public:
    explicit CursorPanel(PlotFunction *func, QCPCursor *cursor);

    void update();
    void update(const QString& info);
    void placeIn(class QToolBar* toolbar);
    void fillMenu(QMenu *menu);

    bool autoUpdateInfo() const { return _autoUpdateInfo; }
    void setAutoUpdateInfo(bool v) { _autoUpdateInfo = v; }

    Mode mode() const;
    void setMode(Mode mode);
    bool enabled() const;
    void setEnabled(bool on);

protected:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

private:
    QCPCursor *_cursor;
    PlotFunction *_function;
    QAction *actnCursorFollow, *actnCursorSetX, *actnCursorSetY,
        *actnShowCursor, *actnCursorVert, *actnCursorHorz, *actnCursorBoth;
    bool _autoUpdateInfo = true;

    void createActions();
    QString formatCursorInfo();

private slots:
    void linkClicked(const class QUrl&);
    void cursorPositionCanged();
    void setCursorX();
    void setCursorY();
    void setCursorShape();
};

#endif // CURSOR_PANEL_H
