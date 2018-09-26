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

    // inherits from SchemaListener
    void elementChanged(Schema*, Element*) override;

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    bool configureInternal() override;
    void fillGraphWithFunctionResults(Z::WorkPlane plane, Graph *graph, int resultIndex) override;
    void afterUpdate() override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;

private:
    QList<QCPItemStraightLine*> _elemBoundMarkers;

    void updateElementBoundMarkers();

    QCPItemStraightLine* makeElemBoundMarker() const;
};

#endif // MULTI_CAUSTIC_WINDOW_H
