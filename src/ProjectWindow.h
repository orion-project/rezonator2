#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <QMainWindow>

#include "AppSettings.h"
#include "MessageBus.h"
#include "SchemaWindows.h"
#include "core/Schema.h"

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
    class Translator;
    class MruFileList;

    namespace Widgets {
        class MruMenu;
        class MdiToolBar;
        class StatusBar;
        class StylesMenu;
        class LanguagesMenu;
    }
}

class ProjectWindow : public QMainWindow,
                      public SchemaToolWindow,
                      public IMessageBusListener
{
    Q_OBJECT

public:
    ProjectWindow(Schema* aSchema);
    ~ProjectWindow() override;

    // inherits from SchemaListener
    void schemaChanged(Schema*) override;
    void schemaLoaded(Schema*) override;
    void schemaSaved(Schema*) override;
    void schemaParamsChanged(Schema*) override;
    void pumpChanged(Schema*, PumpParams*) override;

    // inherited from SettingsListener.
    void settingsChanged() override;

    // inherited from IMessageBusListener
    void messageBusEvent(MessageBusEvent event, const QMap<QString, QVariant>& params) override;

    ProjectOperations* operations() { return _operations; }

protected:
    void closeEvent(class QCloseEvent*) override;

private:
    QMenu *menuFile, *menuEdit, *menuHelp, *menuWindow, *menuTools,
          *menuElement, *menuFunctions, *menuView,
          *menuLangs;

    QAction *actnFileNew, *actnFileOpen, *actnFileExit, *actnFileSave, *actnFileSaveCopy,
            *actnFileSaveAs, *actnFileTripType, *actnFilePump, *actnFileLambda, *actnFileSummary,
            *actnFileOpenExample, *actnFileProps, *actnFilePrint, *actnFilePrintPreview;

    QAction *actnEditCut, *actnEditCopy, *actnEditUndo, *actnEditRedo,
            *actnEditPaste, *actnEditSelectAll;

    QAction *actnFuncRoundTrip, *actnFuncStabMap, *actnFuncStabMap2d,
            *actnFuncRepRate, *actnFuncMultFwd, *actnFuncMultBkwd,
            *actnFuncCaustic, *actnFuncMultirangeCaustic, *actnFuncBeamVariation,
            *actnFuncMultibeamCaustic, *actnFuncBeamParamsAtElems;

    QAction  *actnToolsGaussCalc, *actnToolsCustomElems, *actnToolSettings,
             *actnToolFlipSchema, *actnToolsCalc, *actnToolAdjust,
             *actnToolGrinLens;

    QAction *actnWndClose, *actnWndCloseAll, *actnWndTile, *actnWndCascade,
            *actnWndSchema, *actnWndParams, *actnWndProtocol, *actnWndPumps,
            *actnWndMemos;

    QAction *actnHelpBugReport, *actnHelpUpdates, *actnHelpHomepage, *actnHelpAbout,
            *actnHelpContent, *actnHelpIndex;

    ProjectOperations* _operations;
    CalcManager* _calculations;
    SchemaMdiArea *_mdiArea;

    Ori::Widgets::MruMenu* _mruMenu;
    Ori::Widgets::MdiToolBar* _mdiToolbar;
    Ori::Widgets::LanguagesMenu* _langsMenu;
    Ori::Widgets::StatusBar* _statusBar;
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

    void shortcutEnterActivated();

private slots:
    void showGrinLens();
    void showCustomElems();
    void showSettings();
    void showProtocolWindow();
    void showSchemaWindow();
    void showGaussCalculator();
    void showCalculator();
    void showParamsWindow();
    void showPumpsWindow();
    void showMemosWindow();
    void showAdjustment();
    void showHelp();

    void openSchemaExample();

    void flipSchema();

    /// Slot connected to @c mdiArea::subWindowActivated() signal.
    /// This method is automatically called when mdiArea changes active widget.
    void updateMenuBar();
};

#endif // PROJECT_WINDOW_H
