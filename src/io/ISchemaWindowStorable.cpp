#include "ISchemaWindowStorable.h"

ISchemaWindowStorable::~ISchemaWindowStorable()
{
}

bool ISchemaWindowStorable::storableWrite(QJsonObject& root, Z::Report *report)
{
    Q_UNUSED(root)
    Q_UNUSED(report)
    return true;
}

bool ISchemaWindowStorable::storableRead(const QJsonObject& root, Z::Report *report)
{
    Q_UNUSED(root)
    Q_UNUSED(report)
    return true;
}
