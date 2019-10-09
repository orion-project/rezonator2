#ifndef MULTI_CAUSTIC_WINDOW_H
#define MULTI_CAUSTIC_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../RezonatorDialog.h"
#include "../funcs/MultiCausticFunction.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
QT_END_NAMESPACE

class QCPItemStraightLine;
class MultiElementSelectorWidget;
class PointsRangeEditor;

class MultiCausticWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit MultiCausticWindow(Schema*);

    MultiCausticFunction* function() const { return dynamic_cast<MultiCausticFunction*>(_function); }

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
    void afterSetUnitsX(Z::Unit old, Z::Unit cur) override;
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


/**
    The function arguments dialog that can choose several of range elements
    and set the number of points for plotting inside each of selected elements.
*/
class MultiCausticParamsDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit MultiCausticParamsDlg(Schema*, const QVector<Z::Variable>&);

    const QVector<Z::Variable>& result() const { return _result; }

protected slots:
    void collect() override;

private:
    QVector<Z::Variable> _result;
    MultiElementSelectorWidget *_elemsSelector;
    PointsRangeEditor* _rangeEditor;
    QMap<Element*, Z::VariableRange> _elemRanges;
    QCheckBox *_sameSettings;

    void populate(const QVector<Z::Variable>& vars);

private:
    void currentElementChanged(Element *current, Element *previous);
    void saveEditedRange(Element *elem);
};

#endif // MULTI_CAUSTIC_WINDOW_H
