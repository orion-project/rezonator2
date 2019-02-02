#include "SchemaWriterJson.h"

#include "CommonUtils.h"
#include "JsonUtils.h"
#include "ISchemaWindowStorable.h"
#include "../core/Schema.h"
#include "../WindowsManager.h"

#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

using namespace Z::IO::Json;

//------------------------------------------------------------------------------
//                           SchemaWriterJson
//------------------------------------------------------------------------------

void SchemaWriterJson::writeToFile(const QString &fileName)
{
    auto text = writeToString();
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return _report.error(qApp->translate("IO",
            "Unable to open file for writing: %1").arg(file.errorString()));
    QTextStream stream(&file);
    stream << text;
    file.close();
}

QString SchemaWriterJson::writeToString()
{
    QJsonObject root;
    root["schema_version"] = Z::IO::Utils::currentVersion().str();

    writeGeneral(root);
    writeCustomParams(root);
    writePumps(root);
    writeElements(root, _schema->elements());
    writeParamLinks(root);
    writeFormulas(root);
    writeWindows(root);

    QJsonDocument doc(root);
    return doc.toJson();
}

void SchemaWriterJson::writeGeneral(QJsonObject& root)
{
    root["title"] = _schema->title();
    root["notes"] = _schema->notes();
    root["trip_type"] = TripTypes::info(_schema->tripType()).alias();

    root["builtin_params"] = QJsonObject({
        { "lambda", writeValue(_schema->wavelength().value()) }
    });
}

void SchemaWriterJson::writeCustomParams(QJsonObject& root)
{
    QJsonObject customParams;
    for (Z::Parameter *p : *_schema->customParams())
        customParams[p->alias()] = QJsonObject({
            { "descr", p->description() },
            { "dim", p->dim()->alias() },
            { "value", p->value().value() },
            { "unit", p->value().unit()->alias() },
        });
    root["custom_params"] = customParams;
}

void SchemaWriterJson::writePumps(QJsonObject &root)
{
    QJsonArray pumpsJson;
    for (Z::PumpParams* pump: *_schema->pumps())
    {
        QJsonObject pumpJson;
        writePump(pumpJson, pump);
        pumpsJson.append(pumpJson);
    }
    root["pumps"] = pumpsJson;
}

void SchemaWriterJson::writePump(QJsonObject &root, Z::PumpParams *pump)
{
    Z::PumpMode *mode = Z::Pump::findByModeName(pump->modeName());
    if (!mode)
    {
        qCritical() << "Unable to find mode for pump parameters";
        return;
    }
    root["mode"] = mode->modeName();
    root["label"] = pump->label();
    root["title"] = pump->title();
    root["is_active"] = pump->isActive();
    QJsonObject paramsJson;
    for (Z::ParameterTS* p : *pump->params())
        paramsJson[p->alias()] = writeValueTS(p->value());
    root["params"] = paramsJson;
}

void SchemaWriterJson::writeParamLinks(QJsonObject& root)
{
    QJsonArray linksJson;
    for (const Z::ParamLink *link : *_schema->paramLinks())
    {
        if (link->hasOption(Z::ParamLink_NonStorable)) continue;

        for (int i = 0; i < _schema->elements().size(); i++)
        {
            bool saved = false;
            for (const Z::Parameter *targetParam: _schema->element(i)->params())
                if (targetParam == link->target())
                {
                    linksJson.append(QJsonObject({
                        { "target_elem", i },
                        { "target_param", targetParam->alias() },
                        { "source_param", link->source()->alias() },
                    }));
                    saved = true;
                    break;
                }
            if (saved) break;
        }
    }
    root["param_links"] = linksJson;
}

void SchemaWriterJson::writeFormulas(QJsonObject& root)
{
    QJsonArray formulasJson;
    for (Z::Formula *formula : _schema->formulas()->items().values())
    {
        QJsonObject formulaJson;
        formulaJson["target_param"] = formula->target()->alias();
        formulaJson["code"] = formula->code();
        QJsonArray depsJson;
        for (auto dep : formula->deps())
            depsJson.append(dep->alias());
        formulaJson["param_deps"] = depsJson;
        formulasJson.append(formulaJson);
    }
    root["formulas"] = formulasJson;
}

void SchemaWriterJson::writeWindows(QJsonObject& root)
{
    QJsonArray windowsJson;
    auto windows = WindowsManager::instance().schemaWindows(_schema);
    for (auto window : windows)
    {
        auto storable = dynamic_cast<ISchemaWindowStorable*>(window);
        if (!storable) continue;

        QJsonObject windowJson;
        windowJson["type"] = storable->storableType();
        QString res = storable->storableWrite(windowJson);
        if (res.isEmpty())
            windowsJson.append(windowJson);
        else
            _report.warning(qApp->translate("IO",
                "Unable to save window of type '%1': %2").arg(storable->storableType(), res));
    }
    root["windows"] = windowsJson;
}

namespace Z {
namespace IO {
namespace Json {

void writeElements(QJsonObject& root, const QList<Element*>& elements)
{
    QJsonArray elemsJson;
    for (Element* elem: elements)
    {
        QJsonObject elemJson;
        writeElement(elemJson, elem);
        elemsJson.append(elemJson);
    }
    root["elements"] = elemsJson;
}

void writeElement(QJsonObject& root, Element *elem)
{
    root["type"] = elem->type();
    root["label"] = elem->label();
    root["title"] = elem->title();
    root["is_disabled"] = elem->disabled();

    QJsonObject paramsJson;
    for (Z::Parameter* p : elem->params())
        paramsJson[p->alias()] = writeValue(p->value());
    root["params"] = paramsJson;
}

} // namespace Json
} // namespace IO
} // namespace Z
