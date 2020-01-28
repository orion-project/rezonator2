#include "SchemaWriterJson.h"

#include "CommonUtils.h"
#include "JsonUtils.h"
#include "ISchemaWindowStorable.h"
#include "../core/Schema.h"
#include "../core/ElementFormula.h"
#include "../WindowsManager.h"

#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
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
    writePumps(root, *_schema->pumps());
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

        QString windowType = storable->storableType();
        Z::Report windowReport;
        QJsonObject windowJson;
        windowJson["type"] = windowType;
        if (storable->storableWrite(windowJson, &windowReport))
        {
            windowsJson.append(windowJson);
            if (!windowReport.isEmpty())
            {
                _report.info(QString("There are messages while saving window of type '%1'").arg(windowType));
                _report.report(windowReport);
            }
        }
        else
        {
            _report.warning(QString("Unable to save window of type '%1'").arg(windowType));
            _report.report(windowReport);
        }
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
    auto formulaElem = dynamic_cast<ElemFormula*>(elem);

    root["type"] = elem->type();
    root["label"] = elem->label();
    root["title"] = elem->title();
    root["is_disabled"] = elem->disabled();
    if (formulaElem)
    {
        root["has_matrices_ts"] = formulaElem->hasMatricesTS();
        root["formula"] = formulaElem->formula();
    }

    QJsonObject paramsJson;
    for (int i = 0; i < elem->params().size(); i++)
    {
        Z::Parameter* p = elem->params().at(i);
        auto paramJson = writeValue(p->value());
        if (formulaElem)
        {
            paramJson["dim"] = p->dim()->alias();
            paramJson["descr"] = p->description();
            paramJson["order"] = i;
        }
        paramsJson[p->alias()] = paramJson;
    }
    root["params"] = paramsJson;
}

void writePumps(QJsonObject& root, const QList<PumpParams*>& pumps)
{
    QJsonArray pumpsJson;
    for (auto pump: pumps)
    {
        QJsonObject pumpJson;
        writePump(pumpJson, pump);
        pumpsJson.append(pumpJson);
    }
    root["pumps"] = pumpsJson;
}

void writePump(QJsonObject &root, PumpParams *pump)
{
    auto mode = Pump::findByModeName(pump->modeName());
    if (!mode)
    {
        qCritical() << "SchemaWriterJson::writePump(): Unable to find mode for pump parameters";
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

} // namespace Json
} // namespace IO
} // namespace Z
