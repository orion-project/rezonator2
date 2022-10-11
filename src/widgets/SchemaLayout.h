#ifndef SCHEMA_LAYOUT_H
#define SCHEMA_LAYOUT_H

#include "GraphicsView.h"
#include "../core/Schema.h"
#include "../core/Elements.h"

#include <QGraphicsItem>

#define Sqr(x) ((x)*(x))

/**
    Graphical representation of single element.
*/
class ElementLayout : public QGraphicsItem
{
public:
    enum Slope
    {
        SlopeNone,
        SlopePlus,
        SlopeMinus
    };

    ElementLayout(Element* elem);
    ~ElementLayout() override;

    Element* element() const { return _element; }

    virtual void init() = 0;

    QRectF boundingRect() const override;

    qreal halfW() const { return HW; }
    qreal halfH() const { return HH; }
    void setHalfSize(qreal hw, qreal hh) { HW = hw; HH = hh; }
    void setSlope(Slope slope) { _slope = slope; }
    void setSlope(double elementAngle);
    void setSlopeAngle(qreal angle) { _slopeAngle = angle; }

    void makeElemToolTip();

protected:
    Element* _element;
    Slope _slope = SlopeNone;
    qreal _slopeAngle = 15;
    qreal HW;
    qreal HH;

    void slopePainter(QPainter *painter);
};


#define DECLARE_ELEMENT_LAYOUT_BEGIN \
class Layout : public ElementLayout { \
public: \
    Layout(Element *elem) : ElementLayout(elem) {} \
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override; \
    void init() override;
#define DECLARE_ELEMENT_LAYOUT_END };
#define ELEMENT_LAYOUT_INIT void Layout::init()
#define ELEMENT_LAYOUT_PAINT void Layout::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)


/**
    Graphical representation of a schema.
*/
class SchemaLayout : public Z::GraphicsView, public SchemaListener
{
    Q_OBJECT

public:
    explicit SchemaLayout(Schema *schema, QWidget* parent = nullptr);
    ~SchemaLayout() override;

    // inherits from SchemaListener
    void schemaLoaded(Schema*) override { populate(); }
    void schemaRebuilt(Schema*) override { populate(); }
    void elementCreated(Schema*, Element*) override { populate(); }
    void elementChanged(Schema*, Element*) override { populate(); }
    void elementDeleted(Schema*, Element*) override { populate(); }

private:
    Schema *_schema;
    QGraphicsScene _scene;
    ElementLayout *_axis;
    QVector<ElementLayout*> _elements;
    QMap<ElementLayout*, QGraphicsTextItem*> _elemLabels;

    void addElement(ElementLayout *elem);
    void populate();
    void clear();
    void centerView(const QRectF&);
};


namespace ElementLayoutFactory {
ElementLayout* make(Element *elem);
}

namespace ElementLayoutProps {
QBrush getGlassBrush();
QPen getGlassPen();
QBrush getMirrorBrush();
QPen getMirrorPen();
QPen getPlanePen();
QBrush getGrinBrush(double sizeF);
const QFont& getMarkTSFont();
const QFont& getLabelFont();
} // namespace ElementLayoutProps

#endif // SCHEMA_LAYOUT_H
