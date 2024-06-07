#include "ElementFilter.h"
#include "Element.h"

ElementFilterCondition::~ElementFilterCondition()
{
}


ElementFilter::ElementFilter(std::initializer_list<ElementFilterCondition*> conditions): Filter(conditions)
{
}

namespace ElementFilters {

ElementFilterPtr elemsWithVisibleParams()
{
    static ElementFilterPtr filter(ElementFilter::make<ElementFilterHasVisibleParams, ElementFilterEnabled>());
    return filter;
}

ElementFilterPtr enabledElements()
{
    static ElementFilterPtr filter(ElementFilter::make<ElementFilterEnabled>());
    return filter;
}

} // namespace ElementFilters

bool ElementFilterEnabled::check(const Element *elem)
{
    return !elem->disabled();
}

bool ElementFilterHasParams::check(const Element *elem)
{
    return elem->hasParams();
}

bool ElementFilterHasVisibleParams::check(const Element *elem)
{
    int count = 0;
    for (Z::Parameter* param : elem->params())
        if (param->visible()) count++;
    return count > 0;
}

bool ElementFilterIsRange::check(const Element *elem)
{
    return Z::Utils::isRange(elem);
}
