#include "FunctionBase.h"
#include "../core/Schema.h"

//------------------------------------------------------------------------------
//                                 FunctionBase
//------------------------------------------------------------------------------
FunctionBase::~FunctionBase()
{

}

//------------------------------------------------------------------------------
//                                 InfoFunction
//------------------------------------------------------------------------------
FunctionBase::FunctionState InfoFunction::elementDeleting(Element*)
{
    return _schema->count() == 1? Dead: Ok; // last element is deleting
}


