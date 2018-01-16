#ifndef PIXMAP_ITEM_DELEGATE_H
#define PIXMAP_ITEM_DELEGATE_H

#include <QItemDelegate>
#include <QPainter>

/**
    Simple delegate class to painting pixmaps in table cells.
*/
class PixmapDelegate : public QItemDelegate
{
public:
    PixmapDelegate(QSize iconSize, QWidget *parent = 0) : QItemDelegate(parent), _iconSize(iconSize)
    {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex& index) const
    {
        if (index.data().canConvert<QPixmap>())
        {
            if (option.state & QStyle::State_Selected)
                painter->fillRect(option.rect, option.palette.highlight());

            painter->drawPixmap(
                QRect(option.rect.left()+2, option.rect.top()+2, _iconSize.width(), _iconSize.height()),
                index.data().value<QPixmap>(), QRect(0, 0, _iconSize.width(), _iconSize.height()));
        }
        else
            QItemDelegate::paint(painter, option, index);
    }

private:
    QSize _iconSize;
};

#endif // PIXMAP_ITEM_DELEGATE_H
