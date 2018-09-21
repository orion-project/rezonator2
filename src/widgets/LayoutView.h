#ifndef LAYOUTVIEW_H
#define LAYOUTVIEW_H

#include <QGraphicsView>
#include <QGraphicsItem>

class ElementView;
class OpticalAxisView;

class ElementView : public QGraphicsItem
{
public:
    enum Slope
    {
        SlopeNone,
        SlopePlus,
        SlopeMinus
    };
    ElementView() {}

    const QString& label() const { return _label; }
    void setLabel(const QString& value) { _label = value; }

    Slope slope() const { return _slope; }
    void setSlope(Slope value) { _slope = value; }


protected:
    void slopePainter(QPainter *painter);
    QString _label;
    Slope _slope = SlopeNone;
    qreal _slopeAngle = 15;
};


class ElementRangeView : public ElementView
{
public:
    ElementRangeView() { HW = 50; HH = 5; }
    virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
    qreal length() const { return 2*HW; }
    void setLength(const qreal &value) { HW = value / 2.0; }
};


class ElemMediumRangeView : public ElementView
{
public:
    ElemMediumRangeView() { HW = 30; }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
};


class ElemPlateView : public ElementView
{
public:
    ElemPlateView() { HW = 25; }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
};


class ElemTiltedPlateView : public ElemPlateView
{
public:
    ElemTiltedPlateView() { HW = 15; }
};


class ElemTiltedCrystalView : public ElementView
{
public:
    ElemTiltedCrystalView() { HW = 15; }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
};


class ElemBrewsterPlateView : public ElemTiltedPlateView
{
public:
    ElemBrewsterPlateView() { HW = 15; _slopeAngle = 40; _slope = SlopePlus; }
};


class ElemBrewsterCrystalView : public ElemTiltedCrystalView
{
public:
    ElemBrewsterCrystalView() { HW = 30; HH = 30; _slopeAngle = 40; _slope = SlopePlus; }
};


class ElemPointView : public ElementView
{
public:
    ElemPointView() { HH = 3; HW = 3; }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
};


class ElemMatrixView : public ElementView
{
public:
    ElemMatrixView() { HH = 15; HW = 15; }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
};


class ElemFlatMirrorView : public ElementView
{
public:
    enum Place
    {
        PlaceMiddle,
        PlaceLeft,
        PlaceRight
    };
    ElemFlatMirrorView() { HW = 7; _place = PlaceMiddle; }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
    Place place() const { return _place; }
    void setPlace(Place value) { _place = value; }
private:
    Place _place;
};

class CurvedElementView : public ElementView
{
public:
    enum PaintMode
    {
        ConvexLens,         //      ()
        ConcaveLens,        //      )(
        PlaneConvexMirror,  //      |)
        PlaneConvexLens,    //      |)
        PlaneConcaveMirror, //      |(
        PlaneConcaveLens,   //      |(
        ConvexPlaneMirror,  //      (|
        ConvexPlaneLens,    //      (|
        ConcavePlaneMirror, //      )|
        ConcavePlaneLens    //      )|
    };
    enum MarkTS { MarkNone, MarkT, MarkS };
    CurvedElementView() { HW = 10; ROC = 100; _paintMode = ConvexLens; }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
    PaintMode paintMode() const { return _paintMode; }
    void setPaintMode(PaintMode value) { _paintMode = value; }
    MarkTS markTS() const { return _markTS; }
    void setMarkTS(MarkTS mark) { _markTS = mark; }
protected:
    PaintMode _paintMode;
    qreal ROC;
    MarkTS _markTS = MarkNone;
    const QString& strMarkTS() const;
};

#endif // LAYOUTVIEW_H
