#include "CustomElemsManager.h"

#include "core/Schema.h"
#include "io/SchemaReaderJson.h"

#include "tools/OriSettings.h"

namespace CustomElemsManager {

QString customElemsFileName()
{
    Ori::Settings s;
    return s.settings()->fileName().section('.', 0, -2) % '.' % QStringLiteral("elems.rez");
}

Schema* load()
{
    Schema *schema = new Schema;
    SchemaReaderJson reader(schema);
    reader.readFromFile(customElemsFileName());
    auto report = reader.report();
    if (!report.isEmpty())
    {
        // TODO: report error to the user explicitly
        report.writeToStdout();
        delete schema;
        return nullptr;
    }

    return schema;
}

} // namespace CustomElemsManager

