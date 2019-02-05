#ifndef PLOT_FUNC_WINDOW_H
#define PLOT_FUNC_WINDOW_H

#include <QToolButton>

#include "../SchemaWindows.h"
#include "../funcs/PlotFunction.h"
#include "../widgets/PlotUtils.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QSplitter;
QT_END_NAMESPACE

class QCPCursor;
class QCPGraph;
typedef QCPGraph Graph;

namespace Ori {
namespace Widgets {
class StatusBar;
}}

class CursorPanel;
class FrozenStateButton;
class Plot;
class PlotFunction;
class PlotFuncWindow;
class PlotParamsPanel;
class SchemaStorable;
class UnitsMenu;

enum class ElemDeletionReaction {
    None,
    Close
};

class PlotFuncWindow : public SchemaMdiChild
{
    Q_OBJECT

public:
    explicit PlotFuncWindow(PlotFunction*);
    ~PlotFuncWindow() override;

    PlotFunction* function() const { return _function; }
    Plot* plot() const { return _plot; }

    /// Do autolimits after next update.
    void requestAutolimits() { _autolimitsRequest = true; }

    /// Cursor should be ceneterd after next update.
    void requestCenterCursor() { _centerCursorRequested = true; }

    /// Edits function parameters through dialog.
    bool configure();

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return QList<QMenu*>() << menuPlot << menuLimits /* TODO:NEXT-VER << menuFormat*/; }

    // Implementation of SchemaListener
    void recalcRequired(Schema*) override;
    void elementDeleting(Schema*, Element*) override;

    void storeView(int key);
    void restoreView(int key);

    /// Returns what will happen if one or all the elements are deleted.
    virtual ElemDeletionReaction reactElemDeletion(const Elements&);

public slots:
    void update();

protected slots:
    virtual void updateNotables();
    virtual void updateDataGrid();

protected:
    Plot* _plot;
    PlotFunction* _function;
    Z::Unit _unitX = Z::Units::none();
    Z::Unit _unitY = Z::Units::none();
    QVector<Graph*> _graphsT, _graphsS;
    PlotParamsPanel* _leftPanel;
    QCPCursor* _cursor;
    CursorPanel* _cursorPanel;
    QSplitter* _splitter;
    Ori::Widgets::StatusBar* _statusBar;
    FrozenStateButton* _buttonFrozenInfo;
    bool _autolimitsRequest = false; ///< If autolimits requested after next update.
    bool _centerCursorRequested = false; ///< If cursor should be centered after next update.
    bool _needRecalc = false;
    bool _frozen = false;
    UnitsMenu *_unitsMenuX, *_unitsMenuY;
    QMenu *menuPlot, *menuLimits, *menuFormat, *menuAxisX, *menuAxisY;
    QAction *actnShowT, *actnShowS, *actnShowTS,
        *actnAutolimits, *actnAutolimitsX, *actnAutolimitsY,
        *actnSetLimits, *actnSetLimitsX, *actnSetLimitsY,
        *actnZoomIn, *actnZoomOut, *actnZoomInX, *actnZoomOutX, *actnZoomInY, *actnZoomOutY,
        *actnUpdate, *actnUpdateParams, *actnShowRoundTrip, *actnFreeze, *actnFrozenInfo;

    int _windowIndex = 0;
    static QMap<QString, int> _windowIndeces;

    struct ViewState
    {
        AxisLimits limitsX;
        AxisLimits limitsY;
        Z::Unit unitX, unitY;
        QPointF cursorPos;
    };
    QMap<int, ViewState> _storedView;

    /// Calculates function and plots its results.
    virtual void calculate();

    virtual bool configureInternal() { return true; }

    virtual void afterUpdate() {}
    virtual void afterSetUnitsX(Z::Unit old, Z::Unit cur) { Q_UNUSED(old) Q_UNUSED(cur) }
    virtual void afterSetUnitsY(Z::Unit old, Z::Unit cur) { Q_UNUSED(old) Q_UNUSED(cur) }

    Graph* selectedGraph() const;

    void createActions();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();
    void createContent();

    void updateVisibilityTS();
    void updateTSModeActions();
    void updateAxesTitles();
    void updateGraphs(Z::WorkPlane);

    void showInfo(const QString& text, const QString& icon = QString());

    QString displayWindowTitle() const;

    QPen getLineSettings(Z::WorkPlane);

    void debug_LogGraphsCount();

    virtual QWidget* makeOptionsPanel() { return nullptr; }

    virtual void fillGraphWithFunctionResults(Z::WorkPlane plane, Graph *graph, int resultIndex);

    void disableAndClose();

    Z::Unit getUnitX() const;
    Z::Unit getUnitY() const;

private slots:
    void showT();
    void showS();
    void showTS();
    void updateWithParams();
    void graphSelected(Graph*);
    void updateCursorInfo();
    void showRoundTrip();
    void freeze(bool);
    void updateUnitsMenus();
    void updateUnitsMenuX();
    void updateUnitsMenuY();

    QWidget* optionsPanelRequired();

private:

    void setUnitX(Z::Unit unit);
    void setUnitY(Z::Unit unit);
};

#endif // PLOT_FUNC_WINDOW_H
