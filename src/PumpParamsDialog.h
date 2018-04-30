#ifndef PUMP_PARAMS_DIALOG_H
#define PUMP_PARAMS_DIALOG_H

#include "RezonatorDialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class Schema;
class ParamsEditorTS;
namespace Z {
class PumpParams;
}

class PumpParamsDialog : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit PumpParamsDialog(Z::PumpParams *params, QWidget *parent = nullptr);

    static Z::PumpParams *makeNewPump();
    static bool editPump(Z::PumpParams *params);

public slots:
    void collect() override;

protected:
    void showEvent(QShowEvent*) override;

private:
    Z::PumpParams *_params;
    ParamsEditorTS *_paramsEditor;
    QLineEdit *_editorLabel;
    QLineEdit *_editorTitle;
};

#endif // PUMP_PARAMS_DIALOG_H
