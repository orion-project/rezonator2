#ifndef Z_IO_JSON_H
#define Z_IO_JSON_H

#include "../core/CommonTypes.h"
#include "../core/Values.h"

#include <QJsonObject>

class Schema;

namespace Z {

struct VariableRange;
struct Variable;

namespace IO {
namespace Json {

QJsonObject writeValue(const Value& value);
Result<Value> readValue(const QJsonObject& json, Dim dim = nullptr);
QJsonObject writeValueTS(const ValueTS& value);
Result<ValueTS> readValueTS(const QJsonObject& json, Dim dim = nullptr);

QJsonObject writeVariableRange(const VariableRange& range);
QString readVariableRange(const QJsonObject& json, VariableRange& range, Dim dim = nullptr);
QJsonObject writeVariable(Variable *var, Schema *schema);
QString readVariable(const QJsonObject& json, Variable *var, Schema *schema);
QJsonObject writeVariablePref(Variable *var);
void readVariablePref(const QJsonObject& json, Variable *var, Schema *schema);

} // namespace Json
} // namespace IO
} // namespace Z

#endif // Z_IO_JSON_H
