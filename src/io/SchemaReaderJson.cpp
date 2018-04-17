#include "SchemaReaderJson.h"

#include "z_io_utils.h"
#include "ISchemaWindowStorable.h"
#include "../core/Schema.h"
#include "../core/ElementsCatalog.h"
#include "../WindowsManager.h"

#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

namespace Z {
namespace IO {
namespace Json {

QString readParamValue(const QJsonObject& json, Z::Parameter* param)
{
    auto value = json["value"].toDouble(param->value().value());
    auto unitStr = json["unit"].toString();
    auto unit = param->dim()->unitByAlias(unitStr);
    if (!unit)
        return qApp->translate("IO",
            "Unit '%1' is unacceptable for parameter '%2'").arg(unitStr, param->alias());
    param->setValue(Z::Value(value, unit));
    return QString();
}

/// Json value wrapper allowing to track value path inside file.
/// Path can be useful for logging when reading of some value fails.
class JsonValue
{
public:
    JsonValue(const QJsonObject& root, const QString& key)
    {
        _path = "/" + key;
        initObj(root, key);
    }

    JsonValue(const JsonValue& root, const QString& key)
    {
        _path = root._path % '/' % key;
        initObj(root.obj(), key);
    }

    QString msg() const { return _msg; }

    const QJsonObject& obj() const { return _obj; }
    const QJsonArray& array() const { return _array; }

private:
    QString _path;
    QString _msg;
    QJsonObject _obj;
    QJsonArray _array;

    void initObj(const QJsonObject& root, const QString& key)
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
};

} // namespace Json
} // namespace IO
} // namespace Z

using namespace Z::IO::Json;

//------------------------------------------------------------------------------
//                           SchemaReaderJson
//------------------------------------------------------------------------------

void SchemaReaderJson::readFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return _report.error(qApp->translate("IO",
            "Unable to open file for reading: %1").arg(file.errorString()));
    readFromUtf8(file.readAll());
    file.close();
}

void SchemaReaderJson::readFromString(const QString& text)
{
    readFromUtf8(text.toUtf8());
}

void SchemaReaderJson::readFromUtf8(const QByteArray& data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (doc.isNull())
        return _report.error(qApp->translate("IO",
            "Unable to parse file: %1").arg(error.errorString()));

    QJsonObject root = doc.object();

    Ori::Version version(root["schema_version"].toString());
    if (version > Z::IO::Utils::currentVersion())
        return _report.error(qApp->translate("IO",
            "File version %1 is not supported, max supported version: %2")
                .arg(version.str(), Z::IO::Utils::currentVersion().str()));

    readGeneral(root);
    readCustomParams(root);
    readPump(root);
    readElements(root);
    readParamLinks(root);
    readFormulas(root);
    readWindows(root);
}

#define WITH_JSON_VALUE(value, root, key)\
    JsonValue value(root, key);\
    if (!value.msg().isEmpty())\
        _report.warning(value.msg());\
    else

void SchemaReaderJson::readGeneral(const QJsonObject& root)
{
    // Read trip type
    bool ok;
    auto tripTypeStr = root["trip_type"].toString();
    auto tripType = TripTypes::find(tripTypeStr, &ok);
    if (!ok)
        _report.warning(qApp->translate("IO",
            "Unknown schema trip type: '%1'. %2 is ised instead.")
                .arg(tripTypeStr, TripTypes::info(tripType).alias()));
    _schema->setTripType(tripType);

    // Read named parameters
    WITH_JSON_VALUE(paramsJson, root, "builtin_params")
    {
        // Read lambda
        WITH_JSON_VALUE(lambdaJson, paramsJson, "lambda")
        {
            QString res = readParamValue(lambdaJson.obj(), &_schema->wavelength());
            if (!res.isEmpty())
                _report.warning(res);
        }
    }
}

void SchemaReaderJson::readCustomParams(const QJsonObject& root)
{
    WITH_JSON_VALUE(paramsJson, root, "custom_params")
    {
        for (const QString& paramAlias : paramsJson.obj().keys())
        {
            WITH_JSON_VALUE(paramJson, paramsJson, paramAlias)
            {
                readCustomParam(paramJson.obj(), paramAlias);
            }
        }
    }
}

void SchemaReaderJson::readCustomParam(const QJsonObject& root, const QString &alias)
{
    auto dimStr = root["dim"].toString();
    Z::Dim dim = Z::Dims::findByAlias(dimStr);
    if (!dim)
        return _report.warning(qApp->translate("IO",
            "Unknown dimension '%1' of custom parameter '%2'").arg(dimStr, alias));

    auto param = new Z::Parameter(
        dim,
        alias, // alias
        alias, // label
        alias, // name
        root["descr"].toString(),
        root["category"].toString(),
        true // true
    );

    readParamValue(root, param);

    _schema->params()->append(param);
}

void SchemaReaderJson::readPump(const QJsonObject& root)
{
    Q_UNUSED(root)

    // TODO:NEXT-VER
/*
    #define READ_PUMP_MODE(mode, param1, param2, param3)\
    {\
        auto node = nodePump.firstChildElement(#mode);\
        if (!node.isNull())\
        {\
            if (!_reader->readValueTS(node, #param1, pump.mode.param1) ||\
                !_reader->readValueTS(node, #param2, pump.mode.param2) ||\
                !_reader->readValueTS(node, #param3, pump.mode.param3))\
            {\
                report(Issue::BadPumpParams, #mode);\
                setDefaultPumpParams(_schema, pump, Z::PumpMode_## mode);\
            }\
        }\
        else setDefaultPumpParams(_schema, pump, Z::PumpMode_## mode);\
    }
*/
    // TODO:NEXT-VER
    //    if (_schema->tripType() != Schema::TripType::SP)
    //        disableReport(); // no messages for resonators

    //    BREAKABLE_BLOCK
    //    {
    //        auto nodePump = root.firstChildElement("pump");
    //        if (nodePump.isNull())
    //        {
    //            report(Issue::NoPump);
    //            // TODO
    //            //setDefaultPump(_schema);
    //            break;
    //        }

    //        bool ok;
    //        auto modeName = nodePump.attribute("mode");
    //        auto mode = ENUM_ITEM_BY_NAME(Z::Pump::PumpMode, modeName, &ok);
    //        if (!ok)
    //        {
    //            report(Issue::UnknownPumpMode, modeName);
    //            // TODO
    //            //setDefaultPump(_schema);
    //            break;
    //        }

    //        /* TODO
    //        Z::Pump::Params pump;
    //        pump.mode = mode;
    //        READ_PUMP_MODE(waist, radius, distance, mi)
    //        READ_PUMP_MODE(front, radius, curvature, mi)
    //        READ_PUMP_MODE(complex, re, im, mi)
    //        READ_PUMP_MODE(icomplex, re, im, mi)
    //        READ_PUMP_MODE(vector, radius, angle, distance)
    //        READ_PUMP_MODE(sections, radius_1, radius_2, distance)
    //        _schema->setPump(pump);*/
    //    }
    //    enableReport();
}

void SchemaReaderJson::readElements(const QJsonObject& root)
{
    WITH_JSON_VALUE(elemsJson, root, "elements")
        for (auto it = elemsJson.array().begin(); it != elemsJson.array().end(); it++)
            readElement((*it).toObject());
}

void SchemaReaderJson::readElement(const QJsonObject& root)
{
    auto elemType = root["type"].toString();
    auto elem = ElementsCatalog::instance().create(elemType);
    if (!elem)
        return _report.warning(qApp->translate("IO",
            "Unknown element type '%1', element skipped").arg(elemType));

    _schema->insertElement(elem, -1, false);

    ElementLocker lock(elem);

    elem->setLabel(root["label"].toString());
    elem->setTitle(root["title"].toString());
    elem->setDisabled(root["is_disabled"].toBool());

    WITH_JSON_VALUE(paramsJson, root, "params")
    {
        for (Z::Parameter *param : elem->params())
        {
            WITH_JSON_VALUE(paramJson, paramsJson, param->alias())
            {
                readParamValue(paramJson.obj(), param);
            }
        }
    }

    // TODO: read misalignments
}

void SchemaReaderJson::readParamLinks(const QJsonObject& root)
{
    WITH_JSON_VALUE(linksJson, root, "param_links")
        for (auto it = linksJson.array().begin(); it != linksJson.array().end(); it++)
            readParamLink((*it).toObject());
}

void SchemaReaderJson::readParamLink(const QJsonObject& root)
{
    auto targetElemIndex = root["target_elem"].toInt();
    auto targetElem = _schema->element(targetElemIndex);
    if (!targetElem)
        return _report.warning(qApp->translate("IO",
            "Unable to load link: element with index %1 not found").arg(targetElemIndex));

    auto targetParamAlias = root["target_param"].toString();
    auto targetParam = targetElem->params().byAlias(targetParamAlias);
    if (!targetParam)
        return _report.warning(qApp->translate("IO",
            "Unable to load link: parameter '%1' not found in elemenet #%2")
                .arg(targetParamAlias).arg(targetElem->displayLabel()));

    auto sourceParamAlias = root["source_param"].toString();
    auto sourceParam = _schema->params()->byAlias(sourceParamAlias);
    if (!sourceParam)
        return _report.warning(qApp->translate("IO",
            "Unable to load link: parameter '%1' not found in custom params").arg(sourceParamAlias));

    _schema->paramLinks()->append(new Z::ParamLink(sourceParam, targetParam));
}

void SchemaReaderJson::readFormulas(const QJsonObject& root)
{
    WITH_JSON_VALUE(formulasJson, root, "formulas")
        for (auto it = formulasJson.array().begin(); it != formulasJson.array().end(); it++)
            readFormula((*it).toObject());
}

void SchemaReaderJson::readFormula(const QJsonObject& root)
{
    auto targetAlias = root["target_param"].toString();
    auto targetParam = _schema->params()->byAlias(targetAlias);
    if (!targetParam)
        return _report.warning(qApp->translate("IO",
            "Unable to find target parameter '%1' for formula, formula skipped").arg(targetAlias));

    auto formula = new Z::Formula(targetParam);
    formula->setCode(root["code"].toString());

    WITH_JSON_VALUE(depsJson, root, "param_deps")
        for (auto it = depsJson.array().begin(); it != depsJson.array().end(); it++)
        {
            auto depAlias = (*it).toString();
            auto depParam = _schema->params()->byAlias(depAlias);
            if (!depParam)
            {
                _report.warning(qApp->translate("IO",
                    "Unable to find parameter '%1' required by formula driving parameter '%2'")
                        .arg(depAlias, targetAlias));
                continue;
            }
            formula->addDep(depParam);
        }

    _schema->formulas()->put(formula);
}

void SchemaReaderJson::readWindows(const QJsonObject& root)
{
    WITH_JSON_VALUE(windowsJson, root, "windows")
        for (auto it = windowsJson.array().begin(); it != windowsJson.array().end(); it++)
            readWindow((*it).toObject());
}

void SchemaReaderJson::readWindow(const QJsonObject& root)
{
    auto type = root["type"].toString();
    auto ctor = WindowsManager::getConstructor(type);
    if (!ctor)
        return _report.warning(qApp->translate("IO",
            "Unable to load window of unknown type '%1', skipped.").arg(type));

    SchemaWindow* window = ctor(_schema);
    ISchemaWindowStorable* storable = dynamic_cast<ISchemaWindowStorable*>(window);
    if (!storable)
        return _report.warning(qApp->translate("IO",
            "Window of type '%1' is stored in file but it is not known how to load it, skipped.").arg(type));

    QString res = storable->storableRead(root);
    if (res.isEmpty())
        WindowsManager::instance().show(window);
    else
    {
        _report.warning(qApp->translate("IO",
            "Unable to load window of type '%1': %2").arg(type, res));
        delete window;
    }
}
