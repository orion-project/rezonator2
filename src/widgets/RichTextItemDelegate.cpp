#include "RichTextItemDelegate.h"

#include <QDebug>
#include <QPainter>
#include <QTextDocument>

RichTextItemDelegate::RichTextItemDelegate(int textOffsetY, QObject *parent) : QItemDelegate(parent), _textOffsetY(textOffsetY)
{
}

RichTextItemDelegate::RichTextItemDelegate(QObject *parent) : QItemDelegate(parent)
{
}

void RichTextItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    _paintingIndex = index;
    QItemDelegate::paint(painter, option, index);
}

void RichTextItemDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const
{
    Q_UNUSED(option);
    Q_UNUSED(text);
    Q_ASSERT(_paintingIndex.isValid());

    QTextDocument *doc = document(option, _paintingIndex, false);

    painter->save();
    QRectF textClipRect;
    qreal textOffsetX;
    if (Qt::AlignHCenter & option.displayAlignment)
    {
        QSizeF docSize = doc->size();
        textOffsetX = (rect.width() - docSize.width())/2;
        textClipRect.setRect(0, 0, docSize.width(), rect.height());
    }
    else // we don't use another alignments, extend when needed
    {
        textOffsetX = 0;
        textClipRect.setRect(0, 0, rect.width(), rect.height());
    }
    painter->translate(rect.left() + textOffsetX,  rect.top() + _textOffsetY);
    doc->drawContents(painter, textClipRect);
    painter->restore();
    delete doc;
}

QSize RichTextItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::SizeHintRole);
    if (value.isValid())
        return qvariant_cast<QSize>(value);

    QTextDocument *doc = document(option, index, true);

    QRect decorationRect = rect(option, index, Qt::DecorationRole);
    QRect displayRect = QRect(0, 0, int(doc->size().width()), int(doc->size().height()));
    QRect checkRect = rect(option, index, Qt::CheckStateRole);

    doLayout(option, &checkRect, &decorationRect, &displayRect, true);

    delete doc;

    return (decorationRect|displayRect|checkRect).size();
}

QTextDocument* RichTextItemDelegate::document(const QStyleOptionViewItem &option, const QModelIndex& index, bool isMeasuring) const
{
    QFont font = qvariant_cast<QFont>(index.data(Qt::FontRole)).resolve(option.font);

    QTextDocument *doc = new QTextDocument;
    doc->setDefaultFont(font);
    doc->setTextWidth(-1);
    QString text = index.data(Qt::DisplayRole).toString();
    if (!isMeasuring && (QStyle::State_Selected & option.state))
    {
        // change any explicitly specified color to highlighted text color
        auto group = QStyle::State_Active & option.state ? QPalette::Active : QPalette::Inactive;
        auto color = option.palette.color(group, QPalette::HighlightedText);
        auto colorStyle = QStringLiteral("color:%1").arg(color.name(QColor::HexRgb));
        static QRegExp colorEntry("color: *#[a-fA-F\\d]+"); // only #rrggbb color format is replaced
        text.replace(colorEntry, colorStyle);
        text = QStringLiteral("<span style='%1'>%2</span>").arg(colorStyle, text);
    }
    doc->setHtml(text);
    return doc;
}
