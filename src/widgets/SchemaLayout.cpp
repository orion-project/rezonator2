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
//                             ElementView
//------------------------------------------------------------------------------

/**
    Graphical representation of single element.
*/
class ElementView : public QGraphicsItem
{
public:
    enum Slope
    {
        SlopeNone,
        SlopePlus,
        SlopeMinus
    };

    ElementView(Element* elem): QGraphicsItem(), element(elem){}
    ~ElementView() override {}

    virtual void init() = 0;

    QRectF boundingRect() const override;

    qreal width() const { return 2*HW; }
    qreal height() const { return 2*HH; }
    qreal halfw() const { return HW; }
    qreal halfh() const { return HH; }

    Element* element;
    Slope slope = SlopeNone;
    qreal slopeAngle = 15;
    qreal HW;
    qreal HH;

    void initSlope(double angle);
    void slopePainter(QPainter *painter);
};

QRectF ElementView::boundingRect() const
{
    return QRectF(-HW, -HH, 2*HW, 2*HH);
}

void ElementView::initSlope(double angle)
{
    if (angle > 0)
        slope = SlopePlus;
    else if (angle < 0)
        slope = SlopeMinus;
    else
        slope = SlopeNone;
}

void ElementView::slopePainter(QPainter *painter)
{
    if (slope != SlopeNone)
    {
        qreal angle = (slope == SlopePlus)? slopeAngle: -slopeAngle;
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

#define DECLARE_ELEMENT_VIEW_BEGIN \
class View : public ElementView { \
public: \
    View(Element *elem) : ElementView(elem) {} \
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override; \
    void init() override;
#define DECLARE_ELEMENT_VIEW_END };
#define ELEMENT_VIEW_INIT void View::init()
#define ELEMENT_VIEW_PAINT void View::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)

//------------------------------------------------------------------------------
namespace OpticalAxisView {
    DECLARE_ELEMENT_VIEW_BEGIN
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        HW = 50; HH = 5;
    }

    ELEMENT_VIEW_PAINT {
        painter->drawLine(QLineF(-HW, 0, HW, 0));
    }
}

//------------------------------------------------------------------------------
namespace ElemEmptyRangeView {
    DECLARE_ELEMENT_VIEW_BEGIN
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        HW = 50; HH = 5;
    }

    ELEMENT_VIEW_PAINT {
        Q_UNUSED(painter)
    }
}

//------------------------------------------------------------------------------
namespace ElemMediumRangeView {
    DECLARE_ELEMENT_VIEW_BEGIN
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        HW = 30; HH = 40;
    }

    ELEMENT_VIEW_PAINT {
        painter->fillRect(boundingRect(), getGlassBrush());
        painter->setPen(getGlassPen());
        painter->drawLine(QLineF(-HW, -HH, HW, -HH));
        painter->drawLine(QLineF(-HW, HH, HW, HH));
    }
}

//------------------------------------------------------------------------------
namespace ElemPlateView {
    DECLARE_ELEMENT_VIEW_BEGIN
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        HW = 25; HH = 40;
    }

    ELEMENT_VIEW_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        painter->drawRect(boundingRect());
    }
}

//------------------------------------------------------------------------------
namespace ElemFlatMirrorView {
    DECLARE_ELEMENT_VIEW_BEGIN
        enum {
            PlaceMiddle,
            PlaceLeft,
            PlaceRight
        } place = PlaceMiddle;
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        HW = 7; HH = 40;
        if (!element->owner()) return;
        int index = element->owner()->indexOf(element);
        if (index <= 0)
            place = PlaceLeft;
        else if (index >= element->owner()->count()-1)
            place = PlaceRight;
    }

    ELEMENT_VIEW_PAINT {
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
namespace CurvedElementView {
    DECLARE_ELEMENT_VIEW_BEGIN
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
        } markTS;
        qreal ROC = 100;
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        HW = 10; HH = 40;
    }

    ELEMENT_VIEW_PAINT {
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
namespace ElemCurveMirrorView {
    DECLARE_ELEMENT_VIEW_BEGIN
        QSharedPointer<CurvedElementView::View> view;
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        ElemCurveMirror *mirror = dynamic_cast<ElemCurveMirror*>(element);
        if (!mirror || !element->owner()) return;
        view.reset(new CurvedElementView::View(nullptr));
        view->initSlope(mirror->alpha());
        int index = element->owner()->indexOf(element);
        if (index <= 0)
            view->paintMode = mirror->radius() > 0
                ? CurvedElementView::View::PlaneConcaveMirror
                : CurvedElementView::View::PlaneConvexMirror;
        else if (index >= element->owner()->count()-1)
            view->paintMode = mirror->radius() > 0
                ? CurvedElementView::View::ConcavePlaneMirror
                : CurvedElementView::View::ConvexPlaneMirror;
        else
            view->paintMode = mirror->radius() > 0
                ? CurvedElementView::View::ConvexLens
                : CurvedElementView::View::ConcaveLens;
    }

    ELEMENT_VIEW_PAINT {
        if (view) view->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemThinLensView {
    DECLARE_ELEMENT_VIEW_BEGIN
        QSharedPointer<CurvedElementView::View> view;
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        ElemThinLens *lens = dynamic_cast<ElemThinLens*>(element);
        if (!lens) return;
        view.reset(new CurvedElementView::View(nullptr));
        view->initSlope(lens->alpha());
        view->paintMode = lens->focus() > 0
                       ? CurvedElementView::View::ConvexLens
                       : CurvedElementView::View::ConcaveLens;
    }

    ELEMENT_VIEW_PAINT {
        view->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemCylinderLensTView {
    DECLARE_ELEMENT_VIEW_BEGIN
        QSharedPointer<CurvedElementView::View> view;
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        auto lens = dynamic_cast<ElemCylinderLensT*>(element);
        if (!lens) return;
        view.reset(new CurvedElementView::View(nullptr));
        view->initSlope(lens->alpha());
        view->markTS = CurvedElementView::View::MarkT;
        view->paintMode = lens->focus() > 0
                           ? CurvedElementView::View::ConvexLens
                           : CurvedElementView::View::ConcaveLens;
    }

    ELEMENT_VIEW_PAINT {
        if (view) view->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemCylinderLensSView {
    DECLARE_ELEMENT_VIEW_BEGIN
        QSharedPointer<CurvedElementView::View> view;
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        auto lens = dynamic_cast<ElemCylinderLensS*>(element);
        if (!lens) return;
        view.reset(new CurvedElementView::View(nullptr));
        view->initSlope(lens->alpha());
        view->markTS = CurvedElementView::View::MarkS;
        view->paintMode = lens->focus() > 0
                ? CurvedElementView::View::ConvexLens
                : CurvedElementView::View::ConcaveLens;
    }

    ELEMENT_VIEW_PAINT {
        if (view) view->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace CrystalElementView {
    DECLARE_ELEMENT_VIEW_BEGIN
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {}

    ELEMENT_VIEW_PAINT {
        qreal cline = HH * qTan(DegToRad(slopeAngle));

        QPainterPath path;
        switch (slope)
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
namespace ElemTiltedCrystalView {
    DECLARE_ELEMENT_VIEW_BEGIN
        QSharedPointer<CrystalElementView::View> view;
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        HW = 15; HH = 40;
        auto crystal = dynamic_cast<ElemTiltedCrystal*>(element);
        if (!crystal) return;
        view.reset(new CrystalElementView::View(nullptr));
        view->initSlope(crystal->alpha());
        view->HW = HW; view->HH = HH;
    }

    ELEMENT_VIEW_PAINT {
        if (view) view->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemTiltedPlateView {
    DECLARE_ELEMENT_VIEW_BEGIN
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        HW = 15; HH = 40;
        auto plate = dynamic_cast<ElemTiltedPlate*>(element);
        if (!plate) return;
        initSlope(plate->alpha());
    }

    ELEMENT_VIEW_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        slopePainter(painter);
        painter->drawRect(boundingRect());
    }
}

//------------------------------------------------------------------------------
namespace ElemBrewsterCrystalView {
    DECLARE_ELEMENT_VIEW_BEGIN
        QSharedPointer<CrystalElementView::View> view;
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        view.reset(new CrystalElementView::View(nullptr));
        HW = 30; HH = 30;
        view->HW = HW; view->HH = HH;
        view->slopeAngle = 40; view->slope = SlopePlus;
    }

    ELEMENT_VIEW_PAINT {
        if (view) view->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemBrewsterPlateView {
    DECLARE_ELEMENT_VIEW_BEGIN
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        HW = 15; slopeAngle = 40; slope = SlopePlus;
    }

    ELEMENT_VIEW_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        slopePainter(painter);
        painter->drawRect(boundingRect());
    }
}

//------------------------------------------------------------------------------
namespace ElemMatrixView {
    DECLARE_ELEMENT_VIEW_BEGIN
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        HH = 15; HW = 15;
    }

    ELEMENT_VIEW_PAINT {
        painter->setPen(getGlassPen());
        painter->fillRect(boundingRect(), getMirrorBrush());
        painter->drawRect(QRectF(-HW, -HH, 2*HW, 2*HH));
        painter->drawLine(QLineF(0, -HH, 0, HH));
        painter->drawLine(QLineF(-HW, 0, HW, 0));
    }
}

//------------------------------------------------------------------------------
namespace ElemPointView {
    DECLARE_ELEMENT_VIEW_BEGIN
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
        HH = 3; HW = 3;
    }

    ELEMENT_VIEW_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(Qt::black);
        painter->drawEllipse(boundingRect());
    }
}

//------------------------------------------------------------------------------
namespace ElemNormalInterfaceView {
    DECLARE_ELEMENT_VIEW_BEGIN
    DECLARE_ELEMENT_VIEW_END

    ELEMENT_VIEW_INIT {
    }

    ELEMENT_VIEW_PAINT {
    }
}

//------------------------------------------------------------------------------
//                               SchemaLayout
//------------------------------------------------------------------------------

SchemaLayout::SchemaLayout(Schema *schema, QWidget* parent) : QGraphicsView(parent), _schema(schema) {
    _axis = new OpticalAxisView::View(nullptr);
    _axis->setZValue(1000);
    _scene.addItem(_axis);

    setRenderHint(QPainter::Antialiasing, true);
    setScene(&_scene);
}

SchemaLayout::~SchemaLayout() {
    clear();
}

void SchemaLayout::populate() {
    clear();
    for (Element *elem : _schema->elements()) {
        if (elem->disabled()) continue;
        auto view = ElementViewFactory::makeElementView(elem);
        if (view) {
            view->init();
            addElementView(view);
        }
    }
}

void SchemaLayout::addElementView(ElementView *elem)
{
    if (!_elements.isEmpty())
    {
        ElementView *last = _elements.last();
        elem->setPos(last->x() + last->halfw() + elem->halfw(), 0);
    }
    else
        elem->setPos(0, 0);

    _elements.append(elem);

    qreal fullWidth = 0;
    foreach (ElementView *elem, _elements)
        fullWidth += elem->width();

    _axis->setLength(fullWidth + 20);
    _axis->setX(_axis->halfw() - _elements.first()->halfw() - 10);

    _scene.addItem(elem);

    addLabelView(elem);
}

void SchemaLayout::addLabelView(ElementView* elem)
{
    QGraphicsTextItem *label = _scene.addText(elem->element()->label());
    label->setZValue(1000 + _elements.count());
    label->setFont(getLabelFont());
    QRectF r = label->boundingRect();
    label->setX(elem->x() - r.width() / 2.0);
    label->setY(elem->y() - elem->halfh() - r.height());
    _elemLabels.insert(elem, label);
}

void SchemaLayout::clear()
{
    _scene.removeItem(_axis);
    _scene.clear();
    _elemLabels.clear();
    _scene.addItem(_axis);
    qDeleteAll(_elements);
    _elements.clear();
}

void SchemaLayout::resizeEvent(QResizeEvent *event)
{
    adjustRanges(event->size().width());
}

void SchemaLayout::adjustRanges(int fullWidth)
{
    //for (ElementView *elemView: _elems)
}

//------------------------------------------------------------------------------
//                             ElementViewFactory
//------------------------------------------------------------------------------

namespace ElementViewFactory {

static QMap<QString, std::function<ElementView*(Element*)>> __factoryMethods;

template <class TElement, class TView> void registerView() {
    TElement tmp;
    __factoryMethods.insert(tmp.type(), [](Element*e) { return new TView(e); });
}

ElementView* makeElementView(Element *elem) {
    if (__factoryMethods.empty()) {
        registerView<ElemEmptyRange, ElemEmptyRangeView::View>();
        registerView<ElemMediumRange, ElemMediumRangeView::View>();
        registerView<ElemPlate, ElemPlateView::View>();
        registerView<ElemFlatMirror, ElemFlatMirrorView::View>();
        registerView<ElemCurveMirror, ElemCurveMirrorView::View>();
        registerView<ElemThinLens, ElemEmptyRangeView::View>();
        registerView<ElemEmptyRange, ElemThinLensView::View>();
        registerView<ElemCylinderLensT, ElemCylinderLensTView::View>();
        registerView<ElemCylinderLensS, ElemCylinderLensSView::View>();
        registerView<ElemTiltedCrystal, ElemTiltedCrystalView::View>();
        registerView<ElemTiltedPlate, ElemTiltedPlateView::View>();
        registerView<ElemBrewsterCrystal, ElemBrewsterCrystalView::View>();
        registerView<ElemBrewsterPlate, ElemBrewsterPlateView::View>();
        registerView<ElemMatrix, ElemMatrixView::View>();
        registerView<ElemPoint, ElemPointView::View>();
        registerView<ElemNormalInterface, ElemNormalInterfaceView::View>();
    }
    if (!__factoryMethods.contains(elem->type()))
        return nullptr;
    return __factoryMethods[elem->type()](elem);
}

} // namespace ElementViewFactory
