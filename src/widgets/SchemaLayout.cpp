#include "SchemaLayout.h"

ElementLayout::~ElementLayout()
{
}

ElementLayoutMaker::~ElementLayoutMaker()
{
}

////////////////////////////////////////////////////////////////////////////////
//                             ElementLayoutMakers
////////////////////////////////////////////////////////////////////////////////

#define ELEMENT_LAYOUT_MAKER(elem_type)                             \
{                                                                   \
    elem_type elem;                                                 \
    _makers[elem.type()] = new (elem_type ## LayoutMaker);    \
}

ElementLayoutFactory::ElementLayoutFactory()
{
    ELEMENT_LAYOUT_MAKER(ElemEmptyRange);
    ELEMENT_LAYOUT_MAKER(ElemMediumRange);
    ELEMENT_LAYOUT_MAKER(ElemPlate);
    ELEMENT_LAYOUT_MAKER(ElemFlatMirror);
    ELEMENT_LAYOUT_MAKER(ElemCurveMirror);
    ELEMENT_LAYOUT_MAKER(ElemThinLens);
    ELEMENT_LAYOUT_MAKER(ElemCylinderLensT);
    ELEMENT_LAYOUT_MAKER(ElemCylinderLensS);
    ELEMENT_LAYOUT_MAKER(ElemTiltedCrystal);
    ELEMENT_LAYOUT_MAKER(ElemTiltedPlate);
    ELEMENT_LAYOUT_MAKER(ElemBrewsterCrystal);
    ELEMENT_LAYOUT_MAKER(ElemBrewsterPlate);
    ELEMENT_LAYOUT_MAKER(ElemMatrix);
    ELEMENT_LAYOUT_MAKER(ElemPoint);
}

ElementLayoutFactory::~ElementLayoutFactory()
{
    for (auto maker : _makers.values()) delete maker;
}

ElementLayout* ElementLayoutFactory::makeLayout(Element *elem) const
{
    return _makers.contains(elem->type()) ? _makers[elem->type()]->make(elem):  nullptr;
}

////////////////////////////////////////////////////////////////////////////////
//                               SchemaLayout
////////////////////////////////////////////////////////////////////////////////

SchemaLayout::SchemaLayout(Schema *schema) : _schema(schema)
{
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
            for (int j = 0; j < layout->viewCount(); j++)
                this->add(layout->view(j));
            _elements.append(layout);
        }
    }
}

void SchemaLayout::clear()
{
    LayoutView::clear();
    for (auto elem : _elements) delete elem;
    _elements.clear();
}

////////////////////////////////////////////////////////////////////////////////
//                              Element Layouts
////////////////////////////////////////////////////////////////////////////////

void ElemEmptyRangeLayout::init()
{
    _view = new ElementRangeView;
    _view->setLabel(_element->label());
}

////////////////////////////////////////////////////////////////////////////////

void ElemMediumRangeLayout::init()
{
    _view = new ElemMediumRangeView;
    _view->setLabel(_element->label());
}

////////////////////////////////////////////////////////////////////////////////

void ElemPlateLayout::init()
{
    _view = new ElemPlateView;
    _view->setLabel(_element->label());
}

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

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
