#include "SchemaWriterJson.h"

#include "z_io_utils.h"
#include "../core/Schema.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

namespace Z {
namespace IO {
namespace Json {

QJsonObject writeParamValue(Z::Parameter* param)
{
    return QJsonObject({
        { "value", param->value().value() },
        { "unit", param->value().unit()->alias() }
    });
}

} // namespace Json
} // namespace IO
} // namespace Z

using namespace Z::IO::Json;

SchemaWriterJson::SchemaWriterJson(Schema *schema) : _schema(schema)
{
}

QString SchemaWriterJson::writeToString()
{
    QJsonObject root;
    root["schema_version"] = Z::IO::Utils::currentVersion().str();

    writeGeneral(root);
    writePump(root);
    writeElements(root);
    writeWindows(root);

    QJsonDocument doc(root);
    return doc.toJson();
}

void SchemaWriterJson::writeToFile(const QString &fileName)
{
    QFile(fileName).write(writeToString().toUtf8());

    _schema->setFileName(fileName);
    _schema->events().raise(SchemaEvents::Saved);
}

void SchemaWriterJson::writeGeneral(QJsonObject& root)
{
    root["trip_type"] = TripTypes::info(_schema->tripType()).alias();

    root["named_params"] = QJsonObject({
        { "lambda", writeParamValue(&_schema->wavelength()) }
    });

    QJsonArray globalParams;
    for (Z::Parameter *p : *_schema->params())
        globalParams.append(QJsonObject({
            { "alias", p->alias() },
            { "label", p->label() },
            { "name", p->name() },
            { "descr", p->description() },
            { "category", p->category() },
            { "visible", p->visible() },
            { "dim", p->dim()->alias() },
            { "value", p->value().value() },
            { "unit", p->value().unit()->alias() },
        }));
    root["global_params"] = globalParams;
}

void SchemaWriterJson::writePump(QJsonObject &root)
{
    // TODO:NEXT-VER
}

void SchemaWriterJson::writeElements(QJsonObject& root)
{
    QJsonArray elemsJson;
    for (Element* elem: _schema->elements())
    {
        QJsonObject elemJson;
        writeElement(elemJson, elem);
        elemsJson.append(elemJson);
    }
    root["elements"] = elemsJson;
}

void SchemaWriterJson::writeElement(QJsonObject& root, Element *elem)
{
    root["type"] = elem->type();
    root["label"] = elem->label();
    root["title"] = elem->title();
    root["is_disabled"] = elem->disabled();

    QJsonObject paramsJson;
    for (Z::Parameter* p : elem->params())
        paramsJson[p->alias()] = writeParamValue(p);
    root["params"] = paramsJson;
}

void SchemaWriterJson::writeWindows(QJsonObject& root)
{

}

