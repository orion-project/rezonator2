#ifndef WINDOW_STABILITY_MAP_H
#define WINDOW_STABILITY_MAP_H

#include "PlotFuncWindowStorable.h"
#include "../funcs/StabilityMapFunction.h"

class QCPItemStraightLine;

class StabilityMapWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit StabilityMapWindow(Schema*);

    StabilityMapFunction* function() const { return dynamic_cast<StabilityMapFunction*>(_function); }

    // Implementation of BasicMdiChild
    QList<QAction*> viewActions() override;

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    bool configureInternal() override;
    void afterUpdate() override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;
    QString readWindowSpecific(const QJsonObject& root) override;
    QString writeWindowSpecific(QJsonObject& root) override;

private:
    QAction *_actnStabilityAutolimits, *_actnStabBoundMarkers;
    QCPItemStraightLine *_stabBoundMarkerLow, *_stabBoundMarkerTop;

    void createControl();

    void updateStabBoundMarkers();
    void toggleStabBoundMarkers(bool on);

    QCPItemStraightLine* makeStabBoundMarker() const;

    void autolimitsStability();
};

#endif // WINDOW_STABILITY_MAP_H
