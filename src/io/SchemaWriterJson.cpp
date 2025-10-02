#include "SchemaWriterJson.h"

#include "../core/Schema.h"
#include "../core/ElementFormula.h"
#include "../io/JsonUtils.h"
#include "../io/ISchemaWindowStorable.h"
#include "../windows/WindowsManager.h"

#include <QApplication>
#include <QBuffer>
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
    root[JSON_KEY_VERSION] = Z::IO::Json::currentVersion().str();

    writeGeneral(root);
    writeGlobalParams(root);
    writePumps(root, *_schema->pumps());
    writeElements(root, _schema->elements());
    writeParamLinks(root);
    writeFormulas(root);
    writeWindows(root);
    writeMemos(root);

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

void SchemaWriterJson::writeGlobalParams(QJsonObject& root)
{
    QJsonObject customParams;
    foreach (Z::Parameter *p, *_schema->globalParams())
        customParams[p->alias()] = QJsonObject({
            { "descr", p->description() },
            { "dim", p->dim()->alias() },
            { "value", p->value().value() },
            { "unit", p->value().unit()->alias() },
            { "expr", p->expr() },
            { "error", p->error() },
        });
    root["custom_params"] = customParams;
}

void SchemaWriterJson::writeParamLinks(QJsonObject& root)
{
    QJsonArray linksJson;
    foreach (const Z::ParamLink *link, *_schema->paramLinks())
    {
        if (link->hasOption(Z::ParamLink_NonStorable)) continue;

        auto elems = _schema->elements();
        for (int i = 0; i < elems.size(); i++)
        {
            bool saved = false;
            for (const Z::Parameter *targetParam: elems.at(i)->params())
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
    const auto&  formulas = _schema->formulas()->items();
    auto it = formulas.constBegin();
    while (it != formulas.constEnd())
    {
        auto formula = it.value();
        QJsonObject formulaJson;
        formulaJson["target_param"] = formula->target()->alias();
        formulaJson["code"] = formula->code();
        QJsonArray depsJson;
        for (auto dep : formula->deps())
            depsJson.append(dep->alias());
        formulaJson["param_deps"] = depsJson;
        formulasJson.append(formulaJson);
        it++;
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

void SchemaWriterJson::writeMemos(QJsonObject& root)
{
    if (!_schema->memo || _schema->memo->text.isEmpty())
        return;

    QJsonObject imagesIson;
    auto it = _schema->memo->images.constBegin();
    while (it != _schema->memo->images.constEnd())
    {
        QBuffer b;
        it.value().save(&b, "png");
        imagesIson[it.key()] = QString::fromLatin1(b.buffer().toBase64());
        it++;
    }

    QJsonArray memosJson;
    QJsonObject memoJson;
    memoJson["content"] = QString::fromLatin1(_schema->memo->text.toUtf8().toBase64());
    memoJson["images"] = imagesIson;
    memosJson.append(memoJson);

    root["memos"] = memosJson;
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
    root["layout_show_label"] = elem->layoutOptions.showLabel;
    root["layout_draw_alt"] = elem->layoutOptions.drawAlt;
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
        paramJson["expr"] = p->expr();
        paramJson["error"] = p->error();
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
    auto mode = Pumps::findByModeName(pump->modeName());
    if (!mode)
    {
        qCritical() << "SchemaWriterJson::writePump(): Unable to find mode for pump parameters";
        return;
    }
    root["mode"] = mode->modeName();
    root["label"] = pump->label();
    root["title"] = pump->title();
    root["color"] = pump->color();
    root["is_active"] = pump->isActive();
    QJsonObject paramsJson;
    const auto pumpParams = *pump->params();
    for (Z::ParameterTS* p : std::as_const(pumpParams))
        paramsJson[p->alias()] = writeValueTS(p->value());
    root["params"] = paramsJson;
}

} // namespace Json
} // namespace IO
} // namespace Z
