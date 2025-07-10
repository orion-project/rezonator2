#ifndef Z_IO_JSON_H
#define Z_IO_JSON_H

#include "../core/CommonTypes.h"
#include "../core/Values.h"

#include "core/OriVersion.h"

#include <QJsonObject>
#include <QJsonArray>

#define JSON_KEY_VERSION QStringLiteral("schema_version")

class Schema;
class Element;

namespace Z {

struct VariableRange;
struct Variable;
class Report;

namespace IO {
namespace Json {

inline Ori::Version currentVersion() { return Ori::Version(2, 1); }

void writeUnit(QJsonObject& json, Unit unit);
Result<Unit> readUnit(const QJsonObject& json, Dim dim);

QJsonObject writeValue(const Value& value);
Result<Value> readValue(const QJsonObject& json, Dim dim = nullptr);
QJsonObject writeValueTS(const ValueTS& value);
Result<ValueTS> readValueTS(const QJsonObject& json, Dim dim = nullptr);

QJsonObject writeVariableRange(const VariableRange& range);
QString readVariableRange(const QJsonObject& json, VariableRange& range, Dim dim = nullptr);
QJsonObject writeVariable(const Variable *var, Schema *schema);
QString readVariable(const QJsonObject& json, Variable *var, Schema *schema);
QJsonObject writeVariablePref(Variable *var);
void readVariablePref(const QJsonObject& json, Variable *var, Schema *schema);

Result<Element*> readElemByIndex(const QJsonObject& json, const QString& key, Schema *schema);

/**
    Json value wrapper allowing to track value path inside file.
    Path can be useful for logging when reading of some value fails.
*/
class JsonValue
{
public:
    JsonValue(const QJsonObject& root, const QString& key, Z::Report* report);
    JsonValue(const JsonValue& root, const QString& key, Z::Report* report);

    QString msg() const { return _msg; }

    const QJsonObject& obj() const { return _obj; }
    const QJsonArray& array() const { return _array; }

    operator bool() const { return _msg.isEmpty(); }

private:
    QString _path;
    QString _msg;
    QJsonObject _obj;
    QJsonArray _array;

    void initObj(const QJsonObject& root, const QString& key);
};

} // namespace Json
} // namespace IO
} // namespace Z

#endif // Z_IO_JSON_H
