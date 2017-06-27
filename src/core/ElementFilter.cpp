#include "ElementFilter.h"
#include "Element.h"

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
