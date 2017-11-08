#ifndef SCHEMA_READER_INI_H
#define SCHEMA_READER_INI_H

#include "SchemaFile.h"

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

class IniSection;

/**
    Methods for reading schema and elements from INI file.
    For backward compatibility with reZonator 1.* version.
*/
class SchemaReaderIni : public SchemaFile
{
public:
    SchemaReaderIni(Schema *schema, const QString& fileName);
    ~SchemaReaderIni();

    void read() override;

protected:
    virtual void readWindows() {}

private:
    QSettings *_file = nullptr;
    Ori::Version _version;
    Z::Unit _linearUnit;
    Z::Unit _angularUnit;
    Z::Unit _beamsizeUnit;
    Z::Unit _lambdaUnit;

    void readGeneral(IniSection &ini);
    void readLambda(IniSection &ini);
    void readUnits();
    void readPump(IniSection &ini);
    void readElements();
    void readElement(const QString& section);

    Z::Unit paramUnit(Z::Parameter* param) const;
};

#endif // SCHEMA_READER_INI_H
