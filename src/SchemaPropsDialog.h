#ifndef SCHEMA_PROPS_DIALOG_H
#define SCHEMA_PROPS_DIALOG_H

#include "dialogs/OriBasicConfigDlg.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPlainTextEdit;
QT_END_NAMESPACE

class Schema;

class SchemaPropsDialog : public Ori::Dlg::BasicConfigDialog
{
    Q_OBJECT

public:
    explicit SchemaPropsDialog(QWidget *parent, Schema* schema);

    // inherited from BasicConfigDialog
    virtual void populate() override;
    virtual bool collect() override;

    bool isModified() const { return _isModified; }

private:
    bool _isModified = false;
    QLineEdit* _titleEditor;
    QPlainTextEdit* _notesEditor;
    Schema* _schema;

    QWidget* createGeneralPage();
    QWidget* createNotesPage();
};

namespace Z {
namespace Dlg {

bool editSchemaProps(QWidget* parent, Schema* schema);

} // namespace Dlg
} // namespace Z

#endif // SCHEMA_PROPS_DIALOG_H
