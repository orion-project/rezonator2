#include "JsonUtils.h"

#include "../core/Schema.h"
#include "../core/Variable.h"
#include "../core/Report.h"

namespace Z {
namespace IO {
namespace Json {

Result<Unit> readUnit(const QJsonObject& json, Dim dim)
{
    auto unitStr = json["unit"].toString();
    Unit unit;
    if (dim && unitStr != UNIT(percent)->alias())
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

void writeUnit(QJsonObject& json, Unit unit)
{
    json["unit"] = unit->alias();
}

Result<Dim> readDim(const QJsonObject& json)
{
    auto dimStr = json["dim"].toString();
    auto dim = Dims::findByAlias(dimStr);
    if (!dim)
        return Result<Dim>::fail(QString("Unknown unit dimension: %1").arg(dimStr));
    return Result<Dim>::success(dim);
}

void writeDim(QJsonObject& json, Dim dim)
{
    json["dim"] = dim->alias();
}

QJsonValue doubleToJson(double v)
{
    if (qIsInf(v))
        return v < 0 ? QStringLiteral("-inf") : QStringLiteral("inf");
    return v;
}

double jsonToDouble(const QJsonValue &v)
{
    if (v.isDouble())
        return v.toDouble();
    if (v.isString()) {
        if (v.toString().compare(QStringLiteral("inf"), Qt::CaseInsensitive) == 0)
            return qInf();
        if (v.toString().compare(QStringLiteral("-inf"), Qt::CaseInsensitive) == 0)
            return -qInf();
    }
    return qQNaN();
}

QJsonObject writeValue(const Value& value)
{
    QJsonObject json({
        { "value", doubleToJson(value.value()) },
    });
    writeUnit(json, value.unit());
    return json;
}

Result<Value> readValue(const QJsonObject& json, Dim dim)
{
    auto value = jsonToDouble(json["value"]);
    auto unit = readUnit(json, dim);
    if (!unit.ok())
        return Result<Value>::fail(unit.error());
    return Result<Value>::success(Value(value, unit.value()));
}

QJsonObject writeValueTS(const ValueTS& value)
{
    QJsonObject json({
        { "value_t", doubleToJson(value.rawValueT()) },
        { "value_s", doubleToJson(value.rawValueS()) },
    });
    writeUnit(json, value.unit());
    return json;
}

Result<ValueTS> readValueTS(const QJsonObject& json, Dim dim)
{
    auto valueT = jsonToDouble(json["value_t"]);
    auto valueS = jsonToDouble(json["value_s"]);
    auto unit = readUnit(json, dim);
    if (!unit.ok())
        return Result<ValueTS>::fail(unit.error());
    return Result<ValueTS>::success(ValueTS(valueT, valueS, unit.value()));
}

QJsonObject writeParamSpec(Parameter *p)
{
    auto json = QJsonObject{
        { "alias", p->alias() },
        { "label", p->label() },
        { "name", p->name() },
        { "descr", p->description(), },
        { "custom", p->hasOption(ParamOption::Custom) },
    };
    writeDim(json, p->dim());
    return json;
}

Result<Parameter*> readParamSpec(const QJsonObject& json)
{
    auto dim = readDim(json);
    if (!dim.ok())
        return Result<Parameter*>::fail(dim.error());
    auto alias = json["alias"].toString();
    if (alias.isEmpty())
        return Result<Parameter*>::fail("Parameter alias is empty");
    auto label = json["label"].toString();
    if (label.isEmpty())
        label = alias;
    auto name = json["name"].toString();
    if (name.isEmpty())
        name = alias;
    auto descr = json["descr"].toString();
    auto param = new Parameter(dim.value(), alias, label, name, descr);
    if (json["custom"].toBool())
        param->setOption(ParamOption::Custom);
    return Result<Parameter*>::success(param);
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

QJsonObject writeVariable(const Variable *var, Schema *schema)
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
    Element* elem = elemIndex == -1 ? const_cast<Element*>(schema->globalParamsAsElem())
                                    : schema->element(elemIndex);
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

Result<Element*> readElemByIndex(const QJsonObject& json, const QString& key, Schema *schema)
{
    int elemIndex = json[key].toInt(-1);
    Element* elem = schema->element(elemIndex);
    if (!elem)
        return Result<Element*>::fail(QString("There is no element with index %1").arg(elemIndex));
    return Result<Element*>::success(elem);
}

//------------------------------------------------------------------------------
//                                 JsonValue
//------------------------------------------------------------------------------

JsonValue::JsonValue(const QJsonObject& root, const QString& key, Z::Report* report)
{
    _path = "/" + key;
    initObj(root, key);

    if (!_msg.isEmpty() && report)
        report->warning(_msg);
}

JsonValue::JsonValue(const JsonValue& root, const QString& key, Z::Report* report)
{
    _path = root._path % '/' % key;
    initObj(root.obj(), key);

    if (!_msg.isEmpty() && report)
        report->warning(_msg);
}

void JsonValue::initObj(const QJsonObject& root, const QString& key)
{
    if (!root.contains(key))
    {
        _msg = QString("Key not found: '%1'").arg(_path);
        return;
    }
    QJsonValue value = root[key];
    if (value.isNull() || value.isUndefined())
    {
        _msg = QString("Value is not set at '%1'").arg(_path);
        return;
    }
    if (value.isArray())
        _array = value.toArray();
    else if (value.isObject())
        _obj = value.toObject();
    else
        _msg = QString("Unsupported value type at '%1'").arg(_path);
}

} // namespace Json
} // namespace IO
} // namespace Z
