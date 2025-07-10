#include "Parameters.h"

namespace Z {

ParameterListener::~ParameterListener() {}

ParameterBase::~ParameterBase() {}

ParameterFilterCondition::~ParameterFilterCondition() {}

//------------------------------------------------------------------------------
//                               ParamValueBackup
//------------------------------------------------------------------------------

ParamValueBackup::ParamValueBackup(Parameter* param, const char *reason)
    : _param(param), _value(param->value()), _expr(param->expr()), _reason(reason)
{
    //qDebug() << "Backup param" << _param->alias() << _reason;
}

ParamValueBackup::~ParamValueBackup()
{
    //qDebug() << "Restore param" << _param->alias() << _reason;
    _param->setValue(_value);
    _param->setExpr(_expr);
}

} // namespace Z
