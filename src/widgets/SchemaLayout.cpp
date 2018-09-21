#include "SchemaLayout.h"

#include "Appearance.h"

namespace {

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

QRectF ElementView::boundingRect() const
{
    return QRectF(-HW, -HH, 2*HW, 2*HH);
}

//------------------------------------------------------------------------------
//                             OpticalAxisView
//------------------------------------------------------------------------------

class OpticalAxisView : public ElementView
{
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        painter->drawLine(QLineF(-HW, 0, HW, 0));
    }
};


//------------------------------------------------------------------------------

#define DECLARE_ELEMENT_LAYOUT(elem_type) \
    class elem_type ## View : public ElementView { \
    public: \
        elem_type ## View(Element *elem) : ElementView(elem) { init(); } \
        void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override; \
        void init() override;

#define DECLARE_ELEMENT_LAYOUT_END(elem_type)  }; \
    class elem_type ## ViewMaker : public ElementViewMaker { \
    public: \
        ElementView* make(Element* elem) override; \
    }; \
    ElementView* elem_type ## ViewMaker::make(Element* elem) { \
        return new elem_type ## View(elem); \
    }

DECLARE_ELEMENT_LAYOUT(ElemEmptyRange)
DECLARE_ELEMENT_LAYOUT_END(ElemEmptyRange)

DECLARE_ELEMENT_LAYOUT(ElemMediumRange)
DECLARE_ELEMENT_LAYOUT_END(ElemMediumRange)

DECLARE_ELEMENT_LAYOUT(ElemPlate)
DECLARE_ELEMENT_LAYOUT_END(ElemPlate)

DECLARE_ELEMENT_LAYOUT(ElemFlatMirror)
DECLARE_ELEMENT_LAYOUT_END(ElemFlatMirror)

DECLARE_ELEMENT_LAYOUT(ElemCurveMirror)
DECLARE_ELEMENT_LAYOUT_END(ElemCurveMirror)

DECLARE_ELEMENT_LAYOUT(ElemThinLens)
DECLARE_ELEMENT_LAYOUT_END(ElemThinLens)

DECLARE_ELEMENT_LAYOUT(ElemCylinderLensT)
DECLARE_ELEMENT_LAYOUT_END(ElemCylinderLensT)

DECLARE_ELEMENT_LAYOUT(ElemCylinderLensS)
DECLARE_ELEMENT_LAYOUT_END(ElemCylinderLensS)

DECLARE_ELEMENT_LAYOUT(ElemTiltedCrystal)
DECLARE_ELEMENT_LAYOUT_END(ElemTiltedCrystal)

DECLARE_ELEMENT_LAYOUT(ElemTiltedPlate)
DECLARE_ELEMENT_LAYOUT_END(ElemTiltedPlate)

DECLARE_ELEMENT_LAYOUT(ElemBrewsterCrystal)
DECLARE_ELEMENT_LAYOUT_END(ElemBrewsterCrystal)

DECLARE_ELEMENT_LAYOUT(ElemBrewsterPlate)
DECLARE_ELEMENT_LAYOUT_END(ElemBrewsterPlate)

DECLARE_ELEMENT_LAYOUT(ElemMatrix)
DECLARE_ELEMENT_LAYOUT_END(ElemMatrix)

DECLARE_ELEMENT_LAYOUT(ElemPoint)
DECLARE_ELEMENT_LAYOUT_END(ElemPoint)


void ElemEmptyRangeView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    Q_UNUSED(painter)
}

//------------------------------------------------------------------------------
//                             ElementLayoutMaker
//------------------------------------------------------------------------------

ElementViewMaker::~ElementViewMaker()
{
}

//------------------------------------------------------------------------------
//                             ElementLayoutMakers
//------------------------------------------------------------------------------

ElementViewFactory::ElementViewFactory()
{
    #define ELEMENT_VIEW_MAKER(elem_type) { \
        elem_type elem; \
        _makers[elem.type()] = new (elem_type ## ViewMaker); \
    }

    ELEMENT_VIEW_MAKER(ElemEmptyRange);
    ELEMENT_VIEW_MAKER(ElemMediumRange);
    ELEMENT_VIEW_MAKER(ElemPlate);
    ELEMENT_VIEW_MAKER(ElemFlatMirror);
    ELEMENT_VIEW_MAKER(ElemCurveMirror);
    ELEMENT_VIEW_MAKER(ElemThinLens);
    ELEMENT_VIEW_MAKER(ElemCylinderLensT);
    ELEMENT_VIEW_MAKER(ElemCylinderLensS);
    ELEMENT_VIEW_MAKER(ElemTiltedCrystal);
    ELEMENT_VIEW_MAKER(ElemTiltedPlate);
    ELEMENT_VIEW_MAKER(ElemBrewsterCrystal);
    ELEMENT_VIEW_MAKER(ElemBrewsterPlate);
    ELEMENT_VIEW_MAKER(ElemMatrix);
    ELEMENT_VIEW_MAKER(ElemPoint);

    #undef ELEMENT_VIEW_MAKER
}

ElementViewFactory::~ElementViewFactory()
{
    qDeleteAll(_makers.values());
}

ElementView* ElementViewFactory::makeView(Element *elem) const
{
    if (!_makers.contains(elem->type()))
        return nullptr;
    return _makers[elem->type()]->make(elem);
}

//------------------------------------------------------------------------------
//                               SchemaLayout
//------------------------------------------------------------------------------

SchemaLayout::SchemaLayout(Schema *schema, QWidget* parent) : QGraphicsView(parent), _schema(schema)
{
    _axis = new OpticalAxisView;
    _axis->setZValue(1000);
    _scene.addItem(_axis);

    setRenderHint(QPainter::Antialiasing, true);
    setScene(&_scene);
}

SchemaLayout::~SchemaLayout()
{
    clear();
}

void SchemaLayout::populate()
{
    clear();
    for (Element *elem : _schema->elements())
    {
        if (elem->disabled()) continue;
        auto layout = _factory.makeLayout(elem);
        if (layout)
        {
            addElementView(layout->view());
            _elements.append(layout);
        }
    }
}

void SchemaLayout::addElementView(ElementView *elem)
{
    if (!_elems.isEmpty())
    {
        ElementView *last = _elems.last();
        elem->setPos(last->x() + last->halfw() + elem->halfw(), 0);
    }
    else
        elem->setPos(0, 0);

    _elems.append(elem);

    qreal fullWidth = 0;
    foreach (ElementView *elem, _elems)
        fullWidth += elem->width();

    _axis->setLength(fullWidth + 20);
    _axis->setX(_axis->halfw() - _elems.first()->halfw() - 10);

    _scene.addItem(elem);

    addLabelView(elem);
}

void SchemaLayout::addLabelView(ElementView* elem)
{
    QGraphicsTextItem *label = _scene.addText(elem->label());
    label->setZValue(1000 + _elems.count());
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
    _elems.clear();
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
//                              Element Layouts
//------------------------------------------------------------------------------

void ElemEmptyRangeLayout::init()
{
    _view = new ElementRangeView;
    _view->setLabel(_element->label());
}

//------------------------------------------------------------------------------

void ElemMediumRangeLayout::init()
{
    _view = new ElemMediumRangeView;
    _view->setLabel(_element->label());
}

//------------------------------------------------------------------------------

void ElemPlateLayout::init()
{
    _view = new ElemPlateView;
    _view->setLabel(_element->label());
}

//------------------------------------------------------------------------------

void ElemFlatMirrorLayout::init()
{
    ElemFlatMirrorView *view = new ElemFlatMirrorView;
    if (_element->owner())
    {
        int index = _element->owner()->indexOf(_element);
        if (index <= 0)
            view->setPlace(ElemFlatMirrorView::PlaceLeft);
        else if (index >= _element->owner()->count()-1)
            view->setPlace(ElemFlatMirrorView::PlaceRight);
        else
            view->setPlace(ElemFlatMirrorView::PlaceMiddle);
    }
    _view = view;
    _view->setLabel(_element->label());
}

//------------------------------------------------------------------------------

void ElemCurveMirrorLayout::init()
{
    CurvedElementView *view = new CurvedElementView;
    ElemCurveMirror *mirror = dynamic_cast<ElemCurveMirror*>(_element);
    if (mirror)
    {
        if (_element->owner())
        {
            int index = _element->owner()->indexOf(_element);
            if (index <= 0)
                view->setPaintMode((mirror->radius() > 0)
                                   ? CurvedElementView::PlaneConcaveMirror
                                   : CurvedElementView::PlaneConvexMirror);
            else if (index >= _element->owner()->count()-1)
                view->setPaintMode((mirror->radius() > 0)
                                   ? CurvedElementView::ConcavePlaneMirror
                                   : CurvedElementView::ConvexPlaneMirror);
            else
                view->setPaintMode((mirror->radius() > 0)
                                   ? CurvedElementView::ConvexLens
                                   : CurvedElementView::ConcaveLens);
        }
        if (mirror->alpha() > 0)
            view->setSlope(ElementView::SlopePlus);
        else if (mirror->alpha() < 0)
            view->setSlope(ElementView::SlopeMinus);
    }
    _view = view;
    _view->setLabel(_element->label());
}

//------------------------------------------------------------------------------

void ElemThinLensLayout::init()
{
    CurvedElementView *view = new CurvedElementView;
    ElemThinLens *lens = dynamic_cast<ElemThinLens*>(_element);
    if (lens)
    {
        view->setPaintMode((lens->focus() > 0)
                           ? CurvedElementView::ConvexLens
                           : CurvedElementView::ConcaveLens);
        if (lens->alpha() > 0)
            view->setSlope(ElementView::SlopePlus);
        else if (lens->alpha() < 0)
            view->setSlope(ElementView::SlopeMinus);
    }
    _view = view;
    _view->setLabel(_element->label());
}

//------------------------------------------------------------------------------

void ElemCylinderLensTLayout::init()
{
    CurvedElementView *view = new CurvedElementView;
    ElemCylinderLensT *lens = dynamic_cast<ElemCylinderLensT*>(_element);
    if (lens)
    {
        view->setMarkTS(CurvedElementView::MarkT);
        view->setPaintMode((lens->focus() > 0)
                           ? CurvedElementView::ConvexLens
                           : CurvedElementView::ConcaveLens);
        if (lens->alpha() > 0)
            view->setSlope(ElementView::SlopePlus);
        else if (lens->alpha() < 0)
            view->setSlope(ElementView::SlopeMinus);
    }
    _view = view;
    _view->setLabel(_element->label());
}

//------------------------------------------------------------------------------

void ElemCylinderLensSLayout::init()
{
    CurvedElementView *view = new CurvedElementView;
    ElemCylinderLensS *lens = dynamic_cast<ElemCylinderLensS*>(_element);
    if (lens)
    {
        view->setMarkTS(CurvedElementView::MarkS);
        view->setPaintMode((lens->focus() > 0)
                           ? CurvedElementView::ConvexLens
                           : CurvedElementView::ConcaveLens);
        if (lens->alpha() > 0)
            view->setSlope(ElementView::SlopePlus);
        else if (lens->alpha() < 0)
            view->setSlope(ElementView::SlopeMinus);
    }
    _view = view;
    _view->setLabel(_element->label());
}

//------------------------------------------------------------------------------

void ElemTiltedCrystalLayout::init()
{
    _view = new ElemTiltedCrystalView;
    ElemTiltedCrystal *crystal = dynamic_cast<ElemTiltedCrystal*>(_element);
    if (crystal)
    {
        if (crystal->alpha() > 0)
            _view->setSlope(ElementView::SlopePlus);
        else if (crystal->alpha() < 0)
            _view->setSlope(ElementView::SlopeMinus);
    }
    _view->setLabel(_element->label());
}

//------------------------------------------------------------------------------

void ElemTiltedPlateLayout::init()
{
    _view = new ElemTiltedPlateView;
    ElemTiltedPlate *plate = dynamic_cast<ElemTiltedPlate*>(_element);
    if (plate)
    {
        if (plate->alpha() > 0)
            _view->setSlope(ElementView::SlopePlus);
        else if (plate->alpha() < 0)
            _view->setSlope(ElementView::SlopeMinus);
    }
    _view->setLabel(_element->label());
}

////////////////////////////////////////////////////////////////////////////////

void ElemBrewsterCrystalLayout::init()
{
    _view = new ElemBrewsterCrystalView;
    _view->setLabel(_element->label());
}

////////////////////////////////////////////////////////////////////////////////

void ElemBrewsterPlateLayout::init()
{
    _view = new ElemBrewsterPlateView;
    _view->setLabel(_element->label());
}

////////////////////////////////////////////////////////////////////////////////

void ElemMatrixLayout::init()
{
    _view = new ElemMatrixView;
    _view->setLabel(_element->label());
}

////////////////////////////////////////////////////////////////////////////////

void ElemPointLayout::init()
{
    _view = new ElemPointView;
    _view->setLabel(_element->label());
}

////////////////////////////////////////////////////////////////////////////////
