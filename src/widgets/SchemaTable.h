#ifndef SCHEMATABLE_H
#define SCHEMATABLE_H

#include <QTableWidget>

#include "../core/Schema.h"
#include "../core/Element.h"

#define COL_IMAGE 0
#define COL_LABEL 1
#define COL_PARAMS 2
#define COL_TITLE 3

/**
    Widget presenting a schema in table view.
*/
class SchemaTable:
        public QTableWidget,
        public SchemaListener,
        public ElementSelector
{
    Q_OBJECT

public:
    explicit SchemaTable(Schema *schema, QWidget *parent = 0);
    ~SchemaTable();

    void populateParams();
    void setContextMenu(QMenu *menu) { _contextMenu = menu; }

    Schema* schema() const { return _schema; }

    Element* selected() const;
    void setSelected(Element*);

    Elements selection() const;
    QVector<int> selectedRows() const;

    bool hasSelection() const;

    ///// inherits from SchemaListener
    void schemaLoaded(Schema*);
    void elementCreated(Schema*, Element*);
    void elementChanged(Schema*, Element*);
    void elementDeleting(Schema*, Element*);
    void schemaParamsChanged(Schema*);

signals:
    void doubleClicked(Element*);
    
private slots:
    void doubleClicked(class QTableWidgetItem*);
    void showContextMenu(const QPoint&);

private:
    Schema *_schema;
    QMenu *_contextMenu;

    void adjustColumns();
    void populate();
    void createRow(Element *elem, int row);
    void populateRow(Element *elem, int row);
};

#endif // SCHEMATABLE_H
