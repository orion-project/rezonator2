#ifndef WINDOW_STABILITY_MAP_H
#define WINDOW_STABILITY_MAP_H

#include "PlotFuncWindowStorable.h"
#include "../RezonatorDialog.h"
#include "../funcs/StabilityMapFunction.h"

class ElemAndParamSelector;
class QCPItemStraightLine;
class GeneralRangeEditor;

class StabilityMapWindow final : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit StabilityMapWindow(Schema*);

    StabilityMapFunction* function() const { return dynamic_cast<StabilityMapFunction*>(_function); }

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    bool configureInternal() override;
    void afterUpdate() override;
    void fillViewMenuActions(QList<QAction*>& actions) const override;
    Z::Unit getDefaultUnitX() const override;
    QString getCursorInfo(const QPointF& pos) const override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;
    QString readWindowSpecific(const QJsonObject& root) override;
    QString writeWindowSpecific(QJsonObject& root) override;

private:
    QAction *_actnStabilityAutolimits, *_actnStabBoundMarkers;
    QCPItemStraightLine *_stabBoundMarkerLow, *_stabBoundMarkerTop;

    void updateStabBoundMarkers();
    void toggleStabBoundMarkers(bool on);

    QCPItemStraightLine* makeStabBoundMarker() const;

    void autolimitsStability();
};


/**
    The function argument dialog that can choose one of schema element's parameters
    and set variation of this parameter and the number of points for plotting.
*/
class StabilityParamsDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit StabilityParamsDlg(Schema*, Z::Variable*);

protected slots:
    void collect() override;

private slots:
    void guessRange();

private:
    Z::Variable* _var;
    ElemAndParamSelector* _elemSelector;
    GeneralRangeEditor* _rangeEditor;
    QString _recentKey = "func_stab_map";

    void populate();
};

#endif // WINDOW_STABILITY_MAP_H
