#ifndef MULTI_CAUSTIC_WINDOW_H
#define MULTI_CAUSTIC_WINDOW_H

#include "../core/Values.h"
#include "../funcs/PlotFuncWindowStorable.h"
#include "../math/MultirangeCausticFunction.h"

class QCPItemStraightLine;

class MulticausticWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    struct FuncOffset
    {
        CausticFunction *func;
        Z::ValueSi offset;
        Z::ValueSi position;
    };

public:
    MulticausticWindow(MultirangeCausticFunction* function);

    MultirangeCausticFunction* function() const { return dynamic_cast<MultirangeCausticFunction*>(_function); }

    // Implementation of SchemaListener
    void schemaRebuilt(Schema*) override;
    void elementChanged(Schema*, Element*) override;
    void elementDeleting(Schema*, Element*) override;

    // Implementation of PlotFuncWindow
    void storeView(FuncMode) override;
    void restoreView(FuncMode) override;

    // inherited from IAppSettingsListener
    void optionChanged(AppSettingsOption option) override;

protected:
    // Implementation of PlotFuncWindow
    bool configureInternal() override;
    void afterUpdate() override;
    QList<BasicMdiChild::MenuItem> viewMenuItems() const override;
    QList<BasicMdiChild::MenuItem> formatMenuItems() const override;
    void updateGraphs() override;
    ElemDeletionReaction reactElemDeletion(const Elements&) override;
    void showRoundTrip() override;
    Z::Unit getDefaultUnitX() const override;
    Z::Unit getDefaultUnitY() const override;
    Z::Unit getDefaultUnitY(FuncMode mode) const override;
    SpecPointParams getSpecPointsParams() const override;
    void getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values) override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;
    QString readWindowSpecific(const QJsonObject& root) override;
    QString writeWindowSpecific(QJsonObject& root) override;

    FuncOffset findFuncOffset(const Z::Value& x) const;
private:
    QList<QCPItemStraightLine*> _elemBoundMarkers;
    QAction *_actnElemBoundMarkers, *_actnElemBoundMarkersFormat;
    std::optional<QPen> _elemBoundMarkersPen;

    void toggleElementBoundMarkers(bool on);
    void formatElementBoundMarkers();
    void updateElementBoundMarkers();
    QCPItemStraightLine* makeElemBoundMarker() const;
    QPen elemBoundMarkersPen() const;

    void handleCursorPanelCommand(const QString& cmd);
};

#endif // MULTI_CAUSTIC_WINDOW_H
