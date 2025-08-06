#ifndef BEAM_VARIATION_WINDOW_H
#define BEAM_VARIATION_WINDOW_H

#include "../funcs/PlotFuncWindowStorable.h"
#include "../math/BeamVariationFunction.h"
#include "../windows/RezonatorDialog.h"

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
    void getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values) override;
    QList<BasicMdiChild::MenuItem> viewMenuItems() const override;

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

protected:
    QString helpTopic() const override { return BeamVariationFunction::help_topic(); }

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

    void populate();
};

#endif // BEAM_VARIATION_WINDOW_H
