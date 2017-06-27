#ifndef SCHEMA_READER_XML_H
#define SCHEMA_READER_XML_H

#include "SchemaFile.h"

QT_BEGIN_NAMESPACE
class QDomElement;
QT_END_NAMESPACE

namespace Z {
namespace IO {
namespace XML {
class Reader;
}}}


/**
    Methods for reading schema and elements from XML file.
*/
class SchemaReaderXml : public SchemaFile
{
public:
    SchemaReaderXml(Schema *schema, const QString &fileName);
    ~SchemaReaderXml();

    void read() override;
    void read(const QString&);

private:
    Z::IO::XML::Reader* _reader;

    void readInternal();
    void readTripType(QDomElement& root);
    void readGeneral(QDomElement& root);
    void readPump(QDomElement& root);
    void readElements(QDomElement& root);
    void readElement(QDomElement& root);
    void readStorables(QDomElement& root);

    Schema::TripType readTripType(QDomElement root, const QString& name);
};

#endif // SCHEMA_READER_XML_H
