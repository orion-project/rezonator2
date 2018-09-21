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
    }
}

class ProjectWindow : public QMainWindow, public SchemaToolWindow
{
    Q_OBJECT

public:
    ProjectWindow();
    ~ProjectWindow() override;

    // inherits from SchemaListener
    void schemaChanged(Schema*) override;
    void schemaLoaded(Schema*) override;
    void schemaSaved(Schema*) override;
    void schemaParamsChanged(Schema*) override;
    void pumpChanged(Schema*, Z::PumpParams*) override;

    // inherited from SettingsListener.
    void settingsChanged() override;

protected:
    void closeEvent(class QCloseEvent*) override;

private:
    QMenu *menuFile, /* TODO:NEXT-VER *menuEdit, */ *menuHelp, *menuWindow, *menuTools,
          *menuElement, *menuFunctions, *menuView,
          *menuLangs, *menuStyles;

    QAction *actnFileNew, *actnFileOpen, *actnFileExit, *actnFileSave, *actnFileSaveCopy,
            *actnFileSaveAs, *actnFileTripType, *actnFilePump, *actnFileLambda, *actnFileSummary,
            *actnFileOpenExample;

    /* TODO:NEXT-VER QAction *actnEditCut, *actnEditCopy,
            *actnEditPaste, *actnEditSelectAll; */

    QAction *actnFuncRoundTrip, *actnFuncStabMap, *actnFuncStabMap2d,
            *actnFuncRepRate, *actnFuncMultFwd, *actnFuncMultBkwd,
            *actnFuncCaustic, *actnFuncMultiCaustic;

    QAction  *actnToolsGaussCalc, *actnToolsCatalog, *actnToolsPrefs;

    QAction *actnWndClose, *actnWndCloseAll, *actnWndTile, *actnWndCascade,
            *actnWndSchema, *actnWndParams, *actnWndProtocol, *actnWndPumps;

    QAction *actnHelpBugReport, *actnHelpUpdates, *actnHelpHomepage, *actnHelpAbout;

    ProjectOperations* _operations;
    CalcManager* _calculations;
    SchemaMdiArea *_mdiArea;
    Ori::Styler* _styler;
    Ori::Translator* _translator;
    Ori::Widgets::MruMenu* _mruMenu;
    Ori::Widgets::MdiToolBar* _mdiToolbar;
    Ori::MruFileList *_mruList;
    SchemaViewWindow* _schemaWindow;
    bool _closeEventProcessed = false;

    void createActions();
    void createMenuBar();
    void createToolBars();
    void createStatusBar();

    void registerStorableWindows();

    void updateTitle();
    void updateStatusInfo();
    void updateStability();
    void updateActions();

    void loadSettings();
    void saveSettings();

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

    /// Slot connected to @c mdiArea::subWindowActivated() signal.
    /// This method is automatically called when mdiArea changes active widget.
    void updateMenuBar();
};

#endif // PROJECT_WINDOW_H
