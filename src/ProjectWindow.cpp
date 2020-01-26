#include "ProjectWindow.h"

#include "AdjustmentWindow.h"
#include "CalcManager.h"
#include "CalculatorWindow.h"
#include "CommonData.h"
#include "CustomElemsWindow.h"
#include "ElementsCatalogDialog.h"
#include "GaussCalculatorWindow.h"
#include "HelpSystem.h"
#include "ProjectOperations.h"
#include "ProtocolWindow.h"
#include "PumpWindow.h"
#include "SchemaViewWindow.h"
#include "SchemaParamsWindow.h"
#include "WindowsManager.h"
#include "core/Format.h"
#include "funcs/RoundTripCalculator.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriWindows.h"
#include "tools/OriMruList.h"
#include "tools/OriSettings.h"
#include "widgets/OriFlatToolBar.h"
#include "widgets/OriLangsMenu.h"
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

ProjectWindow::ProjectWindow(Schema* aSchema) : QMainWindow(), SchemaToolWindow(aSchema)
{
    setAttribute(Qt::WA_DeleteOnClose);
    Ori::Wnd::setWindowIcon(this, ":/window_icons/main");

    Ori::Settings s;
    s.beginGroup("View");
    s.restoreWindowGeometry("mainWindow", this);

    _calculations = new CalcManager(schema(), this);
    _operations = new ProjectOperations(schema(), this, _calculations);

    Z::HelpSystem::instance()->setParent(this);

    _schemaWindow = new SchemaViewWindow(schema(), _calculations);

    _mdiArea = new SchemaMdiArea;
    _mdiArea->appendChild(_schemaWindow);
    setCentralWidget(_mdiArea);

    connect(_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenuBar()));
    connect(WindowsManager::instancePtr(), SIGNAL(showMdiSubWindow(QWidget*)), _mdiArea, SLOT(appendChild(QWidget*)));
    connect(CommonData::instance()->mruList(), SIGNAL(clicked(QString)), _operations, SLOT(openSchemaFile(QString)));
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
}

ProjectWindow::~ProjectWindow()
{
    Ori::Settings s;
    s.beginGroup("View");
    s.storeWindowGeometry("mainWindow", this);
}

void ProjectWindow::registerStorableWindows()
{
    WindowsManager::registerConstructor(SchemaParamsWindowStorable::windowType, SchemaParamsWindowStorable::createWindow);
    WindowsManager::registerConstructor(PumpWindowStorable::windowType, PumpWindowStorable::createWindow);
}

void ProjectWindow::createActions()
{
    #define A_ Ori::Gui::action

    actnFileNew = A_(tr("New"), _operations, SLOT(newSchemaFile()), ":/toolbar/schema_new", QKeySequence::New);
    actnFileOpen = A_(tr("Open..."), _operations, SLOT(openSchemaFile()), ":/toolbar/schema_open", QKeySequence::Open);
    actnFileOpenExample = A_(tr("Open Example..."), this, SLOT(openSchemaExample()), ":/toolbar/schema_sample");
    actnFileSave = A_(tr("Save"), _operations, SLOT(saveSchemaFile()), ":/toolbar/schema_save", QKeySequence::Save);
    actnFileSaveAs = A_(tr("Save As..."), _operations, SLOT(saveSchemaFileAs()), nullptr, QKeySequence::SaveAs);
    actnFileSaveCopy = A_(tr("Save Copy..."), _operations, SLOT(saveSchemaFileCopy()));
    actnFileLambda = A_(tr("Change Wavelength..."), _operations, SLOT(setupWavelength()), ":/toolbar/wavelength", Qt::Key_F10);
    actnFileTripType = A_(tr("Change Trip Type..."), _operations, SLOT(setupTripType()));
    actnFilePump = A_(tr("Setup Input Beam..."), _operations, SLOT(setupPump()), ":/toolbar/pump_edit", Qt::Key_F9);
    actnFileSummary = A_(tr("Summary..."), _calculations, SLOT(funcSummary()), ":/toolbar/schema_summary", Qt::CTRL | Qt::Key_I);
    actnFileProps = A_(tr("Properties..."), _operations, SLOT(editSchemaProps()), ":/toolbar/schema_prop");
    actnFileExit = A_(tr("Exit"), qApp, SLOT(closeAllWindows()), nullptr, Qt::CTRL | Qt::Key_Q);

    actnEditCut = A_(tr("Cut"), _mdiArea, SLOT(editableChild_Cut()), ":/toolbar/cut", QKeySequence::Cut);
    actnEditCopy = A_(tr("Copy"), _mdiArea, SLOT(editableChild_Copy()), ":/toolbar/copy", QKeySequence::Copy);
    actnEditPaste = A_(tr("Paste"), _mdiArea, SLOT(editableChild_Paste()), ":/toolbar/paste", QKeySequence::Paste);
    actnEditSelectAll = A_(tr("Select All"), _mdiArea, SLOT(editableChild_SelectAll()), nullptr, QKeySequence::SelectAll);

    actnFuncRoundTrip = A_(tr("Round-trip Matrix"), _calculations, SLOT(funcRoundTrip()), ":/toolbar/func_round_trip");
    actnFuncMultFwd = A_(tr("Multiply Selected Forward"), _calculations, SLOT(funcMultFwd()));
    actnFuncMultBkwd = A_(tr("Multiply Selected Backward"), _calculations, SLOT(funcMultBkwd()));
    actnFuncStabMap = A_(tr("Stability Map..."), _calculations, SLOT(funcStabMap()), ":/toolbar/func_stab_map");
    actnFuncStabMap2d = A_(tr("2D Stability Map..."), _calculations, SLOT(funcStabMap2d()), ":/toolbar/func_stab_map_2d");
    actnFuncRepRate = A_(tr("Intermode Beats Frequency"), _calculations, SLOT(funcRepRate()), ":/toolbar/func_reprate");
    actnFuncCaustic = A_(tr("Caustic..."), _calculations, SLOT(funcCaustic()), ":/toolbar/func_caustic");
    actnFuncMultirangeCaustic = A_(tr("Multirange Caustic..."), _calculations, SLOT(funcMultirangeCaustic()), ":/toolbar/func_multi_caustic");
    actnFuncMultibeamCaustic = A_(tr("Multibeam Caustic..."), _calculations, SLOT(funcMultibeamCaustic()), ":/toolbar/func_multi_beam_caustic");
    actnFuncBeamVariation = A_(tr("Beamsize Variation..."), _calculations, SLOT(funcBeamVariation()), ":/toolbar/func_beam_variation");
    actnFuncBeamParamsAtElems = A_(tr("Beam Parameters at Elemens"), _calculations, SLOT(funcBeamParamsAtElems()), ":/toolbar/func_beamdata");

    actnToolsCustomElems = A_(tr("Custom Elements Library"), this, SLOT(showCustomElems()), ":/toolbar/catalog");
    actnToolsGaussCalc = A_(tr("Gaussian Beam Calculator"), this, SLOT(showGaussCalculator()), ":/toolbar/gauss_calculator");
    actnToolsCalc = A_(tr("Formula Calculator"), this, SLOT(showCalculator()), ":/window_icons/calculator");
    actnToolFlipSchema = A_(tr("Flip Schema..."), this, SLOT(flipSchema()));
    actnToolSettings = A_(tr("Settings..."), this, SLOT(showSettings()), ":/toolbar/settings");
    actnToolAdjust = A_(tr("Adjustment"), this, SLOT(showAdjustment()), ":/toolbar/adjust");

    // These common window actions must not have data (action->data()), as data presense indicates that
    // this action is for activation of specific subwindow and _mdiArea is responsible for it.
    actnWndSchema = A_(tr("Schema"), this, SLOT(showSchemaWindow()), ":/toolbar/schema", Qt::Key_F12);
    actnWndParams = A_(tr("Parameters"), this, SLOT(showParamsWindow()), ":/toolbar/parameter", Qt::Key_F11);
    actnWndPumps = A_(tr("Pumps"), this, SLOT(showPumpsWindow()), ":/toolbar/pumps");
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

    /* TODO:NEXT-VER
    actnFileSave->setEnabled(false);
    actnEditCut->setVisible(false);
    actnEditCut->setEnabled(false); */

    #undef A_
}

void ProjectWindow::createMenuBar()
{
    menuBar()->setNativeMenuBar(AppSettings::instance().useNativeMenuBar);

    _mruMenu = new Ori::Widgets::MruMenu(tr("Recent Files"), CommonData::instance()->mruList(), this);

    menuFile = Ori::Gui::menu(tr("File"), this,
        { actnFileNew, actnFileOpen, actnFileOpenExample, _mruMenu, nullptr, actnFileSave,
          actnFileSaveAs, actnFileSaveCopy, nullptr,
          actnFileProps, actnFileTripType, actnFileLambda, actnFilePump, actnFileSummary, nullptr, actnFileExit });

    menuEdit = Ori::Gui::menu(tr("Edit"), this,
        { actnEditCut, actnEditCopy, actnEditPaste, nullptr, actnEditSelectAll });

    _langsMenu = new Ori::Widgets::LanguagesMenu(CommonData::instance()->translator(), ":/toolbar16/langs", this);
    menuView = new QMenu(tr("View"), this);

    menuFunctions = Ori::Gui::menu(tr("Functions"), this,
        { actnFuncRoundTrip, actnFuncMultFwd, actnFuncMultBkwd, nullptr,
          actnFuncStabMap, actnFuncStabMap2d, actnFuncBeamVariation, nullptr,
          actnFuncCaustic, actnFuncMultirangeCaustic, actnFuncMultibeamCaustic,
          actnFuncBeamParamsAtElems, nullptr, actnFuncRepRate });

    menuTools = Ori::Gui::menu(tr("Tools", "Menu title"), this,
        { actnToolFlipSchema, nullptr, actnToolAdjust, nullptr,
          actnToolsGaussCalc, actnToolsCalc, actnToolsCustomElems, nullptr, actnToolSettings });

    menuWindow = Ori::Gui::menu(tr("Window"), this,
        { actnWndSchema, actnWndParams, actnWndPumps, actnWndProtocol, nullptr,
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
        actnFuncRepRate, nullptr, actnWndParams, actnWndPumps, nullptr, actnToolAdjust, nullptr,
        actnToolsGaussCalc, actnToolsCalc
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

void ProjectWindow::updateTitle()
{
    if (!schema()->title().isEmpty())
        Ori::Wnd::setWindowProject(this, schema()->title());
    else
        Ori::Wnd::setWindowFilePath(this, schema()->fileName());
}

namespace  {

void activateEditAction(QAction* action, IEditableWindow* wnd, IEditableWindow::SupportedCommand cmd)
{
    bool on = wnd && wnd->supportedCommands().testFlag(cmd);
    action->setEnabled(on);
    action->setVisible(on);
}

} // namespace

void ProjectWindow::updateMenuBar()
{
    BasicMdiChild* child = _mdiArea->activeChild();

    // Update Edit menu
    IEditableWindow* editable = _mdiArea->activeEditableChild();
    activateEditAction(actnEditCut, editable, IEditableWindow::EditCmd_Cut);
    activateEditAction(actnEditCopy, editable, IEditableWindow::EditCmd_Copy);
    activateEditAction(actnEditPaste, editable, IEditableWindow::EditCmd_Paste);
    activateEditAction(actnEditSelectAll, editable, IEditableWindow::EditCmd_SelectAll);

    // Update View menu
    menuView->clear();
    if (child)
    {
        auto items = child->menuItems_View();
        if (!items.empty())
        {
            for (auto item : items)
            {
                if (item.action)
                    menuView->addAction(item.action);
                else if (item.menu)
                    menuView->addMenu(item.menu);
                else
                    menuView->addSeparator();
            }
            menuView->addSeparator();
        }
    }
    menuView->addMenu(_langsMenu);

    // Update menu bar
    QMenuBar* menuBar = this->menuBar();
    menuBar->clear();
    menuBar->addMenu(menuFile);
    if (editable)
        menuBar->addMenu(menuEdit);
    menuBar->addMenu(menuView);
    menuBar->addMenu(menuFunctions);
    if (child)
        for (QMenu* menu : child->menus())
            menuBar->addMenu(menu);
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
}

void ProjectWindow::updateStatusInfo()
{
    int totalCount = schema()->count();
    int enabledCount = schema()->enabledCount();
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
            auto pumpMode = Pump::findByModeName(pump->modeName());
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
    QTimer::singleShot(0, [&](){
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
//                             File actions

void ProjectWindow::openSchemaExample()
{
    auto fileName = ProjectOperations::selectSchemaExample();
    if (fileName.isEmpty()) return;
    _operations->openExampleFile(fileName);
}

//------------------------------------------------------------------------------
//                             Tools actions

void ProjectWindow::showCustomElems()
{
    CustomElemsWindow::showWindow();
}

void ProjectWindow::showSettings()
{
    AppSettings::instance().edit(this);
}

void ProjectWindow::showGaussCalculator()
{
    GaussCalculatorWindow::showWindow();
}

void ProjectWindow::showCalculator()
{
    CalculatorWindow::showWindow();
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
