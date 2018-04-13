#ifndef SCHEMA_READER_INI_H
#define SCHEMA_READER_INI_H

#include "../core/Parameters.h"
#include "../core/Report.h"
#include "core/OriVersion.h"

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

class IniSection;
class Schema;

/**
    Methods for reading schema and elements from INI file.
    For backward compatibility with reZonator 1.* version.
*/
class SchemaReaderIni
{
public:
    SchemaReaderIni(Schema *schema) : _schema(schema) {}

    void readFromFile(const QString& fileName);

    const Z::Report& report() const { return _report; }
    bool ok() const { return !_report.hasErrors(); }

private:
    Schema* _schema;
    Z::Report _report;
    QSettings *_file = nullptr;
    Ori::Version _version;
    Z::Unit _linearUnit;
    Z::Unit _angularUnit;
    Z::Unit _beamsizeUnit;
    Z::Unit _lambdaUnit;

    void readGeneral(IniSection &ini);
    void readLambda(IniSection &ini);
    void readUnits(IniSection &ini);
    void readPump(IniSection &ini);
    void readElements();
    void readElement(const QString& section);
    void readWindows();

    Z::Unit paramUnit(Z::Parameter* param) const;
};

#endif // SCHEMA_READER_INI_H
