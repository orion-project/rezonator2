#include "ElementTypesListView.h"
#include "ElementImagesProvider.h"

#include <QKeyEvent>
#include <QPainter>

ElementTypesListView::ElementTypesListView(QWidget *parent) : QListWidget(parent)
{
    setAlternatingRowColors(true);
    setIconSize(iconSize());
    connect(this, SIGNAL(currentRowChanged(int)), this, SLOT(rowSelected(int)));
}

void ElementTypesListView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit enterPressed();
        QListWidget::keyPressEvent(event);
        break;

    default:
        QListWidget::keyPressEvent(event);
    }
}

void ElementTypesListView::populate(Elements elems)
{
    static QMap<QString, QIcon> elemIcons;

    blockSignals(true);
    clear();
    foreach (Element *elem, elems)
    {
        if (!elemIcons.contains(elem->type()))
        {
            // Make icons with margins for better look here
            QIcon defaultIcon(ElementImagesProvider::instance().iconPath(elem->type()));
            QSize defaultSize = ElementImagesProvider::instance().iconSize();
            QSize thisSize = iconSize();
            QPixmap thisPixmap(thisSize);
            thisPixmap.fill(Qt::transparent);
            int marginX = (thisSize.width() - defaultSize.width()) / 2;
            int marginY = (thisSize.height() - defaultSize.height()) / 2;
            QPainter painter(&thisPixmap);
            painter.drawPixmap(marginX, marginY, defaultSize.width(), defaultSize.height(), defaultIcon.pixmap(defaultSize));
            elemIcons.insert(elem->type(), QIcon(thisPixmap));
        }
        auto it = new QListWidgetItem(elemIcons[elem->type()], elem->typeName());
        it->setData(Qt::UserRole, elem->type());
        addItem(it);
    }
    adjust();
    blockSignals(false);
    setCurrentRow(0);
}

void ElementTypesListView::adjust()
{
    if (count() == 0) return;

    QFontMetrics fm(font());
    int width, max_width = 0;
    for (int i = 0; i < count(); i++)
    {
        width = fm.width(item(i)->text());
        if (width > max_width) max_width = width;
    }
    auto iconSize = ElementImagesProvider::instance().iconSize();
    max_width += iconSize.width() * 2 +
                 style()->pixelMetric(QStyle::PM_ScrollBarExtent) +
                 style()->pixelMetric(QStyle::PM_DefaultFrameWidth) * 2;
    setMinimumWidth(max_width);
    setMinimumHeight(iconSize.height() + spacing() * 2 + 4 +
                     style()->pixelMetric(QStyle::PM_DefaultFrameWidth) * 2);
}

void ElementTypesListView::rowSelected(int index)
{
    if (index < 0)
        emit elementSelected(QString());
    else
        emit elementSelected(elemType(item(index)));
}

const QString ElementTypesListView::selected() const
{
    if (currentRow() > -1)
        return elemType(item(currentRow()));
    return QString();
}

QString ElementTypesListView::elemType(QListWidgetItem *item) const
{
    return item->data(Qt::UserRole).toString();
}

QSize ElementTypesListView::iconSize() const
{
    QSize size = ElementImagesProvider::instance().iconSize();
    size.rheight() += 4;
    size.rwidth() += 4;
    return size;
}
