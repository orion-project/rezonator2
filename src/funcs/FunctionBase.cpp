#include "FunctionBase.h"
#include "../core/Schema.h"

//------------------------------------------------------------------------------
//                               FunctionListener
//------------------------------------------------------------------------------

FunctionListener::~FunctionListener()
{
}

//------------------------------------------------------------------------------
//                                 FunctionBase
//------------------------------------------------------------------------------

FunctionBase::~FunctionBase()
{
    NOTIFY_LISTENERS_1(functionDeleted, this);
}

//------------------------------------------------------------------------------
//                                 InfoFunction
//------------------------------------------------------------------------------

FunctionBase::FunctionState InfoFunction::elementDeleting(Element*)
{
    return _schema->count() == 1? Dead: Ok; // last element is deleting
}

void InfoFunction::calculate()
{
    if (_frozen)
    {
        _needRecalc = true;
        return;
    }

    _result = calculateInternal();
    _needRecalc = false;
    NOTIFY_LISTENERS_1(functionCalculated, this);
}

void InfoFunction::freeze(bool on)
{
    _frozen = on;

    if (!_frozen && _needRecalc)
        calculate();
}
