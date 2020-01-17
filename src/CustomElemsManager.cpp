#include "CustomElemsManager.h"

#include "core/Schema.h"
#include "core/ElementsCatalog.h"
#include "io/SchemaReaderJson.h"
#include "io/SchemaWriterJson.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "tools/OriSettings.h"
#include "widgets/OriInfoPanel.h"

#include <QApplication>
#include <QLabel>
#include <QLineEdit>

namespace CustomElemsManager {

QString customElemsFileName()
{
    Ori::Settings s;
    return s.settings()->fileName().section('.', 0, -2) % '.' % QStringLiteral("elems.rez");
}

Schema* load()
{
    Schema *schema = new Schema(QStringLiteral("custom_elems"));
    schema->events().disable();
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
    schema->events().enable();
    return schema;
}

void save(Schema* schema)
{
    SchemaWriterJson writer(schema);
    writer.writeToFile(customElemsFileName());
    if (!writer.report().isEmpty())
    {
        // TODO: report error to the user explicitly
    }
}

static bool hasLinksForElement(Schema* schema, Element* elem)
{
    for (const auto p : elem->params())
        if (schema->paramLinks()->byTarget(p))
            return true;
    return false;
}

void saveAsCustom(Schema* schema, Element* elem)
{
    QSharedPointer<Schema> customElems(load());
    if (!customElems)
    {
        // TODO: report error to the user
        return;
    }

    auto titleEditor = new Ori::Dlg::InputTextEditor;
    titleEditor->setText(elem->title());

    auto infoPanel = new Ori::Widgets::InfoPanel;
    if (hasLinksForElement(schema, elem))
        infoPanel->setInfo(qApp->tr("Some of the source element parameters are linked to the global parameters. "
            "These links will not be preserved in custom element, because they relate to the current schema, "
            "while the custom element is schema independent.", "Save custom element"));
    else infoPanel->setVisible(false);

    auto content = Ori::Layouts::LayoutV({
        titleEditor,
        infoPanel
    }).setMargin(0).makeWidget();

    bool ok = Ori::Dlg::Dialog(content, true)
        .withTitle(qApp->tr("Save custom element", "Save custom element"))
        .withVerticalPrompt(qApp->tr("Element title", "Save custom element"))
        .withContentToButtonsSpacingFactor(2)
        .withVerification([titleEditor, customElems](){
            QString newTitle = titleEditor->text().trimmed();
            if (newTitle.isEmpty())
                return qApp->tr("Element title should not be empty", "Save custom element");
            for (auto elem : customElems->elements())
                if (elem->title() == newTitle)
                    return qApp->tr("Another custom element with such title already exists", "Save custom element");
            return QString();
        })
        .exec();

    if (!ok) return;

    auto newElem = ElementsCatalog::instance().create(elem, true);
    newElem->setTitle(titleEditor->text().trimmed());
    customElems->insertElement(newElem);
    save(customElems.data());
}

} // namespace CustomElemsManager

