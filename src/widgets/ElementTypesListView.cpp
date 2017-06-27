#include "ElementTypesListView.h"
#include "ElementImagesProvider.h"

#include <QKeyEvent>

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

    default:
        QListWidget::keyPressEvent(event);
    }
}

void ElementTypesListView::populate(Elements elems)
{
    blockSignals(true);
    clear();
    foreach (Element *elem, elems)
    {
        auto it = new QListWidgetItem(QIcon(iconPath(elem)), elem->typeName());
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
    setMinimumHeight(iconSize.height() + spacing() * 2 +
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

QString ElementTypesListView::iconPath(Element* elem) const
{
    return ElementImagesProvider::instance().iconPath(elem->type());
}

QSize ElementTypesListView::iconSize() const
{
    return ElementImagesProvider::instance().iconSize();
}
