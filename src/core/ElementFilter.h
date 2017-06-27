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

typedef Ori::Filter<Element*, ElementFilterCondition> ElementFilter;

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
