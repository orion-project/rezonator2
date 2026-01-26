#ifndef PLOT_FUNC_WINDOW_H
#define PLOT_FUNC_WINDOW_H

#include "../math/PlotFunction.h"
#include "../windows/SchemaWindows.h"

#include <QToolButton>
#include <QPen>

#ifdef Q_OS_LINUX
#include <optional>
#endif

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QSplitter;
QT_END_NAMESPACE

class QCPAxis;
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
class FunctionGraph;
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
    enum ValueKind {
        GENERIC, // no special formatting rules
        VALUE_X, // format as set_x command
        VALUE_Y, // format as set_y command
        RAW,     // don't convert value to Y units
        SECTION, // format as a kind of title for subsequent values
    };
    QString name;
    double value;
    QString note; // a text added after the value of any kind
    ValueKind kind = GENERIC;
    CursorInfoValue(ValueKind kind, const QString& name): name(name), kind(kind) {}
    CursorInfoValue(ValueKind kind, const double& value): value(value), kind(kind) {}
    CursorInfoValue(ValueKind kind, const QString& name, const double& value): name(name), kind(kind), value(value) {}
    CursorInfoValue(const QString& name, const double& value): name(name), value(value) {}
    CursorInfoValue(const QString& name, const double& value, const QString& note): name(name), value(value), note(note) {}
};

using CursorInfoValues = QList<CursorInfoValue>;

/**
    A function window displaying results of calculation of @sa PlotFunction.

    PlotFuncWindow and @sa PlotFunction should be considered obsolete
    and all new plot functions should be implemented via @sa PlotFunctionV2 and @sa PlotFuncWindowV2.
*/
class PlotFuncWindow : public SchemaMdiChild
{
    Q_OBJECT

public:
    using FuncMode = int;
    using ViewSettings = QMap<QString, QVariant>;
    enum ViewPart { VP_LIMITS_Y = 0x01, VP_TITLE_Y = 0x02, VP_UNIT_Y = 0x04, VP_CUSRSOR_POS = 0x08 };
    Q_DECLARE_FLAGS(ViewParts, ViewPart)
    enum SelectGraphOption { SG_UPDATE_CUSROR = 0x01, SG_UPDATE_SPEC_POINTS = 0x02 };
    Q_DECLARE_FLAGS(SelectGraphOptions, SelectGraphOption)

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
    QList<QMenu*> menus() override;
    QList<BasicMdiChild::MenuItem> menuItems_View() override;
    QList<BasicMdiChild::MenuItem> menuItems_Edit() override;
    QString helpTopic() const override { return _function->helpTopic(); }

    // Implementation of SchemaListener
    void recalcRequired(Schema*) override { update(); }
    void elementDeleting(Schema*, Element*) override;
    void globalParamDeleting(Schema*, Z::Parameter*) override;
    void elemParamDeleting(Z::Parameter*) override;

    // Called from FuncOptionsPanel when function mode changed e.g. when the Caustic function switches between W and R.
    // Responsible window should override these methods and save/restore signifacant view differences in _storedView.
    virtual void storeView(FuncMode) {}
    virtual void restoreView(FuncMode) {}

    /// Returns what will happen if one or all the elements are deleted.
    virtual ElemDeletionReaction reactElemDeletion(const Elements&);

    // inherited from IAppSettingsListener
    void optionChanged(AppSettingsOption option) override;

signals:
    void finishImageBeforeCopy(QPainter*);

public slots:
    void update();

protected slots:
    virtual void updateSpecPoints();
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
        *actnSetLimitsX, *actnSetLimitsY, *actnSetTextX, *actnSetTextY, *actnSetTextT,
        *actnZoomIn, *actnZoomOut, *actnZoomInX, *actnZoomOutX, *actnZoomInY, *actnZoomOutY,
        *actnUpdate, *actnUpdateParams, *actnShowRoundTrip, *actnFreeze, *actnFrozenInfo,
        *actnCopyGraphData, *actnCopyGraphDataCurSegment, *actnCopyGraphDataAllSegments, *actnCopyPlotImage,
        *actnCopyGraphDataWithParams, *actnFormatX, *actnFormatY, *actnFormatTitle, *actnFormatLegend,
        *actnToggleTitle, *actnToggleLegend, *actnCopyFormatFromSelection, *actnPasteFormatToSelection,
        *actnCopyPlotFormat, *actnPastePlotFormat, *actnSavePlotFormat, *actnLoadPlotFormat,
        *actnFormatCursor, *actnFormatGraphT, *actnFormatGraphS, *actnFormatGraph;
    SelectGraphOptions _selectGraphOptions;
    std::optional<QPen> _cursorPen, _graphPenT, _graphPenS;

    // Stores differences of plot view when function is switched betweeen modes
    // e.g. when the Caustic function switches between W and R.
    // Should be read/written in methods restoreView/storeView
    QMap<FuncMode, ViewSettings> _storedView;
    void storeViewParts(ViewSettings&, ViewParts);
    void restoreViewParts(const ViewSettings&, ViewParts);

    virtual void calculate();
    virtual bool configureInternal() { return true; }
    virtual void updateGraphs();
    virtual void beforeUpdate() {}
    virtual void afterUpdate() {}
    virtual QWidget* makeOptionsPanel() { return nullptr; }
    virtual QList<BasicMdiChild::MenuItem> viewMenuItems() const { return {}; }
    virtual QList<BasicMdiChild::MenuItem> editMenuItems() const { return {}; }
    virtual QList<BasicMdiChild::MenuItem> formatMenuItems() const { return {}; }
    virtual void getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values) { Q_UNUSED(pos) Q_UNUSED(values) }
    virtual Z::Unit getDefaultUnitX() const { return Z::Units::none(); }
    virtual Z::Unit getDefaultUnitY() const { return Z::Units::none(); }
    virtual Z::Unit getDefaultUnitY(FuncMode mode) const { Q_UNUSED(mode) return getDefaultUnitY(); }
    virtual void prepareSpecPoints() {}
    virtual SpecPointParams getSpecPointsParams() const { return SpecPointParams(); }
    virtual void formatMultiGraph(FunctionGraph*) {}

    QCPGraph* selectedGraph() const;
    QPen cursorPen() const;
    QPen graphPenT() const;
    QPen graphPenS() const;

    void createActions();
    void createMenuBar();
    void createToolBar();
    void createContextMenus();
    void createStatusBar();
    void createContent();

    void showModeTS();
    void updateModeTS();
    void updateGraphs(Z::WorkPlane);
    void updateStatusUnits();
    void updatePlotItemToggleActions();

    void showStatusError(const QString &message);
    void clearStatusInfo();

    void disableAndClose();

    Z::Unit getUnitX() const;
    Z::Unit getUnitY() const;
    Z::Unit getUnitY(FuncMode mode) const;
    
private slots:
    void activateModeT();
    void activateModeS();
    void activateModeFlippedTS();
    void updateWithParams();
    void freeze(bool);
    void copyPlotImage();
    void copyGraphData();
    void copyGraphDataAllSegments();
    void copyGraphDataWithParams();
    void copyFormatFromSelection();
    void pasteFormatToSelection();
    void copyPlotFormat();
    void pastePlotFormat();
    void pasteLegendFormat();
    void pasteTitleFormat();
    void pasteAxisFormat(QCPAxis *axis);
    void legendFormatDlg();
    void toggleTitle();
    void toggleLegend();
    void savePlotFormat();
    void loadPlotFormat();
    void cursorFormatDlg();
    void graphFormatDlgT();
    void graphFormatDlgS();
    void graphFormatDlg();

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
Q_DECLARE_OPERATORS_FOR_FLAGS(PlotFuncWindow::SelectGraphOptions)

#endif // PLOT_FUNC_WINDOW_H
