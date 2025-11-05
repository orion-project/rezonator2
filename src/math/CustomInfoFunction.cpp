#include "CustomInfoFunction.h"

CustomInfoFunction::CustomInfoFunction(Schema *schema, Element *elem)
    : InfoFunction(schema), _element(elem) {}
    
QString CustomInfoFunction::calculateInternal()
{
    return {};
}
