#ifndef MULTI_CAUSTIC_WINDOW_H
#define MULTI_CAUSTIC_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../funcs/MultirangeCausticFunction.h"

class QCPItemStraightLine;

class MulticausticWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

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

protected:
    // Implementation of PlotFuncWindow
    bool configureInternal() override;
    void afterUpdate() override;
    void fillViewMenuActions(QList<QAction*>& actions) const override;
    void updateGraphs() override;
    ElemDeletionReaction reactElemDeletion(const Elements&) override;
    void showRoundTrip() override;
    Z::Unit getDefaultUnitX() const override;
    Z::Unit getDefaultUnitY() const override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;
    QString readWindowSpecific(const QJsonObject& root) override;
    QString writeWindowSpecific(QJsonObject& root) override;

private:
    QList<QCPItemStraightLine*> _elemBoundMarkers;
    QAction* _actnElemBoundMarkers;

    void toggleElementBoundMarkers(bool on);
    void updateElementBoundMarkers();
    QCPItemStraightLine* makeElemBoundMarker() const;
};

#endif // MULTI_CAUSTIC_WINDOW_H
