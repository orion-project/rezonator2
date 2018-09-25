#include "SchemaLayout.h"

#include "Appearance.h"

#include <QtMath>

#define RadToDeg(angle) (angle * 180.0 / M_PI)
#define DegToRad(angle) (angle / 180.0 * M_PI)
#define Sqr(x) (x*x)

namespace {

QBrush getGlassBrush()
{
    static QBrush b = QBrush(QPixmap(":/misc32/glass_pattern"));
    return b;
}

QPen getGlassPen()
{
    static QPen p = QPen(Qt::black, 1.5);
    return p;
}

QBrush getMirrorBrush()
{
    static QBrush b = QBrush(Qt::black, Qt::BDiagPattern);
    return b;
}

QPen getMirrorPen()
{
    static QPen p = QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap);
    return p;
}

QPen getPlanePen()
{
    static QPen p = QPen(Qt::black, 1, Qt::DashLine);
    return p;
}

const QFont& getMarkTSFont()
{
    static QFont f = QFont("Arial", 8, QFont::Bold);
    return f;
}

const QFont& getLabelFont()
{
    static QFont f;
    static bool fontInited = false;
    if (!fontInited)
    {
        Z::Gui::adjustSymbolFont(f);
        fontInited = true;
    }
    return f;
}

} // namespace

//------------------------------------------------------------------------------
//                             ElementLayout
//------------------------------------------------------------------------------

ElementLayout::ElementLayout(Element* elem): QGraphicsItem(), _element(elem) {}
ElementLayout::~ElementLayout() {}

QRectF ElementLayout::boundingRect() const
{
    return QRectF(-HW, -HH, 2*HW, 2*HH);
}

void ElementLayout::setSlope(double elementAngle)
{
    if (elementAngle > 0)
        _slope = SlopePlus;
    else if (elementAngle < 0)
        _slope = SlopeMinus;
    else
        _slope = SlopeNone;
}

void ElementLayout::slopePainter(QPainter *painter)
{
    if (_slope != SlopeNone)
    {
        qreal angle = (_slope == SlopePlus)? _slopeAngle: -_slopeAngle;
        QTransform t = painter->transform();
        t.rotate(angle);
        painter->setTransform(t);

        t.rotate(-2*angle);
        QBrush b = painter->brush();
        b.setTransform(t);
        painter->setBrush(b);
    }
}

//------------------------------------------------------------------------------

#define DECLARE_ELEMENT_LAYOUT_BEGIN \
class Layout : public ElementLayout { \
public: \
    Layout(Element *elem) : ElementLayout(elem) {} \
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override; \
    void init() override;
#define DECLARE_ELEMENT_LAYOUT_END };
#define ELEMENT_LAYOUT_INIT void Layout::init()
#define ELEMENT_LAYOUT_PAINT void Layout::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)

//------------------------------------------------------------------------------
namespace OpticalAxisLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
    }

    ELEMENT_LAYOUT_PAINT {
        painter->drawLine(QLineF(-HW, 0, HW, 0));
    }
}

//------------------------------------------------------------------------------
namespace ElemEmptyRangeLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 50; HH = 5;
    }

    ELEMENT_LAYOUT_PAINT {
        Q_UNUSED(painter)
    }
}

//------------------------------------------------------------------------------
namespace ElemMediumRangeLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 30; HH = 40;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->fillRect(boundingRect(), getGlassBrush());
        painter->setPen(getGlassPen());
        painter->drawLine(QLineF(-HW, -HH, HW, -HH));
        painter->drawLine(QLineF(-HW, HH, HW, HH));
    }
}

//------------------------------------------------------------------------------
namespace ElemPlateLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 25; HH = 40;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        painter->drawRect(boundingRect());
    }
}

//------------------------------------------------------------------------------
namespace ElemFlatMirrorLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        enum {
            PlaceMiddle,
            PlaceLeft,
            PlaceRight
        } place = PlaceMiddle;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 7; HH = 40;
        if (!_element->owner()) return;
        int index = _element->owner()->indexOf(_element);
        if (index <= 0)
            place = PlaceLeft;
        else if (index >= _element->owner()->count()-1)
            place = PlaceRight;
    }

    ELEMENT_LAYOUT_PAINT {
        qreal x = 0;
        switch (place) {
        case PlaceLeft:
        case PlaceRight:
            painter->setBrush(getMirrorBrush());
            painter->setPen(getGlassPen());
            painter->drawRect(boundingRect());
            painter->setPen(getMirrorPen());
            x = (place == PlaceLeft)? HW: -HW;
            break;

        case PlaceMiddle:
            painter->setPen(getPlanePen());
            break;
        }
        painter->drawLine(QLineF(x, -HH, x, HH));
    }
}

//------------------------------------------------------------------------------
namespace CurvedElementLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        enum {
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
        } paintMode;
        enum {
            MarkNone,
            MarkT,
            MarkS
        } markTS = MarkNone;
        qreal ROC = 100;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 10; HH = 40;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setBrush(getGlassBrush());
        painter->setPen(getGlassPen());

        qreal sagitta = ROC - qSqrt(Sqr(ROC) - Sqr(HH));
        qreal startAngle = RadToDeg(qAsin(HH / ROC));
        qreal sweepAngle = 2*startAngle;

        QPainterPath path;
        QRectF rightSurface;
        QRectF leftSurface;

        switch (paintMode) {
        case ConvexLens: // ()
            rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
            leftSurface = QRectF(-HW, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW - sagitta, HH);
            path.arcTo(rightSurface, 360-startAngle, sweepAngle);
            path.lineTo(-HW + sagitta, -HH);
            path.arcTo(leftSurface, 180-startAngle, sweepAngle);
            break;

        case ConcaveLens: // )(
            rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
            leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW, -HH);
            path.arcTo(rightSurface, 180-startAngle, sweepAngle);
            path.lineTo(-HW, HH);
            path.arcTo(leftSurface, 360-startAngle, sweepAngle);
            break;

        case PlaneConvexMirror:
            painter->setBrush(getMirrorBrush());
            rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW - sagitta, HH);
            path.arcTo(rightSurface, 360-startAngle, sweepAngle);
            path.lineTo(-HW, -HH);
            path.lineTo(-HW, HH);
            break;

        case PlaneConvexLens:
            rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW - sagitta, HH);
            path.arcTo(rightSurface, 360-startAngle, sweepAngle);
            path.lineTo(-HW, -HH);
            path.lineTo(-HW, HH);
            break;

        case PlaneConcaveMirror:
            painter->setBrush(getMirrorBrush());
            rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW, -HH);
            path.arcTo(rightSurface, 180-startAngle, sweepAngle);
            path.lineTo(-HW, HH);
            path.lineTo(-HW, -HH);
            break;

        case PlaneConcaveLens:
            rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW, -HH);
            path.arcTo(rightSurface, 180-startAngle, sweepAngle);
            path.lineTo(-HW, HH);
            path.lineTo(-HW, -HH);
            break;

        case ConcavePlaneMirror:
            painter->setBrush(getMirrorBrush());
            leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW, HH);
            path.arcTo(leftSurface, 360-startAngle, sweepAngle);
            path.lineTo(HW, -HH);
            path.lineTo(HW, HH);
            break;

        case ConcavePlaneLens:
            leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW, HH);
            path.arcTo(leftSurface, 360-startAngle, sweepAngle);
            path.lineTo(HW, -HH);
            path.lineTo(HW, HH);
            break;

        case ConvexPlaneMirror:
            painter->setBrush(getMirrorBrush());
            leftSurface = QRectF(-HW, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW + sagitta, -HH);
            path.arcTo(leftSurface, 180-startAngle, sweepAngle);
            path.lineTo(HW, HH);
            path.lineTo(HW, -HH);
            break;

        case ConvexPlaneLens:
            leftSurface = QRectF(-HW, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW + sagitta, -HH);
            path.arcTo(leftSurface, 180-startAngle, sweepAngle);
            path.lineTo(HW, HH);
            path.lineTo(HW, -HH);
            break;
        }
        path.closeSubpath();

        slopePainter(painter);
        painter->drawPath(path);

        switch (paintMode) {
        case PlaneConvexMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(rightSurface, int(16*(360-startAngle)), int(16*sweepAngle));
            break;

        case PlaneConcaveMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(rightSurface, int(16*(180-startAngle)), int(16*sweepAngle));
            break;

        case ConcavePlaneMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(leftSurface, int(16*(360-startAngle)), int(16*sweepAngle));
            break;

        case ConvexPlaneMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(leftSurface, int(16*(180-startAngle)), int(16*sweepAngle));
            break;

        default: break;
        }

        if (markTS != MarkNone) {
            auto p = boundingRect().bottomRight();
            p.setX(p.x()-qreal(HW)/2.0);
            painter->setFont(getMarkTSFont());
            painter->drawText(p, MarkT? QStringLiteral("T"): QStringLiteral("S"));
        }
    }
}

//------------------------------------------------------------------------------
namespace ElemCurveMirrorLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        ElemCurveMirror *mirror = dynamic_cast<ElemCurveMirror*>(_element);
        if (!mirror || !_element->owner()) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr));
        layout->init();
        layout->setSlope(mirror->alpha());
        int index = _element->owner()->indexOf(_element);
        if (index <= 0)
            layout->paintMode = mirror->radius() > 0
                ? CurvedElementLayout::Layout::PlaneConcaveMirror
                : CurvedElementLayout::Layout::PlaneConvexMirror;
        else if (index >= _element->owner()->count()-1)
            layout->paintMode = mirror->radius() > 0
                ? CurvedElementLayout::Layout::ConcavePlaneMirror
                : CurvedElementLayout::Layout::ConvexPlaneMirror;
        else
            layout->paintMode = mirror->radius() > 0
                ? CurvedElementLayout::Layout::ConvexLens
                : CurvedElementLayout::Layout::ConcaveLens;
        HW = layout->halfW(); HH = layout->halfH();
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemThinLensLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        ElemThinLens *lens = dynamic_cast<ElemThinLens*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr));
        layout->setSlope(lens->alpha());
        layout->paintMode = lens->focus() > 0
                       ? CurvedElementLayout::Layout::ConvexLens
                       : CurvedElementLayout::Layout::ConcaveLens;
    }

    ELEMENT_LAYOUT_PAINT {
        layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemCylinderLensTLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        auto lens = dynamic_cast<ElemCylinderLensT*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr));
        layout->setSlope(lens->alpha());
        layout->markTS = CurvedElementLayout::Layout::MarkT;
        layout->paintMode = lens->focus() > 0
                           ? CurvedElementLayout::Layout::ConvexLens
                           : CurvedElementLayout::Layout::ConcaveLens;
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemCylinderLensSLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        auto lens = dynamic_cast<ElemCylinderLensS*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr));
        layout->setSlope(lens->alpha());
        layout->markTS = CurvedElementLayout::Layout::MarkS;
        layout->paintMode = lens->focus() > 0
                ? CurvedElementLayout::Layout::ConvexLens
                : CurvedElementLayout::Layout::ConcaveLens;
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace CrystalElementLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {}

    ELEMENT_LAYOUT_PAINT {
        qreal cline = HH * qTan(DegToRad(_slopeAngle));

        QPainterPath path;
        switch (_slope)
        {
        case SlopeNone:
            path.addRect(boundingRect());
            break;

        case SlopePlus:
            path.moveTo(-HW + cline, -HH);
            path.lineTo(HW + cline, -HH);
            path.lineTo(HW - cline, HH);
            path.lineTo(-HW - cline, HH);
            path.closeSubpath();
            break;

        case SlopeMinus:
            path.moveTo(-HW - cline, -HH);
            path.lineTo(HW - cline, -HH);
            path.lineTo(HW + cline, HH);
            path.lineTo(-HW + cline, HH);
            path.closeSubpath();
            break;
        }

        painter->setBrush(getGlassBrush());
        painter->setPen(getGlassPen());
        painter->drawPath(path);
    }
}

//------------------------------------------------------------------------------
namespace ElemTiltedCrystalLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CrystalElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 15; HH = 40;
        auto crystal = dynamic_cast<ElemTiltedCrystal*>(_element);
        if (!crystal) return;
        layout.reset(new CrystalElementLayout::Layout(nullptr));
        layout->setSlope(crystal->alpha());
        layout->setHalfSize(HW, HH);
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemTiltedPlateLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 15; HH = 40;
        auto plate = dynamic_cast<ElemTiltedPlate*>(_element);
        if (!plate) return;
        setSlope(plate->alpha());
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        slopePainter(painter);
        painter->drawRect(boundingRect());
    }
}

//------------------------------------------------------------------------------
namespace ElemBrewsterCrystalLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CrystalElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 30; HH = 30;
        layout.reset(new CrystalElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        layout->setSlopeAngle(40);
        layout->setSlope(SlopePlus);
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemBrewsterPlateLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 15; _slopeAngle = 40; _slope = SlopePlus;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        slopePainter(painter);
        painter->drawRect(boundingRect());
    }
}

//------------------------------------------------------------------------------
namespace ElemMatrixLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HH = 15; HW = 15;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->fillRect(boundingRect(), getMirrorBrush());
        painter->drawRect(QRectF(-HW, -HH, 2*HW, 2*HH));
        painter->drawLine(QLineF(0, -HH, 0, HH));
        painter->drawLine(QLineF(-HW, 0, HW, 0));
    }
}

//------------------------------------------------------------------------------
namespace ElemPointLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HH = 3; HW = 3;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(Qt::black);
        painter->drawEllipse(boundingRect());
    }
}

//------------------------------------------------------------------------------
namespace ElemNormalInterfaceLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 1.5; HH = 40;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->drawLine(QLineF(0, -HH, 0, HH));
    }
}

//------------------------------------------------------------------------------
namespace InterfaceElementLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    private:
        enum NeighbourKind {
            NeighbourUnknown,
            NeighbourAir,
            NeighbourMedium,
        };
        enum {
            BetweenRanges,
            BetweenMedia,
            MediumRight,
            MediumLeft,
        } position = BetweenMedia;
        enum {
            PlaneSurface,
            SphericalSurface,
        } surface = PlaneSurface;
        qreal ROC = 100;
        QRectF surfaceRect;
        QPainterPath fillPath, drawPath;
        NeighbourKind getNeighbour(Schema* schema, int index) const;
        void addSurface(QPainterPath &path) const;
        void paintSpherical(QPainter *painter) const;
        void paintPlane(QPainter *painter) const;
    DECLARE_ELEMENT_LAYOUT_END

    Layout::NeighbourKind Layout::getNeighbour(Schema* schema, int index) const {
        auto elem = schema->element(index);
        if (!elem) return NeighbourUnknown;

        auto medium = dynamic_cast<ElemMediumRange*>(elem);
        if (medium) return NeighbourMedium;

        auto air = dynamic_cast<ElemEmptyRange*>(elem);
        if (air) return NeighbourAir;

        return NeighbourUnknown;
    }

    ELEMENT_LAYOUT_INIT {
        auto schema = dynamic_cast<Schema*>(_element->owner());
        if (!schema) return;

        auto index = schema->indexOf(_element);
        auto left = getNeighbour(schema, index-1);
        auto right = getNeighbour(schema, index+1);
        if (left == NeighbourAir && right == NeighbourAir)
            position = BetweenRanges;
        else if (left == NeighbourMedium && right == NeighbourMedium)
            position = BetweenMedia;
        else if (left == NeighbourAir && right == NeighbourMedium)
            position = MediumRight;
        else if (left == NeighbourMedium && right == NeighbourAir)
            position = MediumLeft;

        auto spherical = dynamic_cast<ElemSphericalInterface*>(_element);
        if (spherical) {
            surface = SphericalSurface;
            qreal sagitta = ROC - qSqrt(Sqr(ROC) - Sqr(HH));
            qreal startAngle = RadToDeg(qAsin(HH / ROC));
            qreal sweepAngle = 2*startAngle;
            if (spherical->radius() > 0) {

            }
            else {

            }
            surfaceRect = rightSurface : leftSurface;
        }
    }

    void Layout::addSurface(QPainterPath &path) const {
    }

    void Layout::paintSpherical(QPainter *painter) const {
        qreal sagitta = ROC - qSqrt(Sqr(ROC) - Sqr(HH));
        qreal startAngle = RadToDeg(qAsin(HH / ROC));
        qreal sweepAngle = 2*startAngle;

        QPainterPath path;
        QRectF rightSurface(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
        QRectF leftSurface(-HW, -ROC, 2*ROC, 2*ROC);

        switch (position) {
        case BetweenRanges:
            break;
        }
        path.moveTo(HW - sagitta, HH);
        path.arcTo(rightSurface, 360-startAngle, sweepAngle);

    }

    void Layout::paintPlane(QPainter *painter) const {
        switch (position) {
        case BetweenRanges:
            painter->setPen(getGlassPen());
            painter->drawLine(QLineF(HW, -HH, -HW, HH));
            break;

        case BetweenMedia:
            painter->fillRect(boundingRect(), getGlassBrush());
            painter->setPen(getGlassPen());
            painter->drawLine(QLineF(-HW, -HH, HW, -HH));
            painter->drawLine(QLineF(HW, -HH, -HW, HH));
            painter->drawLine(QLineF(-HW, HH, HW, HH));
            break;

        case MediumLeft: {
                QPainterPath path;
                path.moveTo(-HW, -HH);
                path.lineTo(HW, -HH);
                path.lineTo(-HW, HH);
                path.closeSubpath();
                painter->fillPath(path, getGlassBrush());

                painter->setPen(getGlassPen());
                painter->drawLine(QLineF(-HW, -HH, HW, -HH));
                painter->drawLine(QLineF(HW, -HH, -HW, HH));
            }
            break;

        case MediumRight: {
                QPainterPath path;
                path.moveTo(HW, -HH);
                path.lineTo(-HW, HH);
                path.lineTo(HW, HH);
                path.closeSubpath();
                painter->fillPath(path, getGlassBrush());

                painter->setPen(getGlassPen());
                painter->drawLine(QLineF(HW, -HH, -HW, HH));
                painter->drawLine(QLineF(-HW, HH, HW, HH));
            }
            break;
        }
    }

    ELEMENT_LAYOUT_PAINT {
        switch (surface) {
        case PlaneSurface:
            paintPlane(painter);
            break;

        case SphericalSurface:
            paintSpherical(painter);
            break;
        }
    }
}

//------------------------------------------------------------------------------
namespace ElemBrewsterInterfaceLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<InterfaceElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 30; HH = 40;
        layout.reset(new InterfaceElementLayout::Layout(_element));
        layout->setHalfSize(HW, HH);
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemTiltedInterfaceLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<InterfaceElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 15; HH = 40;
        layout.reset(new InterfaceElementLayout::Layout(_element));
        layout->setHalfSize(HW, HH);
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
//                               SchemaLayout
//------------------------------------------------------------------------------

SchemaLayout::SchemaLayout(Schema *schema, QWidget* parent) : QGraphicsView(parent), _schema(schema)
{
    _axis = new OpticalAxisLayout::Layout(nullptr);
    _axis->setZValue(1000);
    _scene.addItem(_axis);

    setRenderHint(QPainter::Antialiasing, true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setScene(&_scene);
}

SchemaLayout::~SchemaLayout()
{
    clear();
}

void SchemaLayout::populate()
{
    setUpdatesEnabled(false);

    clear();

    for (int i = 0; i < _schema->count(); i++) {
        Element *elem = _schema->element(i);
        if (elem->disabled()) continue;
        auto layout = ElementLayoutFactory::make(elem);
        if (layout) {
            layout->init();
            addElement(layout);
        }
    }

    // Calculate axis length and position.
    // The first element is at zero position.
    qreal fullW = 0;
    qreal firstHW = -1;
    for (ElementLayout *elem : _elements) {
        fullW += 2*elem->halfW();
        if (firstHW < 0)
            firstHW = elem->halfW();
    }
    const qreal axisMargin = 10;
    fullW += axisMargin + axisMargin;
    _axis->setHalfSize(fullW/2, 5);
    _axis->setX(fullW/2 - firstHW - axisMargin);

    centerView();
    setUpdatesEnabled(true);
}

void SchemaLayout::addElement(ElementLayout *elem)
{
    if (!_elements.isEmpty())
    {
        ElementLayout *last = _elements.last();
        elem->setPos(last->x() + last->halfW() + elem->halfW(), 0);
    }
    else
        elem->setPos(0, 0);

    _elements.append(elem);
    _scene.addItem(elem);

    // Add element label
    QGraphicsTextItem *label = _scene.addText(elem->element()->label());
    label->setZValue(1000 + _elements.count());
    label->setFont(getLabelFont());
    QRectF r = label->boundingRect();
    label->setX(elem->x() - r.width() / 2.0);
    label->setY(elem->y() - elem->halfH() - r.height());
    _elemLabels.insert(elem, label);
}

void SchemaLayout::clear()
{
    _scene.removeItem(_axis);
    _scene.clear();
    _elemLabels.clear();
    _scene.addItem(_axis);
    _elements.clear();
}

void SchemaLayout::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    centerView();
}

void SchemaLayout::centerView()
{
    QRectF r;
    for (ElementLayout *elem : _elements)
        r = r.united(elem->boundingRect());
    for (QGraphicsTextItem *item : _elemLabels.values())
        r = r.united(item->boundingRect());
    centerOn(r.center());
}

//------------------------------------------------------------------------------
//                             ElementLayoutFactory
//------------------------------------------------------------------------------

namespace ElementLayoutFactory {

static QMap<QString, std::function<ElementLayout*(Element*)>> __factoryMethods;

template <class TElement, class TLayout> void registerLayout() {
    TElement tmp;
    __factoryMethods.insert(tmp.type(), [](Element*e) { return new TLayout(e); });
}

ElementLayout* make(Element *elem) {
    if (__factoryMethods.empty()) {
        registerLayout<ElemEmptyRange, ElemEmptyRangeLayout::Layout>();
        registerLayout<ElemMediumRange, ElemMediumRangeLayout::Layout>();
        registerLayout<ElemPlate, ElemPlateLayout::Layout>();
        registerLayout<ElemFlatMirror, ElemFlatMirrorLayout::Layout>();
        registerLayout<ElemCurveMirror, ElemCurveMirrorLayout::Layout>();
        registerLayout<ElemThinLens, ElemEmptyRangeLayout::Layout>();
        registerLayout<ElemCylinderLensT, ElemCylinderLensTLayout::Layout>();
        registerLayout<ElemCylinderLensS, ElemCylinderLensSLayout::Layout>();
        registerLayout<ElemTiltedCrystal, ElemTiltedCrystalLayout::Layout>();
        registerLayout<ElemTiltedPlate, ElemTiltedPlateLayout::Layout>();
        registerLayout<ElemBrewsterCrystal, ElemBrewsterCrystalLayout::Layout>();
        registerLayout<ElemBrewsterPlate, ElemBrewsterPlateLayout::Layout>();
        registerLayout<ElemMatrix, ElemMatrixLayout::Layout>();
        registerLayout<ElemPoint, ElemPointLayout::Layout>();
        registerLayout<ElemNormalInterface, ElemNormalInterfaceLayout::Layout>();
        registerLayout<ElemBrewsterInterface, ElemBrewsterInterfaceLayout::Layout>();
        registerLayout<ElemTiltedInterface, ElemTiltedInterfaceLayout::Layout>();
    }
    if (!__factoryMethods.contains(elem->type()))
        return nullptr;
    return __factoryMethods[elem->type()](elem);
}

} // namespace ElementLayoutFactory
