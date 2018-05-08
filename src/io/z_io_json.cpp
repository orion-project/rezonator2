#include "z_io_json.h"

#include "../core/Schema.h"
#include "../core/Variable.h"

namespace Z {
namespace IO {
namespace Json {

Result<Unit> readUnit(const QJsonObject& json, Dim dim)
{
    auto unitStr = json["unit"].toString();
    Unit unit;
    if (dim)
    {
        unit = dim->unitByAlias(unitStr);
        if (!unit)
            return Result<Unit>::fail(QString(
                "Unit '%1' is unacceptable for dimension '%2'").arg(unitStr, dim->alias()));
    }
    else
    {
        unit = Units::findByAlias(unitStr);
        if (!unit)
            return Result<Unit>::fail(QString(
                "Unknown unit of measurements: %1").arg(unitStr));
    }
    return Result<Unit>::success(unit);
}

QJsonObject writeValue(const Value& value)
{
    return QJsonObject({
        { "value", value.value() },
        { "unit", value.unit()->alias() }
    });
}

Result<Value> readValue(const QJsonObject& json, Dim dim)
{
    auto value = json["value"].toDouble();
    auto unit = readUnit(json, dim);
    if (!unit.ok())
        return Result<Value>::fail(unit.error());
    return Result<Value>::success(Value(value, unit.value()));
}

QJsonObject writeValueTS(const ValueTS& value)
{
    return QJsonObject({
        { "value_t", value.rawValueT() },
        { "value_s", value.rawValueS() },
        { "unit", value.unit()->alias() }
    });
}

Result<ValueTS> readValueTS(const QJsonObject& json, Dim dim)
{
    auto valueT = json["value_t"].toDouble();
    auto valueS = json["value_s"].toDouble();
    auto unit = readUnit(json, dim);
    if (!unit.ok())
        return Result<ValueTS>::fail(unit.error());
    return Result<ValueTS>::success(ValueTS(valueT, valueS, unit.value()));
}

QJsonObject writeVariableRange(const VariableRange& range)
{
    return QJsonObject({
        { "start", writeValue(range.start) },
        { "stop", writeValue(range.stop) },
        { "step", writeValue(range.step) },
        { "points", range.points },
        { "use_step", range.useStep },
    });
}

QString readVariableRange(const QJsonObject& json, VariableRange& range, Dim dim)
{
    auto resStart = readValue(json["start"].toObject(), dim);
    if (!resStart.ok()) return QString("Invalid start value: %1").arg(resStart.error());
    auto resStop = readValue(json["stop"].toObject(), dim);
    if (!resStop.ok()) return QString("Invalid stop value: %1").arg(resStop.error());
    auto resStep = readValue(json["step"].toObject(), dim);
    if (!resStep.ok()) return QString("Invalid step value: %1").arg(resStep.error());
    range.start = resStart.value();
    range.stop = resStop.value();
    range.step = resStep.value();
    range.points = json["points"].toInt(100);
    range.useStep = json["use_step"].toBool();
    return QString();
}

QJsonObject writeVariable(Variable *var, Schema *schema)
{
    return QJsonObject({
        { "element_index", schema->indexOf(var->element) },
        { "param", var->parameter->alias() },
        { "range", writeVariableRange(var->range) }
    });
}

QString readVariable(const QJsonObject& json, Variable* var, Schema *schema)
{
    int elemIndex = json["element_index"].toInt(-1);
    Element* elem = schema->element(elemIndex);
    if (!elem)
        return QString("There is no element with index %1").arg(elemIndex);
    auto paramAlias = json["param"].toString();
    auto param = elem->params().byAlias(paramAlias);
    if (!param)
        return QString("Element with index %1 has no parameter '%2'")
                .arg(elemIndex).arg(elem->displayLabel(), paramAlias);
    VariableRange range;
    auto res = readVariableRange(json["range"].toObject(), range);
    if (!res.isEmpty())
        return QString("Unable to load variable range: %1").arg(res);
    var->element = elem;
    var->parameter = param;
    var->range = range;
    return QString();
}

QJsonObject writeVariablePref(Variable *var)
{
    return QJsonObject({
        { "element_label", var->element->label() },
        { "param", var->parameter->alias() },
        { "range", writeVariableRange(var->range) }
    });
}

void readVariablePref(const QJsonObject& json, Variable* var, Schema *schema)
{
    var->element = schema->elementByLabel(json["element_label"].toString());
    if (var->element)
        var->parameter = var->element->params().byAlias(json["param"].toString());
    // TODO extract validation logic from variable editor dialog into helper method and use it here
    // to verify that restored range is valid and suitable for restored element and its parameter.
    readVariableRange(json["range"].toObject(), var->range);
}

} // namespace Json
} // namespace IO
} // namespace Z
