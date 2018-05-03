#ifndef RICH_TEXT_ITEM_DELEGATE_H
#define RICH_TEXT_ITEM_DELEGATE_H

#include <QItemDelegate>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class RichTextItemDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit RichTextItemDelegate(int textOffsetY, QObject *parent = nullptr);
    explicit RichTextItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const override;

private:
    QTextDocument* document(const QStyleOptionViewItem &option, const QModelIndex& index, bool isMeasuring) const;

    mutable QModelIndex _paintingIndex;
    int _textOffsetY = 0;
};

#endif // RICH_TEXT_ITEM_DELEGATE_H
