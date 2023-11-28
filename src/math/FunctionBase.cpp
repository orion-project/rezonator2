#include "FunctionBase.h"

#include "../core/Schema.h"
#include "../core/Protocol.h"

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

void FunctionBase::setError(const QString& error)
{
    if (!error.isEmpty()) Z_ERROR(QString("%1: %2").arg(name(), error))
    _errorText = error;
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
