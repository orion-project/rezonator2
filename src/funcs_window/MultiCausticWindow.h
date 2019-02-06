#ifndef MULTI_CAUSTIC_WINDOW_H
#define MULTI_CAUSTIC_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../funcs/MultiCausticFunction.h"

class QCPItemStraightLine;

class MultiCausticWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit MultiCausticWindow(Schema*);

    MultiCausticFunction* function() const { return dynamic_cast<MultiCausticFunction*>(_function); }

    // Implementation of SchemaListener
    void elementChanged(Schema*, Element*) override;
    void elementDeleting(Schema*, Element*) override;

    // Implementation of BasicMdiChild
    QList<QAction*> viewActions() override;

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    bool configureInternal() override;
    void fillGraphWithFunctionResults(Z::WorkPlane plane, QCPGraph *graph, int resultIndex) override;
    void afterUpdate() override;
    void afterSetUnitsX(Z::Unit old, Z::Unit cur) override;
    ElemDeletionReaction reactElemDeletion(const Elements&) override;
    QString getDefaultTitle() const override;
    QString getDefaultTitleX() const override;
    QString getDefaultTitleY() const override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;
    QString readWindowSpecific(const QJsonObject& root) override;
    QString writeWindowSpecific(QJsonObject& root) override;

private:
    QList<QCPItemStraightLine*> _elemBoundMarkers;
    QAction* _actnElemBoundMarkers;

    void createActions();

    void updateElementBoundMarkers();
    void toggleElementBoundMarkers(bool on);

    QCPItemStraightLine* makeElemBoundMarker() const;
};

#endif // MULTI_CAUSTIC_WINDOW_H
