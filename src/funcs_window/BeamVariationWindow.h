#ifndef BEAM_VARIATION_WINDOW_H
#define BEAM_VARIATION_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../RezonatorDialog.h"
#include "../funcs/BeamVariationFunction.h"

class BeamShapeExtension;
class ElemAndParamSelector;
class ElemOffsetSelectorWidget;
class GeneralRangeEditor;

class BeamVariationWindow final : public PlotFuncWindowStorable
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
    Z::Unit getDefaultUnitX() const override;
    Z::Unit getDefaultUnitY() const override;
    QString getCursorInfo(const QPointF& pos) const override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;

private:
    BeamShapeExtension* _beamShape;
};


class BeamVariationParamsDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit BeamVariationParamsDlg(Schema*, Z::Variable*, Z::PlotPosition *pos);

protected slots:
    void collect() override;

private slots:
    void guessRange();

private:
    Schema* _schema;
    Z::Variable* _var;
    Z::PlotPosition* _pos;
    ElemAndParamSelector* _elemSelector;
    ElemOffsetSelectorWidget* _placeSelector;
    GeneralRangeEditor* _rangeEditor;
    QString _recentKey = "func_beam_variation";

    void populate();
};

#endif // BEAM_VARIATION_WINDOW_H
