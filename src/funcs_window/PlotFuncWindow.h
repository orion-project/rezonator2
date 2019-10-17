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

namespace Ori {
namespace Widgets {
class StatusBar;
}}

class CursorPanel;
class FrozenStateButton;
class FunctionGraphSet;
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
    virtual QList<ViewMenuItem> viewMenuItems() override;

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
    virtual void showRoundTrip();

protected:
    Plot* _plot;
    PlotFunction* _function;
    Z::Unit _unitX = Z::Units::none();
    Z::Unit _unitY = Z::Units::none();
    QString _title = TitlePlaceholder::defaultTitle();
    QString _titleX = TitlePlaceholder::defaultTitle();
    QString _titleY = TitlePlaceholder::defaultTitle();
    FunctionGraphSet *_graphs;
    PlotParamsPanel* _leftPanel;
    QCPCursor* _cursor;
    CursorPanel* _cursorPanel;
    QMenu* _cursorMenu; // Used for View menu of ProjectWindow
    QSplitter* _splitter;
    Ori::Widgets::StatusBar* _statusBar;
    FrozenStateButton* _buttonFrozenInfo;
    bool _autolimitsRequest = false; ///< If autolimits requested after next update.
    bool _centerCursorRequested = false; ///< If cursor should be centered after next update.
    bool _needRecalc = false;
    bool _frozen = false;
    bool _exclusiveModeTS = false;
    bool _recalcWhenChangeModeTS = false;
    UnitsMenu *_unitsMenuX, *_unitsMenuY;
    QMenu *menuPlot, *menuLimits, *menuFormat, *menuAxisX, *menuAxisY;
    QAction *actnShowT, *actnShowS, *actnShowFlippedTS,
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
        QString title, titleX, titleY;
    };
    QMap<int, ViewState> _storedView;

    virtual void calculate();
    virtual bool configureInternal() { return true; }
    virtual void updateGraphs();
    virtual void afterUpdate() {}
    virtual QString getDefaultTitle() const { return QString(); }
    virtual QString getDefaultTitleX() const { return QString(); }
    virtual QString getDefaultTitleY() const { return QString(); }
    virtual QString formatTitleSpecial(const QString& title) const { return title; }
    virtual void storeViewSpecific(int key) { Q_UNUSED(key) }
    virtual void restoreViewSpecific(int key) { Q_UNUSED(key) }
    virtual QWidget* makeOptionsPanel() { return nullptr; }
    virtual void fillViewMenuActions(QList<QAction*>& actions) const { Q_UNUSED(actions) }

    QCPGraph* selectedGraph() const;

    void createActions();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();
    void createContent();

    void showModeTS();
    void updateModeTS();
    void updateTitles();
    void updateTitle();
    void updateTitleX();
    void updateTitleY();
    void updateGraphs(Z::WorkPlane);
    void updateStatusUnits();

    void showStatusError(const QString &message);
    void clearStatusInfo();

    QString displayWindowTitle() const;

    void disableAndClose();

    Z::Unit getUnitX() const;
    Z::Unit getUnitY() const;
    virtual Z::Unit getDefaultUnitX() const { return Z::Units::none(); }
    virtual Z::Unit getDefaultUnitY() const { return Z::Units::none(); }

private slots:
    void activateModeT();
    void activateModeS();
    void activateModeFlippedTS();
    void updateWithParams();
    void freeze(bool);

    QWidget* optionsPanelRequired();

private:
    void setUnitX(Z::Unit unit);
    void setUnitY(Z::Unit unit);

    void graphSelected(QCPGraph *);
    void updateCursorInfo();
};

#endif // PLOT_FUNC_WINDOW_H
