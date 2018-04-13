#ifndef Z_SCHEMA_STORABLE_H
#define Z_SCHEMA_STORABLE_H

#include <QString>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

/**
    Interface for object who wants to be saved into schema file.
*/
class ISchemaStorable
{
public:
    virtual QString type() const = 0;
    virtual QString write(QJsonObject& root) = 0;
    virtual QString read(const QJsonObject& root) = 0;
};

#endif // Z_SCHEMA_STORABLE_H
