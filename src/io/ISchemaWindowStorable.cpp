#include "ISchemaWindowStorable.h"

ISchemaWindowStorable::~ISchemaWindowStorable()
{
}

QString ISchemaWindowStorable::storableWrite(QJsonObject& root)
{
    Q_UNUSED(root);
    return QString();
}

QString ISchemaWindowStorable::storableRead(const QJsonObject& root)
{
    Q_UNUSED(root);
    return QString();
}
