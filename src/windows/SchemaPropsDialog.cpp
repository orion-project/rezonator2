#include "SchemaPropsDialog.h"

#include "../app/Appearance.h"
#include "../core/Schema.h"

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>

namespace Z {
namespace Dlg {

bool editSchemaProps(QWidget* parent, Schema* schema)
{
    SchemaPropsDialog dialog(parent, schema);
    bool ok = dialog.exec() == QDialog::Accepted;
    if (ok && dialog.isModified())
        schema->events().raise(SchemaEvents::Changed, "Schema changed via props dialog");
    return ok;
}

} // namespace Dlg
} // namespace Z

SchemaPropsDialog::SchemaPropsDialog(QWidget *parent, Schema *schema)
    : Ori::Dlg::BasicConfigDialog(parent), _schema(schema)
{
    pageListIconSize = QSize(48, 48);

    setObjectName("SchemaPropsDialog");
    setTitleAndIcon(tr("Schema Properties"), ":/window_icons/options");

    createPages({
                    createGeneralPage(),
                    createNotesPage(),
                });
}

QWidget* SchemaPropsDialog::createGeneralPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("General"), ":/toolbar/options");

    _titleEditor = new QLineEdit;
    _titleEditor->setFont(Z::Gui::ValueFont().get());

    page->add({
                  new QLabel(tr("Title")),
                  _titleEditor,
                  page->stretch()
              });
    return page;
}

QWidget* SchemaPropsDialog::createNotesPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Notes"), ":/config_pages/note");

    _notesEditor = new QPlainTextEdit;
    _notesEditor->setFont(Z::Gui::ValueFont().get());

    page->add({_notesEditor});
    return page;
}

void SchemaPropsDialog::populate()
{
    _titleEditor->setText(_schema->title());
    _notesEditor->setPlainText(_schema->notes());
}

bool SchemaPropsDialog::collect()
{
    _isModified = false;

    auto title = _titleEditor->text();
    if (title != _schema->title())
    {
        _schema->setTitle(title);
        _isModified = true;
    }

    auto notes = _notesEditor->toPlainText();
    if (notes != _schema->notes())
    {
        _schema->setNotes(notes);
        _isModified = true;
    }

    return true;
}
