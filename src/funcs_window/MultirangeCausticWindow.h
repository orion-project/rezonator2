#ifndef MULTI_CAUSTIC_WINDOW_H
#define MULTI_CAUSTIC_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../funcs/MultirangeCausticFunction.h"

class QCPItemStraightLine;

class MultirangeCausticWindow final : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit MultirangeCausticWindow(Schema*);

    MultirangeCausticFunction* function() const { return dynamic_cast<MultirangeCausticFunction*>(_function); }

    // Implementation of SchemaListener
    void schemaRebuilt(Schema*) override;
    void elementChanged(Schema*, Element*) override;
    void elementDeleting(Schema*, Element*) override;

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    bool configureInternal() override;
    void updateGraphs() override;
    void afterUpdate() override;
    ElemDeletionReaction reactElemDeletion(const Elements&) override;
    QString getDefaultTitle() const override;
    QString getDefaultTitleX() const override;
    QString getDefaultTitleY() const override;
    void fillViewMenuActions(QList<QAction*>& actions) const override;

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
