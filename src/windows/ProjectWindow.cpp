#include "ProjectWindow.h"

#include "../app/Appearance.h"
#include "../app/CalcManager.h"
#include "../app/HelpSystem.h"
#include "../app/ProjectOperations.h"
#include "../app/PersistentState.h"
#include "../core/Format.h"
#include "../math/RoundTripCalculator.h"
#include "../tools/CalculatorWindow.h"
#include "../tools/GaussCalculatorWindow.h"
#include "../tools/GrinLensWindow.h"
#include "../tools/IrisWindow.h"
#include "../tools/LensmakerWindow.h"
#include "AdjustmentWindow.h"
#include "CustomElemsWindow.h"
#include "ElemFormulaWindow.h"
#include "FuncEditorWindow.h"
#include "MemoWindow.h"
#include "ProtocolWindow.h"
#include "PumpWindow.h"
#include "SchemaParamsWindow.h"
#include "SchemaViewWindow.h"
#include "WindowsManager.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriWindows.h"
#include "tools/OriMruList.h"
#include "widgets/OriMruMenu.h"
#include "widgets/OriMdiToolBar.h"
#include "widgets/OriStatusBar.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QTimer>
#include <QToolButton>
#include <QShortcut>

enum ProjectWindowStatusPanels
{
    STATUS_ELEMS,
    STATUS_MODIF,
    STATUS_LAMBDA,
    STATUS_TRIPTYPE,
    STATUS_STABIL,
    STATUS_FILE,

    STATUS_PANELS_COUNT,
};
static const ProjectWindowStatusPanels STATUS_PUMP = STATUS_STABIL;

void ProjectWindow::createProject(TripType tripType)
{
    (new ProjectWindow(ProjectOperations::createDefaultSchema(tripType)))->show();
}

void ProjectWindow::openProject(const QString& fileName, const OpenProjectArgs& args)
{
    auto wnd = new ProjectWindow(new Schema());
    wnd->show();
    wnd->operations()->openSchemaFile(fileName, {.isExample = args.isExample, .addToMru = args.addToMru});
}

ProjectWindow::ProjectWindow(Schema* aSchema) : QMainWindow(), SchemaToolWindow(aSchema)
{
    setAttribute(Qt::WA_DeleteOnClose);
    Ori::Wnd::setWindowIcon(this, ":/window_icons/main");

    _mruList = createMruList(this);

    _calculations = new CalcManager(schema(), this);
    _operations = new ProjectOperations(schema(), this, _calculations, _mruList);

    Z::HelpSystem::instance()->setParent(this);

    _schemaWindow = new SchemaViewWindow(schema(), _calculations);

    _mdiArea = new SchemaMdiArea;
    _mdiArea->appendChild(_schemaWindow);
    setCentralWidget(_mdiArea);

    connect(_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenuBar()));
    connect(WindowsManager::instancePtr(), SIGNAL(showMdiSubWindow(QWidget*)), _mdiArea, SLOT(appendChild(QWidget*)));
    connect(_operations, &ProjectOperations::protocolRequired, this, &ProjectWindow::showProtocolWindow);

    createActions();
    createMenuBar();
    createToolBars();
    createStatusBar();

    updateTitle();
    updateActions();
    updateMenuBar();
    updateStatusInfo();
    updateStability();

    _mdiToolbar->subWindowActivated(_schemaWindow);

    registerStorableWindows();

    if (AppSettings::instance().showProtocolAtStart)
        showProtocolWindow();

    // This allows Enter shortcut for tool windows (see IShortcutListener).
    auto shortcutApply = new QShortcut(Qt::Key_Return, this);
    connect(shortcutApply, &QShortcut::activated, this, &ProjectWindow::shortcutEnterActivated);

    MessageBus::instance().registerListener(this);

    PersistentState::restoreWindowGeometry("main", this, {1024, 768});
}

ProjectWindow::~ProjectWindow()
{
    PersistentState::storeWindowGeometry("main", this);
}

void ProjectWindow::registerStorableWindows()
{
    WindowsManager::registerConstructor(SchemaParamsWindowStorable::windowType(), SchemaParamsWindowStorable::createWindow);
    WindowsManager::registerConstructor(PumpWindowStorable::windowType(), PumpWindowStorable::createWindow);
    WindowsManager::registerConstructor(ElemFormulaWindowStorable::windowType(), ElemFormulaWindowStorable::createWindow);
    WindowsManager::registerConstructor(MemoWindowStorable::windowType(), MemoWindowStorable::createWindow);
    WindowsManager::registerConstructor(FuncEditorWindowStorable::windowType(), FuncEditorWindowStorable::createWindow);
}

void ProjectWindow::createActions()
{
    #define A_ Ori::Gui::V0::action

    actnFileNew = A_(tr("New"), _operations, SLOT(newSchemaFile()), ":/toolbar/schema_new", QKeySequence::New);
    actnFileOpen = A_(tr("Open..."), _operations, SLOT(openSchemaFile()), ":/toolbar/schema_open", QKeySequence::Open);
    actnFileOpenExample = A_(tr("Open Example..."), _operations, SLOT(openExampleFile()), ":/toolbar/schema_sample");
    actnFileSave = A_(tr("Save"), _operations, SLOT(saveSchemaFile()), ":/toolbar/save", QKeySequence::Save);
    actnFileSaveAs = A_(tr("Save As..."), _operations, SLOT(saveSchemaFileAs()), nullptr, QKeySequence::SaveAs);
    actnFileSaveCopy = A_(tr("Save Copy..."), _operations, SLOT(saveSchemaFileCopy()));
    actnFileLambda = A_(tr("Change Wavelength..."), _operations, SLOT(setupWavelength()), ":/toolbar/wavelength", Qt::Key_F10);
    actnFileTripType = A_(tr("Change Trip Type..."), _operations, SLOT(setupTripType()));
    actnFilePump = A_(tr("Setup Input Beam..."), _operations, SLOT(setupPump()), ":/toolbar/pump_edit", Qt::Key_F9);
    actnFileSummary = A_(tr("Summary..."), _calculations, SLOT(funcSummary()), ":/toolbar/schema_summary");
    actnFileProps = A_(tr("Properties..."), _operations, SLOT(editSchemaProps()), ":/toolbar/schema_prop");
    actnFileExit = A_(tr("Exit"), qApp, SLOT(closeAllWindows()), nullptr, Qt::CTRL | Qt::Key_Q);
    actnFilePrint = A_(tr("Print..."), _mdiArea, SLOT(printableChild_SendToPrinter()), ":/toolbar/print");
    actnFilePrintPreview = A_(tr("Print Preview..."), _mdiArea, SLOT(printableChild_PrintPreview()), nullptr);

    actnEditUndo = A_(tr("Undo"), _mdiArea, SLOT(editableChild_Undo()), ":/toolbar/undo", QKeySequence::Undo);
    actnEditRedo = A_(tr("Redo"), _mdiArea, SLOT(editableChild_Redo()), ":/toolbar/redo", QKeySequence::Redo);
    actnEditCut = A_(tr("Cut"), _mdiArea, SLOT(editableChild_Cut()), ":/toolbar/cut", QKeySequence::Cut);
    actnEditCopy = A_(tr("Copy"), _mdiArea, SLOT(editableChild_Copy()), ":/toolbar/copy", QKeySequence::Copy);
    actnEditPaste = A_(tr("Paste"), _mdiArea, SLOT(editableChild_Paste()), ":/toolbar/paste", QKeySequence::Paste);
    actnEditSelectAll = A_(tr("Select All"), _mdiArea, SLOT(editableChild_SelectAll()), nullptr, QKeySequence::SelectAll);

    actnFuncRoundTrip = A_(tr("Round-trip Matrix"), _calculations, SLOT(funcRoundTrip()), ":/toolbar/func_round_trip");
    actnFuncMatrixMult = A_(tr("Multiply Selected"), _calculations, SLOT(funcMatrixMult()));
    actnFuncStabMap = A_(tr("Stability Map..."), _calculations, SLOT(funcStabMap()), ":/toolbar/func_stab_map");
    actnFuncStabMap2d = A_(tr("2D Stability Map..."), _calculations, SLOT(funcStabMap2d()), ":/toolbar/func_stab_map_2d");
    actnFuncRepRate = A_(tr("Intermode Beats Frequency"), _calculations, SLOT(funcRepRate()), ":/toolbar/func_reprate");
    actnFuncCaustic = A_(tr("Caustic..."), _calculations, SLOT(funcCaustic()), ":/toolbar/func_caustic");
    actnFuncMultirangeCaustic = A_(tr("Multirange Caustic..."), _calculations, SLOT(funcMultirangeCaustic()), ":/toolbar/func_multi_caustic");
    actnFuncMultibeamCaustic = A_(tr("Multibeam Caustic..."), _calculations, SLOT(funcMultibeamCaustic()), ":/toolbar/func_multi_beam_caustic");
    actnFuncBeamVariation = A_(tr("Beamsize Variation..."), _calculations, SLOT(funcBeamVariation()), ":/toolbar/func_beam_variation");
    actnFuncBeamParamsAtElems = A_(tr("Beam Parameters at Elemens"), _calculations, SLOT(funcBeamParamsAtElems()), ":/toolbar/func_beamdata");
    actnFuncCustomInfo = A_(tr("Create Custom Info Function"), this, SLOT(showFuncEditor()));
    actnFuncCustomTable = A_(tr("Create Custom Table Function"), _calculations, SLOT(funcCustomTable()));

    actnToolsCustomElems = A_(tr("Custom Elements Library"), this, SLOT(showCustomElems()), ":/toolbar/catalog");
    actnToolsGaussCalc = A_(tr("Gaussian Beam Calculator"), this, SLOT(showGaussCalculator()), ":/toolbar/gauss_calculator");
    actnToolsCalc = A_(tr("Formula Calculator"), this, SLOT(showCalculator()), ":/window_icons/calculator");
    actnToolFlipSchema = A_(tr("Flip Schema..."), this, SLOT(flipSchema()));
    actnSettings = A_(tr("Settings..."), this, SLOT(showSettings()), ":/toolbar/settings");
    actnToolAdjust = A_(tr("Adjustment"), this, SLOT(showAdjustment()), ":/toolbar/adjust");
    actnToolGrinLens = A_(tr("GRIN Lens Assessment"), this, SLOT(showGrinLens()), ":/toolbar/grin");
    actnToolLensmaker = A_(tr("Lensmaker"), this, SLOT(showLensmaker()), ":/window_icons/lens");
    actnToolIris = A_(tr("Iris"), this, SLOT(showIris()), ":/toolbar/iris");

    // These common window actions must not have data (action->data()), as data presense indicates that
    // this action is for activation of specific subwindow and _mdiArea is responsible for it.
    actnWndSchema = A_(tr("Schema"), this, SLOT(showSchemaWindow()), ":/toolbar/schema", Qt::Key_F12);
    actnWndParams = A_(tr("Parameters"), this, SLOT(showParamsWindow()), ":/toolbar/parameter", Qt::Key_F11);
    actnWndPumps = A_(tr("Pumps"), this, SLOT(showPumpsWindow()), ":/toolbar/pumps");
    actnWndMemos = A_(tr("Memos"), this, SLOT(showMemosWindow()), ":/toolbar/notepad");
    actnWndProtocol = A_(tr("Protocol"), this, SLOT(showProtocolWindow()), ":/toolbar/protocol");
    actnWndClose = A_(tr("Close"), _mdiArea, SLOT(closeActiveSubWindow()));
    actnWndCloseAll = A_(tr("Close All"), _mdiArea, SLOT(closeAllSubWindows()), ":/toolbar/windows_close");
    actnWndTile = A_(tr("Tile"), _mdiArea, SLOT(tileSubWindows()));
    actnWndCascade = A_(tr("Cascade"), _mdiArea, SLOT(cascadeSubWindows()));

    auto help = Z::HelpSystem::instance();
    actnHelpContent = A_(tr("Contents"), this, SLOT(showHelp()), ":/toolbar/help", QKeySequence::HelpContents);
    actnHelpIndex = A_(tr("Index"), help, SLOT(showIndex()));
    actnHelpBugReport = A_(tr("Send Bug Report"), help, SLOT(sendBugReport()), ":/toolbar/bug");
    actnHelpUpdates = A_(tr("Check for Updates"), help, SLOT(checkUpdates()), ":/toolbar/update");
    actnHelpHomepage = A_(tr("Visit Homepage"), help, SLOT(visitHomePage()), ":/toolbar/home");
    actnHelpAbout = A_(tr("About..."), help, SLOT(showAbout()));

    #undef A_
}

void ProjectWindow::createMenuBar()
{
    menuBar()->setNativeMenuBar(AppSettings::instance().useNativeMenuBar);

    _mruMenu = new Ori::Widgets::MruMenu(tr("Recent Files"), _mruList, this);

    menuFile = Ori::Gui::menu(tr("File"), this,
        { actnFileNew, actnFileOpen, actnFileOpenExample, _mruMenu, nullptr, actnFileSave,
          actnFileSaveAs, actnFileSaveCopy, nullptr, actnFilePrint, actnFilePrintPreview, nullptr,
          actnFileProps, actnFileTripType, actnFileLambda, actnFilePump, actnFileSummary, nullptr, actnFileExit });

    menuEdit = new QMenu(tr("Edit"), this);

    menuView = new QMenu(tr("View"), this);

    menuFunctions = Ori::Gui::menu(tr("Functions"), this,
        { actnFuncRoundTrip, actnFuncMatrixMult, nullptr,
          actnFuncStabMap, actnFuncStabMap2d, actnFuncBeamVariation, nullptr,
          actnFuncCaustic, actnFuncMultirangeCaustic, actnFuncMultibeamCaustic,
          actnFuncBeamParamsAtElems, nullptr, actnFuncRepRate, nullptr,
          actnFuncCustomInfo, actnFuncCustomTable });

    menuUtils = Ori::Gui::menu(tr("Utils", "Menu title"), this,
        { actnToolFlipSchema, nullptr, actnToolAdjust });

    menuTools = Ori::Gui::menu(tr("Tools", "Menu title"), this,
        { actnToolsGaussCalc, actnToolsCalc, actnToolsCustomElems, actnToolGrinLens, actnToolLensmaker,
          //actnToolIris
        });
    if (AppSettings::instance().isDevMode)
        menuTools->addAction(QIcon(":/toolbar/palette"), tr("Edit App Style Sheet"), this, []{ Z::Gui::editAppStyleSheet(); });

    menuWindow = Ori::Gui::menu(tr("Window"), this,
        { actnWndSchema, actnWndParams, actnWndPumps, actnWndProtocol, actnWndMemos, nullptr,
          actnWndClose, actnWndCloseAll, nullptr, actnWndTile, actnWndCascade, nullptr });
    connect(menuWindow, SIGNAL(aboutToShow()), _mdiArea, SLOT(populateWindowMenu()));

    menuHelp = Ori::Gui::menu(tr("Help"), this,
        { actnHelpContent, actnHelpIndex, nullptr,
          actnHelpBugReport, actnHelpUpdates, actnHelpHomepage, nullptr,
          actnHelpAbout });
}

void ProjectWindow::createToolBars()
{
    addToolBar(makeToolBar({
        actnFileNew, Ori::Gui::menuToolButton(_mruMenu, actnFileOpen),
        actnFileSave, nullptr, actnFileProps, actnFilePump, actnFileSummary , nullptr,
        actnEditCut, actnEditCopy, actnEditPaste, nullptr, actnFuncRoundTrip, nullptr,
        actnFuncStabMap, actnFuncStabMap2d, actnFuncBeamVariation, nullptr,
        actnFuncCaustic, actnFuncMultirangeCaustic, actnFuncMultibeamCaustic, actnFuncBeamParamsAtElems, nullptr,
        actnFuncRepRate, nullptr, actnWndParams, actnWndPumps, actnWndMemos, nullptr, actnToolAdjust, nullptr,
        actnToolsGaussCalc, actnToolsCalc, actnToolGrinLens, //actnToolIris
    }, true));

    _mdiToolbar = new Ori::Widgets::MdiToolBar(tr("Windows"), _mdiArea);
    _mdiToolbar->setMovable(false);
    _mdiToolbar->setFloatable(false);
    Z::WindowUtils::adjustIconSize(_mdiToolbar);
    addToolBar(Qt::BottomToolBarArea, _mdiToolbar);
}

void ProjectWindow::createStatusBar()
{
    _statusBar = new Ori::Widgets::StatusBar(STATUS_PANELS_COUNT);
    _statusBar->connect(STATUS_LAMBDA, SIGNAL(doubleClicked()), _operations, SLOT(setupWavelength()));
    _statusBar->connect(STATUS_TRIPTYPE, SIGNAL(doubleClicked()), _operations, SLOT(setupTripType()));
    _statusBar->connect(STATUS_PUMP, SIGNAL(doubleClicked()), _operations, SLOT(setupPump()));

    auto versionLabel = new QLabel(Z::Strs::appVersion());
    versionLabel->setContentsMargins(3, 0, 3, 0);
    versionLabel->setForegroundRole(QPalette::Mid);
    _statusBar->addPermanentWidget(versionLabel);

    setStatusBar(_statusBar);
}

Ori::MruList* ProjectWindow::createMruList(QObject* parent)
{
    auto mru = new Ori::MruFileList(parent);
    mru->setAutoSave(false);
    mru->setItems(AppSettings::instance().loadMruItems());
    mru->setMaxCount(AppSettings::instance().mruSchemaCount);
    mru->connect(mru, &Ori::MruList::saveRequired, [mru]{
        AppSettings::instance().saveMruItems(mru->items());
    });
    return mru;
}

void ProjectWindow::updateTitle()
{
    if (!schema()->title().isEmpty())
        Ori::Wnd::setWindowProject(this, schema()->title());
    else
        Ori::Wnd::setWindowFilePath(this, schema()->fileName());
}

void ProjectWindow::addEditAction(QAction* action, IEditableWindow* wnd, IEditableWindow::SupportedCommand cmd)
{
    bool on = wnd && wnd->supportedCommands().testFlag(cmd);
    action->setEnabled(on);
    action->setVisible(on);
    if (on)
        menuEdit->addAction(action);
}

void ProjectWindow::updateMenuBar()
{
    BasicMdiChild* child = _mdiArea->activeChild();

    // Update Edit menu
    menuEdit->clear();
    IEditableWindow* editable = _mdiArea->activeEditableChild();
    addEditAction(actnEditUndo, editable, IEditableWindow::EditCmd_Undo);
    addEditAction(actnEditRedo, editable, IEditableWindow::EditCmd_Redo);
    if (actnEditUndo->isVisible() || actnEditRedo->isVisible())
        menuEdit->addSeparator();
    addEditAction(actnEditCut, editable, IEditableWindow::EditCmd_Cut);
    addEditAction(actnEditCopy, editable, IEditableWindow::EditCmd_Copy);
    addEditAction(actnEditPaste, editable, IEditableWindow::EditCmd_Paste);
    if (actnEditCut->isVisible() || actnEditCopy->isVisible() || actnEditPaste->isVisible())
        menuEdit->addSeparator();
    addEditAction(actnEditSelectAll, editable, IEditableWindow::EditCmd_SelectAll);
    if (actnEditSelectAll->isVisible())
        menuEdit->addSeparator();
    if (child)
        for (auto& item : child->menuItems_Edit())
            item.addTo(menuEdit);
    menuEdit->addSeparator();
    menuEdit->addAction(actnSettings);

    IPrintableWindow* printable = dynamic_cast<IPrintableWindow*>(child);
    actnFilePrint->setEnabled(printable);
    actnFilePrintPreview->setEnabled(printable);

    // Update View menu
    menuView->clear();
    int viewActionsCount = 0;
    if (child)
        for (auto& item : child->menuItems_View())
        {
            item.addTo(menuView);
            viewActionsCount++;
        }
    //menuView->addMenu(_langsMenu); TODO: move to settings

    // Update menu bar
    QMenuBar* menuBar = this->menuBar();
    menuBar->clear();
    menuBar->addMenu(menuFile);
    menuBar->addMenu(menuEdit);
    if (viewActionsCount > 0)
        menuBar->addMenu(menuView);
    menuBar->addMenu(menuFunctions);
    if (child) {
        const auto menus = child->menus();
        for (QMenu* menu : std::as_const(menus)) 
            menuBar->addMenu(menu);
    }
    menuBar->addMenu(menuUtils);
    menuBar->addMenu(menuTools);
    menuBar->addMenu(menuWindow);
    menuBar->addMenu(menuHelp);
}

void ProjectWindow::updateActions()
{
    bool isSchemaSP = schema()->isSP();

    actnFileTripType->setIcon(QIcon(TripTypes::info(schema()->tripType()).iconPath()));
    actnFilePump->setEnabled(isSchemaSP);
    actnFilePump->setVisible(isSchemaSP);
    actnFuncStabMap->setVisible(!isSchemaSP);
    actnFuncStabMap2d->setVisible(!isSchemaSP);
    actnFuncMultibeamCaustic->setVisible(isSchemaSP);
    actnFileSave->setEnabled(schema()->modified());
    if (schema()->memo)
        actnWndMemos->setIcon(QIcon(":/toolbar/notepad_1"));
}

void ProjectWindow::updateStatusInfo()
{
    int totalCount = schema()->count();
    int enabledCount = schema()->activeCount();
    if (totalCount != enabledCount)
        _statusBar->setText(STATUS_ELEMS, tr("Elements: %1 (%2)",
                                         "Status text").arg(enabledCount).arg(totalCount));
    else _statusBar->setText(STATUS_ELEMS, tr("Elements: %1", "Status text").arg(totalCount));

    if (!schema()->modified()) _statusBar->clear(STATUS_MODIF);
    else _statusBar->setText(STATUS_MODIF, tr("Modified", "Status text"));

    _statusBar->setText(STATUS_LAMBDA, schema()->wavelength().displayStr());

    auto tripTypeInfo = TripTypes::info(schema()->tripType());
    _statusBar->setIcon(STATUS_TRIPTYPE, tripTypeInfo.iconPath());
    _statusBar->setToolTip(STATUS_TRIPTYPE, tripTypeInfo.toolTip());

    if (schema()->isSP())
    {
        QString pumpHint, pumpIcon;
        auto pump = schema()->activePump();
        if (pump)
        {
            pumpHint = pump->displayStr();
            auto pumpMode = Pumps::findByModeName(pump->modeName());
            if (pumpMode)
                pumpIcon = pumpMode->iconPath();
        }
        _statusBar->setIcon(STATUS_PUMP, pumpIcon);
        _statusBar->setToolTip(STATUS_PUMP, pumpHint);
    }

    if (schema()->fileName().isEmpty()) _statusBar->clear(STATUS_FILE);
    else _statusBar->setText(STATUS_FILE, schema()->fileName());
}

void ProjectWindow::updateStability()
{
    if (!schema()->isResonator()) return;
    auto stable = Calc::isStable(schema());
    QString icon, hint;
    if (!stable.T && !stable.S)
    {
        icon = QStringLiteral(":/icons/stability_err");
        hint = tr("System is unstable", "Status text");
    }
    else if (!stable.T)
    {
        icon = QStringLiteral(":/icons/stability_err");
        hint = tr("System is unstable in T-plane", "Status text");
    }
    else if (!stable.S)
    {
        icon = QStringLiteral(":/icons/stability_err");
        hint = tr("System is unstable in S-plane", "Status text");
    }
    else
    {
        icon = QStringLiteral(":/icons/stability_ok");
        hint = tr("System is stable", "Status text");
    }
    _statusBar->setIcon(STATUS_STABIL, icon);
    _statusBar->setToolTip(STATUS_STABIL, hint);
}

void ProjectWindow::closeEvent(QCloseEvent* ce)
{
    QMainWindow::closeEvent(ce);

    // Workaround: on MacOS this function is called twice for some reason,
    // but we want to process it only once. This solution can handle it.

    if (_forceClosing) {
        ce->accept();
        return;
    }

    ce->ignore();

    if (_closingInProgress) return;

    _closingInProgress = true;
    QTimer::singleShot(0, this, [&](){
        if (_operations->canClose()) {
            _forceClosing = true;
            close();
        }
        _closingInProgress = false;
    });
}

void ProjectWindow::settingsChanged()
{
    SchemaToolWindow::settingsChanged();
    Z::WindowUtils::adjustIconSize(_mdiToolbar);
}

//------------------------------------------------------------------------------
//                             Tools actions

void ProjectWindow::showCustomElems()
{
    CustomElemsWindow::showWindow();
}

void ProjectWindow::showSettings()
{
    AppSettings::instance().edit();
}

void ProjectWindow::showGaussCalculator()
{
    GaussCalculatorWindow::showWindow();
}

void ProjectWindow::showCalculator()
{
    CalculatorWindow::showWindow();
}

void ProjectWindow::showGrinLens()
{
    GrinLensWindow::showWindow();
}

void ProjectWindow::flipSchema()
{
    if (Ori::Dlg::yes(tr("Do you want to rearrange elements in the opposite order?")))
        schema()->flip();
}

void ProjectWindow::showAdjustment()
{
    AdjustmentWindow::adjust(schema(), nullptr);
}

void ProjectWindow::showLensmaker()
{
    LensmakerWindow::showWindow();
}

void ProjectWindow::showIris()
{
    IrisWindow::showWindow();
}

//------------------------------------------------------------------------------
//                               Window actions

void ProjectWindow::showProtocolWindow()
{
    _mdiArea->appendChild(ProtocolWindow::create());
}

void ProjectWindow::showSchemaWindow()
{
    _mdiArea->activateChild(_schemaWindow);
}

void ProjectWindow::showParamsWindow()
{
    _mdiArea->appendChild(SchemaParamsWindow::create(schema()));
}

void ProjectWindow::showPumpsWindow()
{
    _mdiArea->appendChild(PumpWindow::create(schema()));
}

void ProjectWindow::showMemosWindow()
{
    _mdiArea->appendChild(MemoWindow::create(schema()));
}

void ProjectWindow::showFuncEditor()
{
    _mdiArea->appendChild(FuncEditorWindow::create(schema()));
    schema()->markModified("Custom func added");
}

//------------------------------------------------------------------------------
//                               Help actions

void ProjectWindow::showHelp()
{
    auto activeChild = _mdiArea->activeChild();
    if (activeChild)
    {
        QString helpTopic = activeChild->helpTopic();
        if (!helpTopic.isEmpty())
        {
            Z::HelpSystem::instance()->showTopic(helpTopic);
            return;
        }
    }
    Z::HelpSystem::instance()->showContents();
}

//------------------------------------------------------------------------------
//                               Schema events

void ProjectWindow::pumpChanged(Schema *s, PumpParams *pump)
{
    if (pump->isActive() && s->isSP())
        updateStatusInfo();
}

void ProjectWindow::schemaParamsChanged(Schema*)
{
    updateTitle();
    updateActions();
    updateStatusInfo();
}

void ProjectWindow::schemaChanged(Schema* s)
{
    actnFileSave->setEnabled(s->state().isModified());
    setWindowModified(s->state().isModified());

    updateStatusInfo();
    updateTitle();
    updateStability();
}

void ProjectWindow::schemaLoaded(Schema*)
{
    updateStatusInfo();
    updateTitle();
    updateStability();
    updateActions();
}

void ProjectWindow::schemaSaved(Schema*)
{
    actnFileSave->setEnabled(false);

    updateStatusInfo();
    updateTitle();
}

void ProjectWindow::shortcutEnterActivated()
{
    auto shortcutListener = dynamic_cast<IShortcutListener*>(qApp->activeWindow());
    if (!shortcutListener)
        shortcutListener = dynamic_cast<IShortcutListener*>(_mdiArea->activeChild());
    if (shortcutListener)
        shortcutListener->shortcutEnterPressed();
}

void ProjectWindow::messageBusEvent(MessageBusEvent event, const QMap<QString, QVariant> &params)
{
    Q_UNUSED(params)
    switch (event) {
    case MBE_MEMO_ADDED:
        actnWndMemos->setIcon(QIcon(":/toolbar/notepad_1"));
        break;
    case MBE_MEMO_REMOVED:
        actnWndMemos->setIcon(QIcon(":/toolbar/notepad"));
        break;
    case MBE_MDI_CHILD_REQUESTED:
        if (auto wnd = params.value("wnd").value<QWidget*>(); wnd) {
            if (_mdiArea->hasChild(wnd))
                _mdiArea->activateChild(wnd);
            else
                _mdiArea->appendChild(wnd);
        }
        break;    
    }
}
