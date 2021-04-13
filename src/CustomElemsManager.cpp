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
#include <QFile>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>

namespace CustomElemsManager {

QString libraryFile()
{
    Ori::Settings s;
    return s.settings()->fileName().section('.', 0, -2) % '.' % QStringLiteral("elems.rez");
}

Ori::Result<Schema*> loadLibrary()
{
    Schema *library = new Schema(QStringLiteral("custom_elems"));

    QString fileName = libraryFile();
    if (!QFile::exists(fileName))
        return Ori::Result<Schema*>::ok(library);

    library->events().disable();
    SchemaReaderJson reader(library);
    reader.readFromFile(libraryFile());
    auto report = reader.report();
    if (!report.isEmpty())
    {
        qDebug() << "There are messages while loading Custom Element Library";
        report.writeToStdout();
        delete library;
        return Ori::Result<Schema*>::fail(report.str());
    }
    library->events().enable();

    for (Element* elem : library->elements())
        elem->setOption(Element_CustomSample);

    return Ori::Result<Schema*>::ok(library);
}

QString saveLibrary(Schema* library)
{
    SchemaWriterJson writer(library);
    writer.writeToFile(libraryFile());
    auto report = writer.report();
    if (!report.isEmpty())
    {
        qDebug() << "There are messages while saving Custom Element Library";
        report.writeToStdout();
        return report.str();
    }
    return QString();
}

static bool hasLinksForElement(Schema* schema, Element* elem)
{
    for (const auto p : elem->params())
        if (schema->paramLinks()->byTarget(p))
            return true;
    return false;
}

QString saveToLibrary(Element* elem)
{
    auto res = loadLibrary();
    if (!res.ok())
        return res.error();

    QSharedPointer<Schema> library(res.result());

    auto labelEditor = new QLineEdit;
    labelEditor->setText(elem->labelPrefix());
    auto titleEditor = new Ori::Dlg::InputTextEditor;
    titleEditor->setText(elem->title());

    QBoxLayout *infoLayout = nullptr;
    auto schema = dynamic_cast<Schema*>(elem->owner());
    if (schema and hasLinksForElement(schema, elem))
    {
        auto infoPanel = new Ori::Widgets::InfoPanel;
        infoPanel->setInfo(qApp->tr("Some of the source element parameters are linked to the global parameters. "
            "These links will not be preserved in custom element, because they relate to the current schema, "
            "while the custom element is schema independent.", "Save custom element"));
        infoLayout = Ori::Layouts::LayoutV({Ori::Layouts::Space(12), infoPanel}).setMargin(0).boxLayout();
    }

    auto layoutCommon = new QFormLayout;
    layoutCommon->setMargin(0);
    layoutCommon->addRow(qApp->tr("Label:", "Save custom element"), labelEditor);
    layoutCommon->addRow(qApp->tr("Title:", "Save custom element"), titleEditor);

    QSharedPointer<QWidget> content(Ori::Layouts::LayoutV({layoutCommon, infoLayout}).setMargin(0).makeWidget());

    bool ok = Ori::Dlg::Dialog(content.data(), false)
        .withTitle(qApp->tr("Save to Custom Library", "Save custom element"))
        .withContentToButtonsSpacingFactor(2)
        .exec();

    if (!ok) return QString();

    auto newElem = ElementsCatalog::instance().create(elem, true);
    newElem->setLabel(labelEditor->text().trimmed());
    newElem->setTitle(titleEditor->text().trimmed());
    library->insertElements({newElem}, -1, Arg::RaiseEvents(false));
    return saveLibrary(library.data());
}

} // namespace CustomElemsManager

