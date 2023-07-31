#ifndef PLOT_FUNC_WINDOW_H
#define PLOT_FUNC_WINDOW_H

#include <QToolButton>

#include "../SchemaWindows.h"
#include "../funcs/PlotFunction.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QSplitter;
QT_END_NAMESPACE

class QCPGraph;

namespace QCPL {
class Cursor;
class CursorPanel;
class Plot;
}

namespace Ori {
namespace Widgets {
class StatusBar;
}}

class FrozenStateButton;
class FunctionGraphSet;
class PlotFunction;
class PlotFuncWindow;
class PlotParamsPanel;
class SchemaStorable;
class UnitsMenu;

enum class ElemDeletionReaction {
    None,
    Close
};

struct CursorInfoValue
{
    QString name;
    double value;
    QString note;

    CursorInfoValue(const QString& name, const double& value): name(name), value(value) {}
    CursorInfoValue(const QString& name, const double& value, const QString& note): name(name), value(value), note(note) {}
    bool isX() const { return name == QStringLiteral("X"); }
    bool isY() const { return name == QStringLiteral("Y"); }
};

using CursorInfoValues = QList<CursorInfoValue>;

class PlotFuncWindow : public SchemaMdiChild
{
    Q_OBJECT

public:
    using FuncMode = int;
    using ViewSettings = QMap<QString, QVariant>;
    enum ViewPart { VP_LIMITS_Y = 0x01, VP_TITLE_Y = 0x02, VP_UNIT_Y = 0x04, VP_CUSRSOR_POS = 0x08 };
    Q_DECLARE_FLAGS(ViewParts, ViewPart)

public:
    explicit PlotFuncWindow(PlotFunction*);
    ~PlotFuncWindow() override;

    PlotFunction* function() const { return _function; }
    QCPL::Plot* plot() const { return _plot; }

    /// Do autolimits after next update.
    void requestAutolimits() { _autolimitsRequest = true; }

    /// Cursor should be ceneterd after next update.
    void requestCenterCursor() { _centerCursorRequested = true; }

    /// Edits function parameters through dialog.
    bool configure();

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return QList<QMenu*>() << menuPlot << menuLimits << menuFormat; }
    QList<ViewMenuItem> menuItems_View() override;
    QString helpTopic() const override { return _function->helpTopic(); }

    // Implementation of SchemaListener
    void recalcRequired(Schema*) override { update(); }
    void elementDeleting(Schema*, Element*) override;

    // Called from FuncOptionsPanel when function mode changed e.g. when the Caustic function switches between W and R.
    // Responsible window should override these methods and save/restore signifacant view differences in _storedView.
    virtual void storeView(FuncMode) {}
    virtual void restoreView(FuncMode) {}

    /// Returns what will happen if one or all the elements are deleted.
    virtual ElemDeletionReaction reactElemDeletion(const Elements&);

    // inherited from IAppSettingsListener
    void optionChanged(AppSettingsOptions option) override;

signals:
    void finishImageBeforeCopy(QPainter*);

public slots:
    void update();

protected slots:
    virtual void updateNotables();
    virtual void updateDataGrid();
    virtual void showRoundTrip();

protected:
    QCPL::Plot* _plot;
    QCPL::Cursor* _cursor;
    QCPL::CursorPanel* _cursorPanel;
    PlotFunction* _function;
    Z::Unit _unitX = Z::Units::none();
    Z::Unit _unitY = Z::Units::none();
    FunctionGraphSet *_graphs;
    PlotParamsPanel* _leftPanel;
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
    QMenu *menuPlot, *menuLimits, *menuFormat;
    QAction *actnShowT, *actnShowS, *actnShowFlippedTS,
        *actnAutolimits, *actnAutolimitsX, *actnAutolimitsY,
        *actnSetLimitsX, *actnSetLimitsY, *actnSetTitleX, *actnSetTitleY,
        *actnZoomIn, *actnZoomOut, *actnZoomInX, *actnZoomOutX, *actnZoomInY, *actnZoomOutY,
        *actnUpdate, *actnUpdateParams, *actnShowRoundTrip, *actnFreeze, *actnFrozenInfo,
        *actnCopyGraphData, *actnCopyGraphDataCur, *actnCopyGraphDataAll, *actnCopyPlotImage,
        *actnCopyGraphDataEx, *actnFormatLegend;

    // Stores differences of plot view when function is switched betweeen modes
    // e.g. when the Caustic function switches between W and R.
    // Should be read/written in methods restoreView/storeView
    QMap<FuncMode, ViewSettings> _storedView;
    void storeViewParts(ViewSettings&, ViewParts);
    void restoreViewParts(const ViewSettings&, ViewParts);

    virtual void calculate();
    virtual bool configureInternal() { return true; }
    virtual void updateGraphs();
    virtual void afterUpdate() {}
    virtual QWidget* makeOptionsPanel() { return nullptr; }
    virtual void fillViewMenuActions(QList<QAction*>& actions) const { Q_UNUSED(actions) }
    virtual void getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values) const { Q_UNUSED(pos) Q_UNUSED(values) }

    QCPGraph* selectedGraph() const;

    void createActions();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();
    void createContent();

    void showModeTS();
    void updateModeTS();
    void updateGraphs(Z::WorkPlane);
    void updateStatusUnits();

    void showStatusError(const QString &message);
    void clearStatusInfo();

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
    void copyPlotImage();
    void copyGraphData();
    void copyGraphDataAll();
    void copyGraphDataEx();
    void formatLegend();

    QWidget* optionsPanelRequired();

private:
    void setUnitX(Z::Unit unit);
    void setUnitY(Z::Unit unit);

    void graphSelected(QCPGraph *);
    void graphsMenuAboutToShow();
    void updateCursorInfo();

    friend class BeamShapeExtension;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PlotFuncWindow::ViewParts)

#endif // PLOT_FUNC_WINDOW_H
