#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <QMainWindow>

#include "core/Schema.h"
#include "AppSettings.h"
#include "SchemaWindows.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QToolBar;
QT_END_NAMESPACE

class CalcManager;
class ProjectOperations;
class SchemaMdiArea;
class SchemaViewWindow;

namespace Ori {
    class Styler;
    class Translator;
    class MruFileList;

    namespace Widgets {
        class MruMenu;
        class MdiToolBar;
        class StylesMenu;
        class LanguagesMenu;
    }
}

class ProjectWindow : public QMainWindow, public SchemaToolWindow
{
    Q_OBJECT

public:
    ProjectWindow(Schema* readySchema = nullptr);
    ~ProjectWindow() override;

    // inherits from SchemaListener
    void schemaChanged(Schema*) override;
    void schemaLoaded(Schema*) override;
    void schemaSaved(Schema*) override;
    void schemaParamsChanged(Schema*) override;
    void pumpChanged(Schema*, Z::PumpParams*) override;

    // inherited from SettingsListener.
    void settingsChanged() override;

    ProjectOperations* operations() { return _operations; }

protected:
    void closeEvent(class QCloseEvent*) override;

private:
    QMenu *menuFile, *menuEdit, *menuHelp, *menuWindow, *menuTools,
          *menuElement, *menuFunctions, *menuView,
          *menuLangs, *menuStyles;

    QAction *actnFileNew, *actnFileOpen, *actnFileExit, *actnFileSave, *actnFileSaveCopy,
            *actnFileSaveAs, *actnFileTripType, *actnFilePump, *actnFileLambda, *actnFileSummary,
            *actnFileOpenExample, *actnFileProps;

    QAction *actnEditCut, *actnEditCopy,
            *actnEditPaste, *actnEditSelectAll;

    QAction *actnFuncRoundTrip, *actnFuncStabMap, *actnFuncStabMap2d,
            *actnFuncRepRate, *actnFuncMultFwd, *actnFuncMultBkwd,
            *actnFuncCaustic, *actnFuncMultiCaustic, *actnFuncBeamVariation;

    QAction  *actnToolsGaussCalc, *actnToolsCatalog, *actnToolsPrefs,
             *actnToolFlipSchema;

    QAction *actnWndClose, *actnWndCloseAll, *actnWndTile, *actnWndCascade,
            *actnWndSchema, *actnWndParams, *actnWndProtocol, *actnWndPumps;

    QAction *actnHelpBugReport, *actnHelpUpdates, *actnHelpHomepage, *actnHelpAbout;

    ProjectOperations* _operations;
    CalcManager* _calculations;
    SchemaMdiArea *_mdiArea;

    Ori::Widgets::MruMenu* _mruMenu;
    Ori::Widgets::MdiToolBar* _mdiToolbar;
    Ori::Widgets::StylesMenu* _stylesMenu;
    Ori::Widgets::LanguagesMenu* _langsMenu;
    SchemaViewWindow* _schemaWindow;
    bool _forceClosing = false;
    bool _closingInProgress = false;

    void createActions();
    void createMenuBar();
    void createToolBars();
    void createStatusBar();

    void registerStorableWindows();

    void updateTitle();
    void updateStatusInfo();
    void updateStability();
    void updateActions();

private slots:
    void actionHelpAbout();
    void actionHelpHomePage();
    void actionHelpUpdate();
    void actionHelpBugReport();

    void showElementsCatalog();
    void showPreferences();
    void showProtocolWindow();
    void showSchemaWindow();
    void showGaussCalculator();
    void showParamsWindow();
    void showPumpsWindow();

    void openSchemaExample();

    void flipSchema();

    /// Slot connected to @c mdiArea::subWindowActivated() signal.
    /// This method is automatically called when mdiArea changes active widget.
    void updateMenuBar();
};

#endif // PROJECT_WINDOW_H
