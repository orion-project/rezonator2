#ifndef ELEMENTS_TABLE_H
#define ELEMENTS_TABLE_H

#include "../core/Schema.h"
#include "../core/Element.h"

#include <QTableView>

class ElementsTableModel;

class ElementsTable : public QTableView, public ElementSelector
{
    Q_OBJECT

public:
    explicit ElementsTable(Schema* schema, QWidget *parent = nullptr);
    ~ElementsTable();

    // Inherited from ElementSelector
    Element* selected() const override { return currentElem(); }
    Elements selection() const override;

    void selectElems(const Elements& elems);

    Element* currentElem() const;
    void setCurrentElem(Element*);
    int currentRow() const;

    QMenu *elementContextMenu = nullptr;
    QMenu *lastRowContextMenu = nullptr;

protected:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

signals:
    void currentElemChanged(Element*);
    void selectedElemsChanged(const Elements&);
    void elemDoubleClicked(Element*);

private slots:
     void currentRowChanged(const QModelIndex &current, const QModelIndex &);
     void indexDoubleClicked(const QModelIndex &index);
     void showContextMenu(const QPoint&);

private:
    Schema* _schema;
    ElementsTableModel* _model;
};

#endif // ELEMENTS_TABLE_H
