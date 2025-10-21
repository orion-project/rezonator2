#include "Parameters.h"

#include <QRegularExpression>

namespace Z {

ParameterListener::~ParameterListener() {}

ParameterBase::~ParameterBase() {}

ParameterFilterCondition::~ParameterFilterCondition() {}

//------------------------------------------------------------------------------
//                               ParamValueBackup
//------------------------------------------------------------------------------

ParamValueBackup::ParamValueBackup(Parameter* param, const char *reason)
    : _param(param), _value(param->value()), _expr(param->expr()), _error(param->error()), _reason(reason)
{
    //qDebug() << "Backup param" << _param->alias() << _reason;
}

ParamValueBackup::~ParamValueBackup()
{
    //qDebug() << "Restore param" << _param->alias() << _reason;
    _param->setValue(_value);
    _param->setExpr(_expr);
    _param->setError(_error);
}

//------------------------------------------------------------------------------
//                               namespace Param
//------------------------------------------------------------------------------

namespace Param {

bool isValidAlias(const QString& s)
{
    static QRegularExpression r("^[a-zA-Z_][a-zA-Z_0-9]*$");
    return r.match(s).hasMatch();
}

} // namespace Param


} // namespace Z
