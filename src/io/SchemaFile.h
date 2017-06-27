#ifndef SCHEMA_FILE_H
#define SCHEMA_FILE_H

#include "../core/Schema.h"
#include "z_io_report.h"

////////////////////////////////////////////////////////////////////////////////

class SchemaLoadingProcess
{
public:
    SchemaLoadingProcess(Schema *schema, const QString& fileName): _schema(schema), _fileName(fileName)
    {
        _schema->events().raise(SchemaEvents::Loading);
        _schema->events().disable();
    }

    ~SchemaLoadingProcess()
    {
        _schema->setFileName(_fileName);
        _schema->events().enable();
        _schema->events().raise(SchemaEvents::Loaded);
    }

private:
    Schema* _schema;
    QString _fileName;
};

////////////////////////////////////////////////////////////////////////////////
/// Base class for schema reader/writer.
///
class SchemaFile
{
public:
    SchemaFile(Schema* schema, const QString& fileName) : _schema(schema), _fileName(fileName) {}
    virtual ~SchemaFile() {}

    Schema* schema() const { return _schema; }

    virtual void read() {}
    virtual void write() {}

    const Z::IO::Report& report() const { return _report; }
    bool ok() const { return _report.ok(); }

protected:
    Schema* _schema;
    QString _fileName;
    Z::IO::Report _report;
};

////////////////////////////////////////////////////////////////////////////////

#endif // SCHEMA_FILE_H

