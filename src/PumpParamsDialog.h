#ifndef PUMP_PARAMS_DIALOG_H
#define PUMP_PARAMS_DIALOG_H

#include "RezonatorDialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class ParamsEditorTS;
class PumpParams;

class PumpParamsDialog : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit PumpParamsDialog(PumpParams *params, QWidget *parent = nullptr);

    static PumpParams *makeNewPump();
    static bool editPump(PumpParams *params);

public slots:
    void collect() override;

protected:
    void showEvent(QShowEvent*) override;
    QString helpTopic() const override { return "input_beam.html"; }

private:
    PumpParams *_params;
    ParamsEditorTS *_paramsEditor;
    QLineEdit *_editorLabel;
    QLineEdit *_editorTitle;
};

#endif // PUMP_PARAMS_DIALOG_H
