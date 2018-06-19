#ifndef ELEMENT_FILTER_H
#define ELEMENT_FILTER_H

#include "core/OriFilter.h"

class Element;

class ElementFilterCondition
{
public:
    virtual ~ElementFilterCondition() {}
    virtual bool check(Element*) = 0;
};

class ElementFilter : public Ori::Filter<Element*, ElementFilterCondition>
{
public:
    ElementFilter(std::initializer_list<ElementFilterCondition*> conditions): Filter(conditions) {}

    template <typename ...TCondition>
    static ElementFilter* make()
    {
        return new ElementFilter({ new TCondition... });
    }
};

#define DECLARE_ELEM_FILTER(class_name)\
    class class_name: public ElementFilterCondition\
    {\
    public:\
        virtual bool check(Element* elem) override;\
    };

DECLARE_ELEM_FILTER(ElementFilterEnabled)
DECLARE_ELEM_FILTER(ElementFilterHasParams)
DECLARE_ELEM_FILTER(ElementFilterHasVisibleParams)
DECLARE_ELEM_FILTER(ElementFilterIsRange)

#endif // ELEMENT_FILTER_H
