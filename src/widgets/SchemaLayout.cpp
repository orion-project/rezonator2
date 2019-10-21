#include "SchemaLayout.h"

#include "../Appearance.h"
#include "../AppSettings.h"
#include "../funcs/FormatInfo.h"

#include <QClipboard>
#include <QtMath>
#include <QMenu>

#define Sqr(x) ((x)*(x))

namespace {

QBrush getGlassBrush()
{
    static QBrush b = QBrush(QPixmap(":/misc/glass_pattern"));
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

QBrush getGrinBrush(double sizeF)
{
    int size = int(sizeF);
    static QMap<int, QBrush> brushes;
    if (!brushes.contains(size))
    {
        QLinearGradient g(0, -size, 0, size);
        g.setColorAt(0, Qt::white);
        g.setColorAt(0.5, Qt::gray);
        g.setColorAt(1, Qt::white);
        brushes[size] = QBrush(g);
    }
    return brushes[size];
}

const QFont& getMarkTSFont()
{
    static QFont f = QFont("Arial", 8, QFont::Bold);
    return f;
}

const QFont& getLabelFont()
{
    static QFont f = Z::Gui::ElemLabelFont().get();
    return f;
}

} // namespace

//------------------------------------------------------------------------------
//                             ElementLayout
//------------------------------------------------------------------------------

ElementLayout::ElementLayout(Element* elem): QGraphicsItem(), _element(elem)
{
}

ElementLayout::~ElementLayout()
{
}

void ElementLayout::makeElemToolTip()
{
    if (!_element) return;
    auto schema = dynamic_cast<Schema*>(_element->owner());
    if (!schema) return;

    Z::Format::FormatElemParams f;
    f.schema = schema;

    setToolTip(QStringLiteral("#%1 <b>%2</b> <i>(%3)</i><br>%4")
               .arg(schema->indexOf(_element)+1)
               .arg(_element->label())
               .arg(_element->typeName())
               .arg(f.format(_element))
               );
}

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
    enum CurvedForm {
        FormUnknown,
        ConvexLens,         //      ()
        ConcaveLens,        //      )(
        ConvexConcaveLens,  //      ((
        ConcaveConvexLens,  //      ))
        PlanoConvexMirror,  //      |)
        PlanoConvexLens,    //      |)
        PlanoConcaveMirror, //      |(
        PlanoConcaveLens,   //      |(
        ConvexPlanoMirror,  //      (|
        ConvexPlanoLens,    //      (|
        ConcavePlanoMirror, //      )|
        ConcavePlanoLens    //      )|
    };

    DECLARE_ELEMENT_LAYOUT_BEGIN
        CurvedForm paintMode = FormUnknown;
        void paintCornerMark(QPainter *painter, const QString& mark) const;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setBrush(getGlassBrush());
        painter->setPen(getGlassPen());

        const qreal ROC = 100;
        const qreal sagitta = ROC - qSqrt(Sqr(ROC) - Sqr(HH));
        const qreal startAngle = qRadiansToDegrees(qAsin(HH / ROC));
        const qreal sweepAngle = 2*startAngle;

        QPainterPath path;
        QRectF rightSurface;
        QRectF leftSurface;

        switch (paintMode) {
        case FormUnknown:
            break;

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

        case ConvexConcaveLens: // ((
            leftSurface = QRectF(-HW, -ROC, 2*ROC, 2*ROC);
            rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW + sagitta, -HH);
            path.arcTo(leftSurface, 180-startAngle, sweepAngle);
            path.lineTo(HW, HH);
            path.arcTo(rightSurface, 180+startAngle, -sweepAngle);
            break;

        case ConcaveConvexLens: // ))
            leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW, -HH);
            path.arcTo(leftSurface, startAngle, -sweepAngle);
            path.lineTo(HW - sagitta, HH);
            path.arcTo(rightSurface, -startAngle, sweepAngle);
            break;

        case PlanoConvexMirror:
            painter->setBrush(getMirrorBrush());
            rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW - sagitta, HH);
            path.arcTo(rightSurface, 360-startAngle, sweepAngle);
            path.lineTo(-HW, -HH);
            path.lineTo(-HW, HH);
            break;

        case PlanoConvexLens:
            rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW - sagitta, HH);
            path.arcTo(rightSurface, 360-startAngle, sweepAngle);
            path.lineTo(-HW, -HH);
            path.lineTo(-HW, HH);
            break;

        case PlanoConcaveMirror:
            painter->setBrush(getMirrorBrush());
            rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW, -HH);
            path.arcTo(rightSurface, 180-startAngle, sweepAngle);
            path.lineTo(-HW, HH);
            path.lineTo(-HW, -HH);
            break;

        case PlanoConcaveLens:
            rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW, -HH);
            path.arcTo(rightSurface, 180-startAngle, sweepAngle);
            path.lineTo(-HW, HH);
            path.lineTo(-HW, -HH);
            break;

        case ConcavePlanoMirror:
            painter->setBrush(getMirrorBrush());
            leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW, HH);
            path.arcTo(leftSurface, 360-startAngle, sweepAngle);
            path.lineTo(HW, -HH);
            path.lineTo(HW, HH);
            break;

        case ConcavePlanoLens:
            leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW, HH);
            path.arcTo(leftSurface, 360-startAngle, sweepAngle);
            path.lineTo(HW, -HH);
            path.lineTo(HW, HH);
            break;

        case ConvexPlanoMirror:
            painter->setBrush(getMirrorBrush());
            leftSurface = QRectF(-HW, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW + sagitta, -HH);
            path.arcTo(leftSurface, 180-startAngle, sweepAngle);
            path.lineTo(HW, HH);
            path.lineTo(HW, -HH);
            break;

        case ConvexPlanoLens:
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
        case PlanoConvexMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(rightSurface, int(16*(360-startAngle)), int(16*sweepAngle));
            break;

        case PlanoConcaveMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(rightSurface, int(16*(180-startAngle)), int(16*sweepAngle));
            break;

        case ConcavePlanoMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(leftSurface, int(16*(360-startAngle)), int(16*sweepAngle));
            break;

        case ConvexPlanoMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(leftSurface, int(16*(180-startAngle)), int(16*sweepAngle));
            break;

        default: break;
        }
    }

    void Layout::paintCornerMark(QPainter *painter, const QString& mark) const {
        auto p = boundingRect().bottomRight();
        p.setX(p.x()-qreal(HW)/2.0);
        painter->setFont(getMarkTSFont());
        painter->drawText(p, mark);
    }
}

//------------------------------------------------------------------------------
namespace ElemCurveMirrorLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 10; HH = 40;
        ElemCurveMirror *mirror = dynamic_cast<ElemCurveMirror*>(_element);
        if (!mirror || !_element->owner()) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        layout->setSlope(mirror->alpha());
        int index = _element->owner()->indexOf(_element);
        if (index <= 0)
            layout->paintMode = mirror->radius() > 0
                ? CurvedElementLayout::PlanoConcaveMirror
                : CurvedElementLayout::PlanoConvexMirror;
        else if (index >= _element->owner()->count()-1)
            layout->paintMode = mirror->radius() > 0
                ? CurvedElementLayout::ConcavePlanoMirror
                : CurvedElementLayout::ConvexPlanoMirror;
        else
            layout->paintMode = mirror->radius() > 0
                ? CurvedElementLayout::ConvexLens
                : CurvedElementLayout::ConcaveLens;
        layout->init();
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
        HW = 10; HH = 40;
        auto lens = dynamic_cast<ElemThinLens*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        layout->setSlope(lens->alpha());
        layout->paintMode = lens->focus() > 0
                       ? CurvedElementLayout::ConvexLens
                       : CurvedElementLayout::ConcaveLens;
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemCylinderLensTLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 10; HH = 40;
        auto lens = dynamic_cast<ElemCylinderLensT*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        layout->setSlope(lens->alpha());
        layout->paintMode = lens->focus() > 0
                           ? CurvedElementLayout::ConvexLens
                           : CurvedElementLayout::ConcaveLens;
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) {
            layout->paint(painter, nullptr, nullptr);
            layout->paintCornerMark(painter, QStringLiteral("T"));
        }
    }
}

//------------------------------------------------------------------------------
namespace ElemCylinderLensSLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 10; HH = 40;
        auto lens = dynamic_cast<ElemCylinderLensS*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        layout->setSlope(lens->alpha());
        layout->paintMode = lens->focus() > 0
                ? CurvedElementLayout::ConvexLens
                : CurvedElementLayout::ConcaveLens;
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) {
            layout->paint(painter, nullptr, nullptr);
            layout->paintCornerMark(painter, QStringLiteral("S"));
        }
    }
}

//------------------------------------------------------------------------------
namespace CrystalElementLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {}

    ELEMENT_LAYOUT_PAINT {
        qreal cline = HH * qTan(qDegreesToRadians(_slopeAngle));

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
        layout->init();
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
        layout->init();
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
        HW = 15; HH = 40; _slopeAngle = 40; _slope = SlopePlus;
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
        HW = 5; HH = 5;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(Qt::black);
        painter->drawEllipse(-3, -3, 6, 6);
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
namespace InterfaceElementHeper {

enum class ElementPlacement {
    BetweenRanges,
    BetweenMedia,
    BeforeMedium,
    AfterMedium,
};

enum class ElementNeighbour {
    Unknown,
    Air,
    Medium,
};

ElementNeighbour getNeighbour(Schema* schema, int index) {
    auto elem = schema->element(index);
    if (elem) {
        if (dynamic_cast<ElemMediumRange*>(elem))
            return ElementNeighbour::Medium;

        if (dynamic_cast<ElemEmptyRange*>(elem))
            return ElementNeighbour::Air;
    }
    else if (schema->isSP())
        return ElementNeighbour::Air;
    return ElementNeighbour::Unknown;
}

ElementPlacement getPlacement(Element* elem) {
    auto schema = dynamic_cast<Schema*>(elem->owner());
    if (!schema) return ElementPlacement::BetweenMedia;

    auto index = schema->indexOf(elem);
    auto left = getNeighbour(schema, index-1);
    auto right = getNeighbour(schema, index+1);

    if (left == ElementNeighbour::Air && right == ElementNeighbour::Air)
        return ElementPlacement::BetweenRanges;

    if (left == ElementNeighbour::Medium && right == ElementNeighbour::Medium)
        return ElementPlacement::BetweenMedia;

    if (left == ElementNeighbour::Air && right == ElementNeighbour::Medium)
        return ElementPlacement::BeforeMedium;

    if (left == ElementNeighbour::Medium && right == ElementNeighbour::Air)
        return ElementPlacement::AfterMedium;

    return ElementPlacement::BetweenMedia;
}

}

using namespace InterfaceElementHeper;

//------------------------------------------------------------------------------
namespace PlaneInterfaceElementLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        ElementPlacement placement;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        placement = getPlacement(_element);
    }

    ELEMENT_LAYOUT_PAINT {
        switch (placement) {
        case ElementPlacement::BetweenRanges:
            painter->setPen(getPlanePen());
            painter->drawLine(QLineF(HW, -HH, -HW, HH));
            break;

        case ElementPlacement::BetweenMedia:
            painter->fillRect(boundingRect(), getGlassBrush());
            painter->setPen(getGlassPen());
            painter->drawLine(QLineF(-HW, -HH, HW, -HH));
            painter->drawLine(QLineF(HW, -HH, -HW, HH));
            painter->drawLine(QLineF(-HW, HH, HW, HH));
            break;

        case ElementPlacement::AfterMedium: {
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

        case ElementPlacement::BeforeMedium: {
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
}

//------------------------------------------------------------------------------
namespace ElemBrewsterInterfaceLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<PlaneInterfaceElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 30; HH = 40;
        layout.reset(new PlaneInterfaceElementLayout::Layout(_element));
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
        QSharedPointer<PlaneInterfaceElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 15; HH = 40;
        layout.reset(new PlaneInterfaceElementLayout::Layout(_element));
        layout->setHalfSize(HW, HH);
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemSphericalInterfaceLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    private:
        InterfaceElementHeper::ElementPlacement _placement;
        QRectF _surface;
        qreal _startAngle;
        qreal _sweepAngle;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 15; HH = 40;
        _placement = InterfaceElementHeper::getPlacement(_element);
        auto intf = dynamic_cast<ElemSphericalInterface*>(_element);
        const qreal ROC = 100;
        const qreal sagitta = ROC - qSqrt(Sqr(ROC) - Sqr(HH));
        const qreal startAngle = qRadiansToDegrees(qAsin(HH / ROC));
        if (intf && intf->radius() < 0) {
            _surface = QRectF(-sagitta, -ROC, 2*ROC, 2*ROC);
            _startAngle = 180 - startAngle;
            _sweepAngle = 2*startAngle;
        }
        else {
            _surface = QRectF(sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            _startAngle = startAngle;
            _sweepAngle = -2*startAngle;
        }
    }

    ELEMENT_LAYOUT_PAINT {
        switch (_placement) {
        case ElementPlacement::BetweenRanges:
            painter->setPen(getPlanePen());
            painter->drawArc(_surface, int(_startAngle)*16, int(_sweepAngle)*16);
            break;

        case ElementPlacement::BetweenMedia:
            painter->fillRect(boundingRect(), getGlassBrush());
            painter->setPen(getGlassPen());
            painter->drawLine(QLineF(-HW, -HH, HW, -HH));
            painter->drawLine(QLineF(-HW, HH, HW, HH));
            painter->drawArc(_surface, int(_startAngle)*16, int(_sweepAngle)*16);
            break;

        case ElementPlacement::AfterMedium: {
                QPainterPath path;
                path.moveTo(-HW, -HH);
                path.lineTo(0, -HH);
                path.arcTo(_surface, _startAngle, _sweepAngle);
                path.lineTo(-HW, HH);
                path.closeSubpath();
                painter->fillPath(path, getGlassBrush());

                painter->setPen(getGlassPen());
                QPainterPath path1;
                path1.moveTo(-HW, -HH);
                path1.lineTo(0, -HH);
                path1.arcTo(_surface, _startAngle, _sweepAngle);
                path1.lineTo(-HW, HH);
                painter->drawPath(path1);
            }
            break;

        case ElementPlacement::BeforeMedium: {
                QPainterPath path;
                path.moveTo(HW, -HH);
                path.lineTo(0, -HH);
                path.arcTo(_surface, _startAngle, _sweepAngle);
                path.lineTo(HW, HH);
                path.closeSubpath();
                painter->fillPath(path, getGlassBrush());

                painter->setPen(getGlassPen());
                QPainterPath path1;
                path1.moveTo(HW, -HH);
                path1.lineTo(0, -HH);
                path1.arcTo(_surface, _startAngle, _sweepAngle);
                path1.lineTo(HW, HH);
                painter->drawPath(path1);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
namespace ElemThickLensLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 25; HH=40;
        auto lens = dynamic_cast<ElemThickLens*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        auto R1 = lens->radius1();
        auto R2 = lens->radius2();
        if (R1 < 0 && R2 > 0) // ()
            layout->paintMode = CurvedElementLayout::ConvexLens;
        else if (R1 > 0 && R2 < 0) // )(
            layout->paintMode = CurvedElementLayout::ConcaveLens;
        else if (R1 < 0 && R2 < 0) // ((
            layout->paintMode = CurvedElementLayout::ConvexConcaveLens;
        else if (R1 > 0 && R2 > 0) // ))
            layout->paintMode = CurvedElementLayout::ConcaveConvexLens;
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemGrinLensLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 25; HH = 40;
    }

    ELEMENT_LAYOUT_PAINT {
        QPainterPath path;
        path.addRect(boundingRect());
        painter->setBrush(getGrinBrush(HH));
        painter->setPen(getGlassPen());
        painter->drawPath(path);
    }
}

//------------------------------------------------------------------------------
namespace AxiconElementLayout {
    enum AxiconForm {
        FormUnknown,
        ConvexLens,         //      <>
        ConcaveLens,        //      ><
        PlanoConvexMirror,  //      |>
        PlanoConvexLens,    //      |>
        PlanoConcaveMirror, //      |<
        PlanoConcaveLens,   //      |<
        ConvexPlanoMirror,  //      <|
        ConcavePlanoMirror, //      >|
    };

    DECLARE_ELEMENT_LAYOUT_BEGIN
        AxiconForm paintMode = FormUnknown;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
    }

    ELEMENT_LAYOUT_PAINT {
        const qreal sagitta = HW * 0.85;

        painter->setPen(getGlassPen());

        switch (paintMode) {
        case FormUnknown:
            break;
        case ConvexLens: // <>
        case ConcaveLens: // )(
        case PlanoConvexLens: // |>
        case PlanoConcaveLens: // |<
            painter->setBrush(getGlassBrush());
            break;
        case PlanoConvexMirror: // |>
        case PlanoConcaveMirror: // |<
        case ConvexPlanoMirror:  // <|
        case ConcavePlanoMirror: // >|
            painter->setBrush(getMirrorBrush());
            break;
        }

        QPainterPath path;

        switch (paintMode) {
        case FormUnknown:
            break;

        case ConvexLens: // <>
            path.moveTo(HW, 0);
            path.lineTo(HW - sagitta, HH);
            path.lineTo(-HW + sagitta, HH);
            path.lineTo(-HW, 0);
            path.lineTo(-HW + sagitta, -HH);
            path.lineTo(HW - sagitta, -HH);
            break;

        case ConcaveLens: // )(
            path.moveTo(HW - sagitta, 0);
            path.lineTo(HW, HH);
            path.lineTo(-HW, HH);
            path.lineTo(-HW + sagitta, 0);
            path.lineTo(-HW, -HH);
            path.lineTo(HW, -HH);
            break;

        case PlanoConvexMirror: // |>
        case PlanoConvexLens: // |>
            path.moveTo(-HW * 0.5, HH);
            path.lineTo(HW - sagitta, HH);
            path.lineTo(HW, 0);
            path.lineTo(HW - sagitta, -HH);
            path.lineTo(-HW * 0.5, -HH);
            break;

        case PlanoConcaveMirror: // |<
        case PlanoConcaveLens: // |<
            path.moveTo(0, 0);
            path.lineTo(HW, HH);
            path.lineTo(-HW * 0.5, HH);
            path.lineTo(-HW * 0.5, -HH);
            path.lineTo(HW, -HH);
            break;

        case ConvexPlanoMirror:  // <|
            path.moveTo(HW * 0.5, HH);
            path.lineTo(-HW + sagitta, HH);
            path.lineTo(-HW, 0);
            path.lineTo(-HW + sagitta, -HH);
            path.lineTo(HW * 0.5, -HH);
            break;

        case ConcavePlanoMirror: // >|
            path.moveTo(HW * 0.5, HH);
            path.lineTo(-HW, HH);
            path.lineTo(0, 0);
            path.lineTo(-HW, -HH);
            path.lineTo(HW * 0.5, -HH);
            break;
        }
        path.closeSubpath();

        slopePainter(painter);
        painter->drawPath(path);

        painter->setPen(getMirrorPen());
        painter->setBrush(Qt::NoBrush);
        QPainterPath path1;
        switch (paintMode) {
        case PlanoConvexMirror: // |>
            path1.moveTo(HW - sagitta, HH);
            path1.lineTo(HW, 0);
            path1.lineTo(HW - sagitta, -HH);
            break;

        case PlanoConcaveMirror: // |<
            path1.moveTo(HW, HH);
            path1.lineTo(HW - sagitta, 0);
            path1.lineTo(HW, -HH);
            break;

        case ConvexPlanoMirror:  // <|
            path1.moveTo(-HW + sagitta, HH);
            path1.lineTo(-HW, 0);
            path1.lineTo(-HW + sagitta, -HH);
            break;

        case ConcavePlanoMirror: // >|
            path1.moveTo(-HW, HH);
            path1.lineTo(-HW + sagitta, 0);
            path1.lineTo(-HW, -HH);
            break;

        default: break;
        }
        painter->drawPath(path1);
    }
}

//------------------------------------------------------------------------------
namespace ElemAxiconMirrorLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<AxiconElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 12; HH = 40;
        auto mirror = dynamic_cast<ElemAxiconMirror*>(_element);
        if (!mirror || !_element->owner()) return;
        layout.reset(new AxiconElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        layout->setSlope(mirror->alpha());
        int index = _element->owner()->indexOf(_element);
        if (index <= 0)
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::PlanoConcaveMirror
                : AxiconElementLayout::PlanoConvexMirror;
        else if (index >= _element->owner()->count()-1)
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::ConcavePlanoMirror
                : AxiconElementLayout::ConvexPlanoMirror;
        else
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::ConvexLens
                : AxiconElementLayout::ConcaveLens;
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemAxiconLensLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<AxiconElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 12; HH = 40;
        auto mirror = dynamic_cast<ElemAxiconLens*>(_element);
        if (!mirror || !_element->owner()) return;
        layout.reset(new AxiconElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        layout->setSlope(mirror->alpha());
        int index = _element->owner()->indexOf(_element);
        if (index <= 0)
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::PlanoConcaveMirror
                : AxiconElementLayout::PlanoConvexMirror;
        else if (index >= _element->owner()->count()-1)
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::ConcavePlanoMirror
                : AxiconElementLayout::ConvexPlanoMirror;
        else
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::PlanoConvexLens
                : AxiconElementLayout::PlanoConcaveLens;
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
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
            layout->makeElemToolTip();
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

    auto r = _scene.itemsBoundingRect();
    r.adjust(-10, -10, 10, 10);
    _scene.setSceneRect(r);
    centerView(r);

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
    label->setToolTip(elem->toolTip());
    // Try to position new label avoiding overlapping with previous labels
    QRectF r = label->boundingRect();
    qreal labelX = elem->x() - r.width() / 2.0;
    qreal labelY = elem->y() - elem->halfH() - r.height();
    qreal minY = labelY;
    for (int prevIndex = _elements.size()-2; prevIndex >= 0; prevIndex--) {
        auto prevLabel = _elemLabels[_elements.at(prevIndex)];
        auto prevRect = prevLabel->boundingRect();
        if (labelX <= prevLabel->x() + prevRect.width() &&
            labelY <= prevLabel->y() && labelY > prevLabel->y() - prevRect.height())
            labelY = minY - prevRect.height()*0.75;
        else minY = qMin(minY, prevLabel->y());
    }
    label->setX(labelX);
    label->setY(labelY);
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
    centerView(QRectF());
}

void SchemaLayout::centerView(const QRectF& rect)
{
    QRectF r = rect.isEmpty() ? _scene.itemsBoundingRect(): rect;
    centerOn(r.center());
}

void SchemaLayout::contextMenuEvent(QContextMenuEvent *event)
{
    if (!_menu) _menu = createContextMenu();
    _menu->popup(this->mapToGlobal(event->pos()));
}

QMenu* SchemaLayout::createContextMenu()
{
    auto menu = new QMenu(this);
    menu->addAction(QIcon(":/toolbar/copy_img"), tr("Copy Image"), this, &SchemaLayout::copyImage);
    return menu;
}

void SchemaLayout::copyImage()
{
    QImage image(_scene.sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(AppSettings::instance().layoutExportTransparent ? Qt::transparent : Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    _scene.render(&painter);

    qApp->clipboard()->setImage(image);
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
        registerLayout<ElemThinLens, ElemThinLensLayout::Layout>();
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
        registerLayout<ElemSphericalInterface, ElemSphericalInterfaceLayout::Layout>();
        registerLayout<ElemThickLens, ElemThickLensLayout::Layout>();
        registerLayout<ElemGrinLens, ElemGrinLensLayout::Layout>();
        registerLayout<ElemAxiconMirror, ElemAxiconMirrorLayout::Layout>();
        registerLayout<ElemAxiconLens, ElemAxiconLensLayout::Layout>();
    }
    if (!__factoryMethods.contains(elem->type()))
        return nullptr;
    return __factoryMethods[elem->type()](elem);
}

} // namespace ElementLayoutFactory
