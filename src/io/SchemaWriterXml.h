#ifndef SCHEMA_WRITER_XML_H
#define SCHEMA_WRITER_XML_H

#include "SchemaFile.h"

QT_BEGIN_NAMESPACE
class QDomElement;
QT_END_NAMESPACE

namespace Z {
namespace IO {
namespace XML {
class Writer;
}}}

/**
    Methods for writing schema and elements into XML file.
*/
class SchemaWriterXml : public SchemaFile
{
public:
    SchemaWriterXml(Schema *schema, const QString& fileName);
    SchemaWriterXml(Schema *schema);
    ~SchemaWriterXml();

    void write() override;
    QString writeToString();

private:
    Z::IO::XML::Writer* _writer;

    void writeInternal();
    void writeGeneral(QDomElement& root);
    void writePump(QDomElement &root);
    void writeElements(QDomElement& root);
    void writeElement(QDomElement& root, Element *elem);
    void writeWindows(QDomElement& root);

    static QString tripTypeStr(Schema::TripType tripType);
};

#endif // SCHEMA_WRITER_XML_H
