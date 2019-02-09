#ifndef BEAM_VARIATION_WINDOW_H
#define BEAM_VARIATION_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../RezonatorDialog.h"
#include "../funcs/BeamVariationFunction.h"

class ElemAndParamSelector;
class ElemOffsetSelectorWidget;
class GeneralRangeEditor;

class BeamVariationWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    BeamVariationWindow(Schema*);

    BeamVariationFunction* function() const {
        return dynamic_cast<BeamVariationFunction*>(_function);
    }

protected:
    // Implementation of PlotFuncWindow
    bool configureInternal() override;
};


class BeamVariationParamsDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit BeamVariationParamsDlg(Schema*, Z::Variable*);

protected slots:
    void collect() override;

private slots:
    void guessRange();

private:
    Z::Variable* _var;
    ElemAndParamSelector* _elemSelector;
    ElemOffsetSelectorWidget* _placeSelector;
    GeneralRangeEditor* _rangeEditor;
    QString _recentKey = "func_beam_vs_stab";

    void populate();
};

#endif // BEAM_VARIATION_WINDOW_H
