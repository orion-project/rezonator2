#ifndef SCHEMA_LAYOUT_H
#define SCHEMA_LAYOUT_H

#include "../core/Schema.h"
#include "../core/Elements.h"

#include <QGraphicsItem>
#include <QGraphicsView>

//------------------------------------------------------------------------------

/**
    Graphical representation of single element.
*/
class ElementView : public QGraphicsItem
{
public:
    ElementView(Element* elem): QGraphicsItem(), _element(elem){}
    ~ElementView() override {}

    Element *element() const { return _element; }

    virtual void init() = 0;

    QRectF boundingRect() const override;

    qreal width() const { return 2*HW; }
    qreal height() const { return 2*HH; }
    qreal halfw() const { return HW; }
    qreal halfh() const { return HH; }

protected:
    Element* _element;

    qreal HW;
    qreal HH = 40;
};

//------------------------------------------------------------------------------

class ElementViewMaker
{
public:
    virtual ~ElementViewMaker();
    virtual ElementView *make(Element*) = 0;
};

class ElementViewFactory
{
public:
    ElementViewFactory();
    ~ElementViewFactory();
    ElementView* makeView(Element*) const;
private:
    QMap<QString, ElementViewMaker*> _makers;
};

//------------------------------------------------------------------------------
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
    ElementViewFactory _factory;
    class OpticalAxisView *_axis;
    QMap<ElementView*, QGraphicsTextItem*> _elemLabels;

    void addElementView(ElementView *elem);
    void addLabelView(ElementView* elem);
    void populate();
    void clear();
    void adjustRanges(int fullWidth);
};

#endif // SCHEMA_LAYOUT_H
