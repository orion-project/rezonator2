#ifndef SCHEMA_LAYOUT_H
#define SCHEMA_LAYOUT_H

#include "../core/Schema.h"
#include "../core/Elements.h"

#include <QGraphicsItem>
#include <QGraphicsView>

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

/**
    Graphical representation of a schema.
*/
class SchemaLayout : public QGraphicsView, public SchemaListener
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

protected:
    QGraphicsScene _scene;

    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    Schema *_schema;
    ElementLayout *_axis;
    QVector<ElementLayout*> _elements;
    QMap<ElementLayout*, QGraphicsTextItem*> _elemLabels;
    QMenu* _menu = nullptr;

    void addElement(ElementLayout *elem);
    void populate();
    void clear();
    void centerView(const QRectF&);
    QMenu* createContextMenu();
    void copyImage();
};


namespace ElementLayoutFactory {
ElementLayout* make(Element *elem);
}

#endif // SCHEMA_LAYOUT_H
