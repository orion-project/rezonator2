#ifndef SCHEMA_WRITER_JSON_H
#define SCHEMA_WRITER_JSON_H

#include <QString>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

class Element;
class Schema;

class SchemaWriterJson
{
public:
    SchemaWriterJson(Schema *schema);

    QString writeToString();
    void writeToFile(const QString& fileName);

private:
    Schema *_schema;

    void writeGeneral(QJsonObject& root);
    void writePump(QJsonObject &root);
    void writeElements(QJsonObject& root);
    void writeElement(QJsonObject& root, Element *elem);
    void writeWindows(QJsonObject& root);
//    void writeParam(QJsonObject& root, Z::Parameter *param);
//    void writeValue(QJsonObject& root, Z::Value *value);
};

#endif // SCHEMA_WRITER_JSON_H
