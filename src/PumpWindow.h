#ifndef PUMP_WINDOW_H
#define PUMP_WINDOW_H

#include "RezonatorDialog.h"
#include "core/Schema.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class PumpParamsEditor : public QWidget
{
public:
    PumpParamsEditor() : QWidget() {}
    virtual void collect() {}
};


class PumpWindow : public RezonatorDialog
{
    Q_OBJECT

public:
    static bool edit(QWidget *parent, class Schema* schema);

private slots:
    void inputTypeChanged(int mode);
    void collect() override;

private:
    explicit PumpWindow(QWidget *parent, class Schema* schema);

    Schema *_schema;
    Z::Pump::Params _params;
    QLabel* _drawing;
    QWidget *_paramEditor;
    QMap<Z::Pump::Params::Mode, PumpParamsEditor*> _paramEditors;
};

#endif // PUMP_WINDOW_H
