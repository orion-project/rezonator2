#ifndef SCHEMA_ELEMS_TABLE_H
#define SCHEMA_ELEMS_TABLE_H

#include <QTableWidget>

#include "../core/Schema.h"
#include "../core/Element.h"

/**
    Widget presenting a schema in table view.
*/
class SchemaElemsTable: public QTableWidget, public SchemaListener, public ElementSelector
{
    Q_OBJECT

public:
    explicit SchemaElemsTable(Schema *schema, QWidget *parent = nullptr);

    void populateParams();

    Schema* schema() const { return _schema; }

    Element* selected() const override;
    void setSelected(Element*);

    Elements selection() const override;
    QList<int> selectedRows() const;

    bool hasSelection() const;

    // inherits from SchemaListener
    void schemaLoaded(Schema*) override;
    void schemaRebuilt(Schema*) override;
    void elementCreated(Schema*, Element*) override;
    void elementChanged(Schema*, Element*) override;
    void elementDeleting(Schema*, Element*) override;

    QMenu *elementContextMenu = nullptr;
    QMenu *lastRowContextMenu = nullptr;

signals:
    void doubleClicked(Element*);

private slots:
    void doubleClicked(class QTableWidgetItem*);
    void showContextMenu(const QPoint&);

private:
    enum { COL_IMAGE, COL_LABEL, COL_PARAMS, COL_TITLE, COL_COUNT };

    Schema *_schema;

    void adjustColumns();
    void populate();
    void createRow(Element *elem, int row);
    void populateRow(Element *elem, int row);
    void fillPlaceholderRow();
};

#endif // SCHEMA_ELEMS_TABLE_H
