#include "ElementTypesListView.h"
#include "ElementImagesProvider.h"

#include "../core/Utils.h"

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

void ElementTypesListView::populate(Elements elems, DisplayNameKind displayNameKind)
{
    static QMap<QString, QIcon> elemIcons;

    blockSignals(true);
    clear();
    for (Element *elem : elems)
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
        QString displayName;
        switch (displayNameKind)
        {
        case DisplayNameKind::Type: displayName = elem->typeName(); break;
        case DisplayNameKind::Title: displayName = elem->title(); break;
        }
        if (displayName.isEmpty())
            displayName = elem->typeName();
        auto it = new QListWidgetItem(elemIcons[elem->type()], displayName);
        it->setData(Qt::UserRole, ptr2var(elem));
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
        emit elementSelected(nullptr);
    else
        emit elementSelected(elem(item(index)));
}

Element* ElementTypesListView::selected() const
{
    if (currentRow() > -1)
        return elem(item(currentRow()));
    return nullptr;
}

Element *ElementTypesListView::elem(QListWidgetItem *item) const
{
    return var2ptr<Element*>(item->data(Qt::UserRole));
}

QSize ElementTypesListView::iconSize() const
{
    QSize size = ElementImagesProvider::instance().iconSize();
    size.rheight() += 4;
    size.rwidth() += 4;
    return size;
}
