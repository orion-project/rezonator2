#ifndef CUSTOM_INFO_FUNCTION_H
#define CUSTOM_INFO_FUNCTION_H

#include "FunctionBase.h"

class PyRunner;

class CustomInfoFunction : public InfoFunction
{
public:
    CustomInfoFunction(Schema*, Element*);
    QString calculateInternal() override;
private:
    Element* _element;
};


#endif // CUSTOM_INFO_FUNCTION_H