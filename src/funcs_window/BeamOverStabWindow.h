#ifndef BEAM_VS_STAB_WINDOW_H
#define BEAM_VS_STAB_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../RezonatorDialog.h"
#include "../funcs/BeamOverStabFunction.h"

class ElemAndParamSelector;
class ElemOffsetSelectorWidget;
namespace VariableRangeEditor {
    class GeneralRangeEd;
}

class BeamOverStabWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    BeamOverStabWindow(Schema*);

    BeamOverStabFunction* function() const { return dynamic_cast<BeamOverStabFunction*>(_function); }

protected:
    // Implementation of PlotFuncWindow
    bool configureInternal() override;
};


class BeamOverStabParamsDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit BeamOverStabParamsDlg(Schema*, Z::Variable*);

protected slots:
    void collect() override;

private slots:
    void guessRange();

private:
    Z::Variable* _var;
    ElemAndParamSelector* _elemSelector;
    ElemOffsetSelectorWidget* _placeSelector;
    VariableRangeEditor::GeneralRangeEd* _rangeEditor;
    QString _recentKey = "func_beam_vs_stab";

    void populate();
};

#endif // BEAM_VS_STAB_WINDOW_H
