#ifndef PIXMAP_ITEM_DELEGATE_H
#define PIXMAP_ITEM_DELEGATE_H

#include <QItemDelegate>
#include <QPainter>

/**
    Simple delegate class to painting pixmaps in table cells.
*/
// TODO: remove this class and switch to usage PixmapItemDelegate
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


/**
    Simple delegate class to painting pixmaps in table cells.
*/
class PixmapItemDelegate : public QItemDelegate
{
public:
    PixmapItemDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex& index) const
    {
        auto indexData = index.data(Qt::DecorationRole);
        if (indexData.canConvert<QPixmap>())
        {
            if (option.state & QStyle::State_Selected)
                painter->fillRect(option.rect, option.palette.highlight());

            auto pixmap = indexData.value<QPixmap>();
            painter->drawPixmap(
                QRect(option.rect.left()+2, option.rect.top()+2, pixmap.width(), pixmap.height()),
                pixmap, QRect(0, 0, pixmap.width(), pixmap.height()));
        }
        else
            QItemDelegate::paint(painter, option, index);
    }
};

#endif // PIXMAP_ITEM_DELEGATE_H
