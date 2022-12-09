#include "ElementTypesListView.h"

#include "../core/Utils.h"

#include <QKeyEvent>

ElementTypesListView::ElementTypesListView(QWidget *parent) : QListWidget(parent)
{
    setAlternatingRowColors(true);
    setIconSize(Z::Utils::elemIconSize());
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
    auto sz = iconSize();
    sz.setHeight(sz.height() + 8);

    blockSignals(true);
    clear();
    for (Element *elem : elems)
    {
        QString displayName;
        switch (displayNameKind)
        {
        case DisplayNameKind::Type: displayName = elem->typeName(); break;
        case DisplayNameKind::Title: displayName = elem->title(); break;
        }
        if (displayName.isEmpty())
            displayName = elem->typeName();
        auto it = new QListWidgetItem(QIcon(Z::Utils::elemIconPath(elem->type())), displayName);
        it->setData(Qt::UserRole, ptr2var(elem));
        it->setSizeHint(sz);
        addItem(it);
    }
    blockSignals(false);
    setCurrentRow(0);
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
