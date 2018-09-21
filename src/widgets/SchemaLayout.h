#ifndef SCHEMA_LAYOUT_H
#define SCHEMA_LAYOUT_H

#include "../core/Schema.h"
#include "../core/Elements.h"

#include <QGraphicsItem>
#include <QGraphicsView>

class ElementView;

namespace ElementViewFactory {
ElementView* makeElementView(Element *elem);
}

/**
    Graphical representation of a schema.
*/
class SchemaLayout : public QGraphicsView, public SchemaListener
{
public:
    explicit SchemaLayout(Schema *schema, QWidget* parent = nullptr);
    ~SchemaLayout() override;

    // inherits from SchemaListener
    void schemaLoaded(Schema*) override { populate(); }
    void elementCreated(Schema*, Element*) override { populate(); }
    void elementChanged(Schema*, Element*) override { populate(); }
    void elementDeleted(Schema*, Element*) override { populate(); }

protected:
    QGraphicsScene _scene;

    void resizeEvent(QResizeEvent *event) override;

private:
    Schema *_schema;
    QVector<ElementView*> _elements;
    ElementView *_axis;
    QMap<ElementView*, QGraphicsTextItem*> _elemLabels;

    void addElementView(ElementView *elem);
    void addLabelView(ElementView* elem);
    void populate();
    void clear();
    void adjustRanges(int fullWidth);
};

#endif // SCHEMA_LAYOUT_H
