#ifndef GRAPHICS_VIEW_H
#define GRAPHICS_VIEW_H

#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

namespace Z {

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphicsView(QWidget *parent = nullptr);

public slots:
    void copyImage();

signals:
    void zoomIn();
    void zoomOut();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QMenu *_menu = nullptr;
};

} // namespace Z

#endif // GRAPHICS_VIEW_H
