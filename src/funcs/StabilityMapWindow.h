#ifndef WINDOW_STABILITY_MAP_H
#define WINDOW_STABILITY_MAP_H

#include "../funcs/PlotFuncWindowStorable.h"
#include "../math/StabilityMapFunction.h"
#include "../windows/RezonatorDialog.h"

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
    QList<BasicMdiChild::MenuItem> viewMenuItems() const override;
    QList<BasicMdiChild::MenuItem> formatMenuItems() const override;
    Z::Unit getDefaultUnitX() const override;
    void getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values) override;
    SpecPointParams getSpecPointsParams() const override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;
    QString readWindowSpecific(const QJsonObject& root) override;
    QString writeWindowSpecific(QJsonObject& root) override;

    // inherited from IAppSettingsListener
    void optionChanged(AppSettingsOption option) override;

private:
    QAction *_actnStabilityAutolimits, *_actnStabBoundMarkers, *_actnStabBoundMarkersFormat;
    QCPItemStraightLine *_stabBoundMarkerLow, *_stabBoundMarkerTop;
    std::optional<QPen> _stabBoundMarkerPen;

    void updateStabBoundMarkers();
    void toggleStabBoundMarkers(bool on);
    void formatStabBoundMarkers();
    QPen stabBoundMarkerPen() const;
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

    void populate();
};

#endif // WINDOW_STABILITY_MAP_H
