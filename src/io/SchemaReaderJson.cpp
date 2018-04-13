#include "SchemaReaderJson.h"

#include "z_io_utils.h"
#include "../core/Schema.h"
#include "../core/ElementsCatalog.h"

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

class JsonValue
{
public:
    JsonValue(const QJsonObject& root, const QString& key)
    {
        _path = key;
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
            _msg = "Key not found: " + _path;
        QJsonValue value = root[key];
        if (_msg.isEmpty() && (value.isNull() || value.isUndefined()))
            _msg = "Value is not set: " + _path;
        if (value.isArray())
            _array = value.toArray();
        else if (value.isObject())
            _obj = value.toObject();
        else
            _msg = "Unsupported value type: " + _path;
    }
};

} // namespace Json
} // namespace IO
} // namespace Z

using namespace Z::IO::Json;

SchemaReaderJson::SchemaReaderJson(Schema *schema) : _schema(schema)
{
}

void SchemaReaderJson::readFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return _report.error(file.errorString());
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
        return _report.error(error.errorString());

    QJsonObject root = doc.object();

    Ori::Version version(root["schema_version"].toString());
    if (version > Z::IO::Utils::currentVersion())
        return _report.error(qApp->translate("IO",
            "File version %1 is not supported, max supported version: %2")
                .arg(version.str(), Z::IO::Utils::currentVersion().str()));

    readGeneral(root);
    readGlobalParams(root);
    readPump(root);
    readElements(root);
    readStorables(root);
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
    WITH_JSON_VALUE(paramsJson, root, "named_params")
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

void SchemaReaderJson::readGlobalParams(const QJsonObject& root)
{
    WITH_JSON_VALUE(paramsJson, root, "global_params")
    {
        for (const QString& paramAlias : paramsJson.obj().keys())
        {
            WITH_JSON_VALUE(paramJson, paramsJson, paramAlias)
            {
                readGlobalParam(paramJson.obj(), paramAlias);
            }
        }
    }
}

void SchemaReaderJson::readGlobalParam(const QJsonObject& root, const QString &alias)
{
    auto dimStr = root["dim"].toString();
    Z::Dim dim = nullptr;
    for (auto d: Z::Dims::dims())
        if (d->alias() == dimStr)
        {
            dim = d;
            break;
        }
    if (!dim)
        return _report.warning(qApp->translate("IO",
            "Unknown dimension '%1' of global parameter '%2'").arg(dimStr, alias));

    auto param = new Z::Parameter(
        dim,
        alias,
        root["label"].toString(),
        root["name"].toString(),
        root["descr"].toString(),
        root["category"].toString(),
        root["visible"].toBool(true)
    );

    readParamValue(root, param);

    _schema->params()->append(param);
}

void SchemaReaderJson::readPump(const QJsonObject& root)
{
    // TODO
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

void SchemaReaderJson::readStorables(const QJsonObject& root)
{
    // TODO
}
