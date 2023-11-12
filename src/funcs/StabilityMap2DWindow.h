#ifndef WINDOW_STABILITY_MAP_2D_H
#define WINDOW_STABILITY_MAP_2D_H

#include "../funcs/PlotFuncWindowStorable.h"
#include "../math/StabilityMap2DFunction.h"
#include "../windows/RezonatorDialog.h"

#include "qcpl_types.h"

class ElemAndParamSelector;
class GeneralRangeEditor;
class QCPColorMap;
class QCPColorScale;

QT_BEGIN_NAMESPACE
class QGroupBox;
QT_END_NAMESPACE

class StabilityMap2DWindow final : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit StabilityMap2DWindow(Schema*);

    StabilityMap2DFunction* function() const { return dynamic_cast<StabilityMap2DFunction*>(_function); }

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    bool configureInternal() override;
    ElemDeletionReaction reactElemDeletion(const Elements&) override;
    void updateGraphs() override;
    Z::Unit getDefaultUnitX() const override;
    Z::Unit getDefaultUnitY() const override;
    void getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values) override;
    QList<BasicMdiChild::MenuItem> editMenuItems() const override { return { _actnCopyGraphData2D }; }
    QList<BasicMdiChild::MenuItem> formatMenuItems() const override { return { _actnFormatColorScale }; }

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;
    QString readWindowSpecific(const QJsonObject& root) override;
    QString writeWindowSpecific(QJsonObject& root) override;

    // Implementation of SchemaListener
    void elementDeleting(Schema*, Element*) override;

private slots:
    void copyGraphData2D();

private:
    QCPColorMap *_graph;
    QCPGraph *_autolimiter;
    QAction *_actnStabilityAutolimits, *_actnCopyGraphData2D, *_actnFormatColorScale;
    QCPColorScale *_colorScale;
    bool _zAutolimitsRequest = true;

    struct ViewState
    {
        QCPL::AxisLimits limitsZ;
        QString titleZ;
    };
    QMap<int, ViewState> _auxStoredView;

    void createContent();
    void createActions();
    void createContextMenus();
    void autolimitsStability(bool replot);
    void pasteColorScaleFormat();
};


/**
    The function arguments dialog that can choose two different parameters
    and set variation of each parameter and the number of points for plotting.
*/
class StabilityMap2DParamsDlg : public RezonatorDialog
{
    Q_OBJECT

    struct VarEditor {
        Z::Variable *var;
        ElemAndParamSelector *elemSelector;
        GeneralRangeEditor *rangeEditor;
        QGroupBox *groupBox;
    };

public:
    explicit StabilityMap2DParamsDlg(Schema*, Z::Variable*, Z::Variable*);

protected slots:
    void collect() override;

private:
    Schema* _schema;
    VarEditor _editor1, _editor2;
    QString _recentKey = "func_stab_map_2d";

    void makeControls(const QString& title, Schema *schema, VarEditor* editor);
    void guessRange(VarEditor* editor);
    void populate();
    void populate(VarEditor* editor);
    void collect(VarEditor* editor, Z::Variable *var);
};

#endif // WINDOW_STABILITY_MAP_2D_H
