#ifndef PUMP_WINDOW_H
#define PUMP_WINDOW_H

#include "RezonatorDialog.h"
#include "core/Schema.h"

QT_BEGIN_NAMESPACE
class QBoxLayout;
class QLabel;
QT_END_NAMESPACE

class ValueEditorTS;

//------------------------------------------------------------------------------
/*
class PumpParamsEditor : public QWidget
{
public:
    PumpParamsEditor() : QWidget() {}
    virtual void collect() {}
};

//------------------------------------------------------------------------------

class WaistParamsEditor : public PumpParamsEditor
{
    Q_OBJECT
public:
    WaistParamsEditor(Z::Pump::WaistParams *params);
private:
    Z::Pump::WaistParams *_params;
    ValueEditorTS *_waist, *_dist, *_MI;
};

//------------------------------------------------------------------------------

class FrontParamsEditor : public PumpParamsEditor
{
    Q_OBJECT
public:
    FrontParamsEditor(Z::Pump::FrontParams *params);
private:
    Z::Pump::FrontParams *_params;
    ValueEditorTS *_beam, *_front, *_MI;
};

//------------------------------------------------------------------------------

class ComplexParamsEditor : public PumpParamsEditor
{
    Q_OBJECT
public:
    ComplexParamsEditor(Z::Pump::ComplexParams *params);
private:
    Z::Pump::ComplexParams *_params;
    ValueEditorTS *_im, *_re, *_MI;
};

//------------------------------------------------------------------------------

class RayVectorParamsEditor : public PumpParamsEditor
{
    Q_OBJECT
public:
    RayVectorParamsEditor(Z::Pump::RayVectorParams *params);
private:
    Z::Pump::RayVectorParams *_params;
    ValueEditorTS *_angle, *_dist, *_radius;
};

//------------------------------------------------------------------------------

class TwoSectionsParamsEditor : public PumpParamsEditor
{
    Q_OBJECT
public:
    TwoSectionsParamsEditor(Z::Pump::TwoSectionsParams *params);
private:
    Z::Pump::TwoSectionsParams *_params;
    ValueEditorTS *_radius1, *_radius2, *_dist;
};
*/
//------------------------------------------------------------------------------
class PumpWindow : public RezonatorDialog
{
    Q_OBJECT

public:
    static bool edit(QWidget *parent, class Schema* schema);

private slots:
    //void inputTypeChanged(int mode);
    void collect() override;

private:
    explicit PumpWindow(QWidget *parent, class Schema* schema);
/*
    Schema *_schema;
    Z::Pump::Params _params;
    QLabel* _drawing;
    QBoxLayout *_paramEditorsLayout;
    QMap<Z::Pump::Params::Mode, PumpParamsEditor*> _paramEditors;*/
};

#endif // PUMP_WINDOW_H
