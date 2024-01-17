#ifndef SCHEMA_LAYOUT_H
#define SCHEMA_LAYOUT_H

#include "../core/Schema.h"

#include <QGraphicsView>
#include <QGraphicsItem>

#define Sqr(x) ((x)*(x))

class SchemaLayout;

//------------------------------------------------------------------------------

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

    ElementLayout(Element* elem, SchemaLayout* owner);
    ~ElementLayout() override;

    Element* element() const { return _element; }

    bool isSelected() const { return _selected; }
    virtual void setSelected(bool on) { _selected = on; }

    virtual void init() {}

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
    SchemaLayout* _owner;
    Slope _slope = SlopeNone;
    qreal _slopeAngle = 15;
    qreal HW;
    qreal HH;
    bool _selected = false;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent*) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) override;
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;

    void slopePainter(QPainter *painter);

    const QPen& getSelectedAxisPen() const;
    const QBrush& getGlassBrush() const;
    const QPen& getGlassPen() const;
    const QBrush& getMirrorBrush() const;
    const QPen& getMirrorPen() const;
    const QPen& getPlanePen() const;
    const QBrush& getGrinBrush(double sizeF) const;
    const QFont& getMarkTSFont() const;
};

//------------------------------------------------------------------------------

class ElemLabelItem : public QGraphicsTextItem
{
public:
    ElemLabelItem(Element* elem, SchemaLayout* owner);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent*) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) override;
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;

private:
    Element* _element;
    SchemaLayout* _owner;
};

//------------------------------------------------------------------------------

class SchemaScene : public QGraphicsScene
{
public:
    SchemaScene(SchemaLayout* owner);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent*) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;

private:
    SchemaLayout* _owner;
};

//------------------------------------------------------------------------------

class ElementLayoutOptionsView
{
public:
    virtual bool hasAltVersion() const { return false; }
    virtual const char *altVersionOptionTitle() const;
};

//------------------------------------------------------------------------------

/**
    Graphical representation of a schema.
*/
class SchemaLayout : public QGraphicsView, public SchemaListener
{
    Q_OBJECT

public:
    explicit SchemaLayout(Schema *schema, QWidget* parent = nullptr);
    ~SchemaLayout() override;

    void updateSelection(const Elements& selected);
    std::function<Elements()> getSelection;
    void elementClicked(Element* elem, bool multiselect);

    // inherits from SchemaListener
    void schemaLoaded(Schema*) override { populate(); }
    void schemaRebuilt(Schema*) override { populate(); }
    void elementCreated(Schema*, Element*) override { populate(); }
    void elementChanged(Schema*, Element*) override { populate(); }
    void elementDeleted(Schema*, Element*) override { populate(); }

    QMenu* elementContextMenu;
    QMenu* paperContextMenu() const { return _paperContextMenu; }

signals:
    void selectedElemsChanged(const Elements&);
    void elemDoubleClicked(Element*);

private:
    Schema *_schema;
    SchemaScene *_scene;
    ElementLayout *_axis;
    QVector<ElementLayout*> _elements;
    QMap<Element*, ElementLayout*> _elemLayouts;
    QMap<ElementLayout*, ElemLabelItem*> _elemLabels;
    QColor _defaultLabelColor;
    QColor _selectedLabelColor = Qt::blue;
    QMenu* _paperContextMenu;

    void addElement(ElementLayout *elem);
    void populate();
    void clear();
    void centerView(const QRectF&);
    void copyImage() const;
    const QFont& getLabelFont() const;
};

//------------------------------------------------------------------------------

namespace ElementLayoutFactory {
ElementLayout* make(Element *elem, SchemaLayout *owner);
ElementLayoutOptionsView *getOptions(Element *elem);
}

#endif // SCHEMA_LAYOUT_H
