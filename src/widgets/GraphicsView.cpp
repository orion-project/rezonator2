#include "GraphicsView.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QMenu>
#include <QWheelEvent>

namespace Z {

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing, true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

void GraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    if (!_menu)
    {
        _menu = new QMenu(this);
        _menu->addAction(QIcon(":/toolbar/copy_img"), tr("Copy Image"), this, &GraphicsView::copyImage);
    }
    _menu->popup(this->mapToGlobal(event->pos()));
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    if (!event->modifiers().testFlag(Qt::ControlModifier))
        return QGraphicsView::wheelEvent(event);

    if (event->angleDelta().y() > 0)
        emit zoomIn();
    else
        emit zoomOut();
}

void GraphicsView::copyImage()
{
    QImage image(scene()->sceneRect().size().toSize(), QImage::Format_RGB888);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    scene()->render(&painter);

    qApp->clipboard()->setImage(image);
}

} // namespace Z
