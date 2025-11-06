#ifndef PLOT_BASE_WINDOW_H
#define PLOT_BASE_WINDOW_H

#include "SchemaWindows.h"

class QSplitter;

class QCPAxis;
class QCPGraph;

namespace QCPL {
class Cursor;
class CursorPanel;
class Plot;
}

namespace Ori::Widgets {
class StatusBar;
}

struct PlotCursorInfo;

class PlotParamsPanel;

/**
    Base window providing plotting widgent and basic commands for plot manipulation.
    Intended to be used as a base class for particular plotting windows
    @sa PlotWindow, @sa PlotFuncWindow
*/
class PlotBaseWindow : public SchemaMdiChild
{
    Q_OBJECT

public:
    // inherits from BasicMdiChild
    QList<QMenu*> menus() override;
    QList<BasicMdiChild::MenuItem> menuItems_View() override;
    QList<BasicMdiChild::MenuItem> menuItems_Edit() override;

    // inherited from IAppSettingsListener
    void optionChanged(AppSettingsOption option) override;

signals:
    void finishImageBeforeCopy(QPainter*);
    
protected:
    struct WndConfig
    {
        int statusPanelCount;
        int statusPanelPoints;
        bool leftPanelHasSpecPoints;
        bool leftPanelHasDataTable;
        bool leftPanelHasOptions;
        bool selectedGraphUpdatesCursor;
        
        WndConfig();
    };

    WndConfig _config;
    QCPL::Plot* _plot;
    QSplitter* _splitter;
    PlotParamsPanel* _leftPanel;
    QCPL::Cursor* _cursor;
    QCPL::CursorPanel* _cursorPanel;
    QMenu* _cursorMenu; // Used for View menu of ProjectWindow
    QMenu *menuPlot, *menuLimits, *menuFormat;
    Ori::Widgets::StatusBar* _statusBar;
    QAction *actnAutolimits, *actnAutolimitsX, *actnAutolimitsY,
        *actnZoomIn, *actnZoomOut, *actnZoomInX, *actnZoomOutX, *actnZoomInY, *actnZoomOutY,
        *actnSetLimitsX, *actnSetLimitsY, *actnSetTextX, *actnSetTextY, *actnSetTextT,
        *actnFormatTitle, *actnFormatLegend, *actnFormatX, *actnFormatY,
        *actnFormatCursor, *actnFormatGraph,
        *actnSavePlotFormat, *actnLoadPlotFormat,
        *actnCopyGraphData, *actnCopyGraphDataCurSegment, *actnCopyGraphDataAllSegments,
        *actnCopyGraphDataWithParams, *actnCopyPlotImage, *actnCopyPlotFormat, *actnPastePlotFormat,
        *actnToggleTitle, *actnToggleLegend;
    std::optional<QPen> _cursorPen;
    
    explicit PlotBaseWindow(Schema*, const WndConfig &config = {});

    virtual void graphSelected(QCPGraph*);
    virtual void fillCursorInfo(PlotCursorInfo&) {}
    virtual QList<BasicMdiChild::MenuItem> viewMenuItems() const { return {}; }
    virtual QList<BasicMdiChild::MenuItem> editMenuItems() const { return {}; }
    virtual QList<BasicMdiChild::MenuItem> formatMenuItems() const { return {}; }
    
    QCPGraph* selectedGraph() const;
    QPen cursorPen() const;

    void updateCursorInfo();
    void updateDataGrid();
    void updatePlotItemToggleActions();
    
private slots:
    void toggleTitle();
    void toggleLegend();
    void savePlotFormat();
    void loadPlotFormat();
    void copyPlotImage();
    void copyPlotFormat();
    void pastePlotFormat();
    void pasteLegendFormat();
    void pasteTitleFormat();
    void pasteAxisFormat(QCPAxis *axis);
    void cursorFormatDlg();
    void graphFormatDlg();
    void copyGraphData();
    void copyGraphDataAllSegments();
    void copyGraphDataWithParams();
    
private:
    void createContent();
    void createActions();
    void createMenuBar();
    void createToolBar();
    void createContextMenus();
    void createStatusBar();
    
    void graphsMenuAboutToShow();
};

#endif // PLOT_BASE_WINDOW_H