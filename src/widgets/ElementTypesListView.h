#ifndef ELEMENT_TYPES_LISTVIEW_H
#define ELEMENT_TYPES_LISTVIEW_H

#include <QListWidget>

#include "../core/Element.h"

class ElementTypesListView : public QListWidget
{
    Q_OBJECT

public:
    explicit ElementTypesListView(QWidget *parent = nullptr);
    
    void populate(Elements elems);
    const QString selected() const;

signals:
    void enterPressed();
    void elementSelected(const QString& type);

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
    void adjust();

    QString elemType(QListWidgetItem*) const;
    QString iconPath(Element*) const;
    QSize iconSize() const;

private slots:
    void rowSelected(int index);
};

#endif // ELEMENT_TYPES_LISTVIEW_H
