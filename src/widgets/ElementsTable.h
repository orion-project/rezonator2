#ifndef ELEMENTS_TABLE_H
#define ELEMENTS_TABLE_H

#include "../core/Schema.h"
#include "../core/Element.h"

#include <QTableView>

class ElementsTableModel;

class ElementsTable : public QTableView, public SchemaListener, public ElementSelector
{
    Q_OBJECT

public:
    explicit ElementsTable(Schema* schema, QWidget *parent = nullptr);
    ~ElementsTable();

    // Inherited from ElementSelector
    Element* selected() const override { return currentElem(); }
    Elements selection() const override;

    Element* currentElem() const;
    void setCurrentElem(Element*);
    int currentRow() const;

signals:
    void currentElemChanged(Element*);
    void elemDoubleClicked(Element*);

private slots:
     void currentRowChanged(const QModelIndex &current, const QModelIndex &);
     void indexDoubleClicked(const QModelIndex &index);

private:
    Schema* _schema;
    ElementsTableModel* _model;
};

#endif // ELEMENTS_TABLE_H
