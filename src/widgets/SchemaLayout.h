#ifndef SCHEMALAYOUT_H
#define SCHEMALAYOUT_H

#include "LayoutView.h"
#include "../core/Schema.h"
#include "../core/Elements.h"

////////////////////////////////////////////////////////////////////////////////

class ElementLayout
{
public:
    ElementLayout(Element* elem): _element(elem) { }
    virtual ~ElementLayout();
    Element *element() const { return _element; }
    virtual int viewCount() { return 1; }
    virtual ElementView* view(int) { return _view; }
    virtual void init() = 0;

protected:
    Element* _element;
    ElementView* _view;
};

////////////////////////////////////////////////////////////////////////////////

class ElementLayoutMaker
{
public:
    virtual ~ElementLayoutMaker();
    virtual ElementLayout *make(Element*) = 0;
};

class ElementLayoutFactory
{
public:
    ElementLayoutFactory();
    ~ElementLayoutFactory();
    ElementLayout* makeLayout(Element*) const;
private:
    QMap<QString, ElementLayoutMaker*> _makers;
};

////////////////////////////////////////////////////////////////////////////////

class SchemaLayout :
        public LayoutView,
        public SchemaListener
{
public:
    SchemaLayout(Schema *schema);
    ~SchemaLayout();

    ///// inherits from SchemaListener
    void schemaLoaded(Schema*) { populate(); }
    void elementCreated(Schema*, Element*) { populate(); }
    void elementChanged(Schema*, Element*) { populate(); }
    void elementDeleted(Schema*, Element*) { populate(); }

private:
    Schema *_schema;
    QVector<ElementLayout*> _elements;
    ElementLayoutFactory _factory;

    void populate();
    void clear();
};

////////////////////////////////////////////////////////////////////////////////

#define DECLARE_ELEMENT_LAYOUT(elem_type)                                       \
    class elem_type ## Layout : public ElementLayout                            \
    {                                                                           \
    public:                                                                     \
        elem_type ## Layout(Element *elem) : ElementLayout(elem) { init(); }    \
        void init() override;

#define DECLARE_ELEMENT_LAYOUT_END(elem_type)  };                               \
    class elem_type ## LayoutMaker : public ElementLayoutMaker                  \
    {                                                                           \
    public:                                                                     \
        ElementLayout* make(Element* elem) override                             \
        {                                                                       \
            return new elem_type ## Layout(elem);                               \
        }                                                                       \
    };                                                                          \

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

#endif // SCHEMALAYOUT_H
