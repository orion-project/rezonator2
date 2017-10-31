#include "Calculator.h"
#include "FunctionBase.h"
#include "../core/Schema.h"
#include "../core/Protocol.h"

#include <QApplication>

//------------------------------------------------------------------------------
//                                 FunctionBase
//------------------------------------------------------------------------------

FunctionBase::FunctionState InfoFunction::elementDeleting(Element*)
{
    return _schema->count() == 1? Dead: Ok; // last element is deleting
}


