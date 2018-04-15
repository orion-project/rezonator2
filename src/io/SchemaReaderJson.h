#ifndef SCHEMA_READER_JSON_H
#define SCHEMA_READER_JSON_H

#include "../core/Report.h"

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

class Element;
class Schema;

class SchemaReaderJson
{
public:
    SchemaReaderJson(Schema *schema) : _schema(schema) {}

    void readFromFile(const QString& fileName);
    void readFromString(const QString& text);
    void readFromUtf8(const QByteArray& data);

    const Z::Report& report() const { return _report; }

private:
    Schema *_schema;
    Z::Report _report;

    void readGeneral(const QJsonObject& root);
    void readCustomParams(const QJsonObject& root);
    void readCustomParam(const QJsonObject& root, const QString& alias);
    void readPump(const QJsonObject& root);
    void readElements(const QJsonObject& root);
    void readElement(const QJsonObject& root);
    void readParamLinks(const QJsonObject& root);
    void readParamLink(const QJsonObject& root);
    void readWindows(const QJsonObject& root);
    void readWindow(const QJsonObject& root);
};

#endif // SCHEMA_READER_JSON_H
