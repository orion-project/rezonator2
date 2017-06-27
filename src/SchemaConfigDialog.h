#ifndef SCHEMA_CONFIG_DLG_H
#define SCHEMA_CONFIG_DLG_H

#include "../orion/BasicConfigDlg.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QTextEdit;
QT_END_NAMESPACE

class Schema;

class SchemaConfigDialog : public Ori::Gui::BasicConfigDialog
{
    Q_OBJECT

public:
    SchemaConfigDialog(QWidget* parent, Schema* schema);

    ///// inherited from BasicConfigDialog
    virtual void populate();
    virtual void collect();

private:
    QComboBox *comboUnitsLinear;
    QComboBox *comboUnitsAngle;
    QComboBox *comboUnitsBeamsize;
    QComboBox *comboUnitsLambda;
    QTextEdit *editorComment;
    QLineEdit *editorTitle;

    Schema* _schema;

    QWidget* createGeneralPage();
    QWidget* createUnitsPage();
    QWidget* createCalcPage();
    QWidget* createCommentPage();
};

#endif // SCHEMA_CONFIG_DLG_H
