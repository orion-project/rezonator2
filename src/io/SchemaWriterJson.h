#ifndef SCHEMA_WRITER_JSON_H
#define SCHEMA_WRITER_JSON_H

#include "../core/Report.h"

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

class Element;
class ElemFormula;
class Schema;
class PumpParams;

class SchemaWriterJson
{
public:
    SchemaWriterJson(Schema *schema) : _schema(schema) {}

    void writeToFile(const QString& fileName);
    QString writeToString();

    const Z::Report& report() const { return _report; }

private:
    Schema *_schema;
    Z::Report _report;

    void writeGeneral(QJsonObject& root);
    void writeCustomParams(QJsonObject& root);
    void writeParamLinks(QJsonObject& root);
    void writeFormulas(QJsonObject& root);
    void writeWindows(QJsonObject& root);
};

namespace Z {
namespace IO {
namespace Json {

void writeElements(QJsonObject& root, const QList<Element*>& elements);
void writeElement(QJsonObject& root, Element *elem);

void writePumps(QJsonObject& root, const QList<PumpParams*>& pumps);
void writePump(QJsonObject &root, PumpParams *pump);

} // namespace Json
} // namespace IO
} // namespace Z

#endif // SCHEMA_WRITER_JSON_H
