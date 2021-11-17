#include "ElementFilter.h"
#include "Element.h"

ElementFilterCondition::~ElementFilterCondition()
{
}


ElementFilter::ElementFilter(std::initializer_list<ElementFilterCondition*> conditions): Filter(conditions)
{
}

ElementFilter* ElementFilter::elemsWithVisibleParams()
{
    static ElementFilter* filter = ElementFilter::make<ElementFilterHasVisibleParams, ElementFilterEnabled>();
    return filter;
}

ElementFilter* ElementFilter::enabledElements()
{
    static ElementFilter* filter = ElementFilter::make<ElementFilterEnabled>();
    return filter;
}


bool ElementFilterEnabled::check(Element *elem)
{
    return !elem->disabled();
}

bool ElementFilterHasParams::check(Element *elem)
{
    return elem->hasParams();
}

bool ElementFilterHasVisibleParams::check(Element *elem)
{
    int count = 0;
    for (Z::Parameter* param : elem->params())
        if (param->visible()) count++;
    return count > 0;
}

bool ElementFilterIsRange::check(Element *elem)
{
    return Z::Utils::isRange(elem);
}
