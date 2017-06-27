#ifndef Z_SCHEMA_STORABLE_H
#define Z_SCHEMA_STORABLE_H

#include <QString>

QT_BEGIN_NAMESPACE
class QDomElement;
QT_END_NAMESPACE

namespace Z {
namespace IO {
namespace XML {
class Writer;
class Reader;
}}}

/**
    Interface for object who wants to be saved into schema file.
*/
class ISchemaStorable
{
public:
    virtual QString type() const = 0;

    virtual void write(Z::IO::XML::Writer*, QDomElement&) = 0;
    virtual bool read(Z::IO::XML::Reader*, QDomElement&) = 0;
};

#endif // Z_SCHEMA_STORABLE_H
