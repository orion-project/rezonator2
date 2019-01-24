#ifndef MULTI_CAUSTIC_WINDOW_H
#define MULTI_CAUSTIC_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../RezonatorDialog.h"
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
    void fillGraphWithFunctionResults(Z::WorkPlane plane, Graph *graph, int resultIndex) override;
    void afterUpdate() override;
    ElemDeletionReaction reactElemDeletion(const Elements&) override;

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


namespace VariableEditor {
    class MultiElementRangeEd;
}

/**
    The function arguments dialog that can choose several of range elements
    and set the number of points for plotting inside each of selected elements.
*/
class MultiCausticParamsDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit MultiCausticParamsDlg(Schema*, QVector<Z::Variable>&);

private:
    QVector<Z::Variable>& _vars;
    VariableEditor::MultiElementRangeEd *_varEditor;

protected slots:
    void collect();
};

#endif // MULTI_CAUSTIC_WINDOW_H
