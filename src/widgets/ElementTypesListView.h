#ifndef ELEMENT_TYPES_LISTVIEW_H
#define ELEMENT_TYPES_LISTVIEW_H

#include <QListWidget>

#include "../core/Element.h"

class ElementTypesListView : public QListWidget
{
    Q_OBJECT

public:
    enum class DisplayNameKind { Type, Title };

    explicit ElementTypesListView(QWidget *parent = nullptr);
    
    void populate(Elements elems, DisplayNameKind displayNameKind = DisplayNameKind::Type);
    Element* selected() const;

signals:
    void enterPressed();
    void elementSelected(Element*);

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
    void adjust();

    Element* elem(QListWidgetItem*) const;
    QString iconPath(Element*) const;
    QSize iconSize() const;

private slots:
    void rowSelected(int index);
};

#endif // ELEMENT_TYPES_LISTVIEW_H
