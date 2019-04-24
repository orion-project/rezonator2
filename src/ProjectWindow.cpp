#include "ProjectWindow.h"

#include "CalcManager.h"
#include "CommonData.h"
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
#include "widgets/OriStylesMenu.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QTimer>
#include <QToolButton>

enum ProjectWindowStatusPanels
{
    STATUS_ELEMS,
    STATUS_MODIF,
    STATUS_LAMBDA,
    STATUS_TRIPTYPE,
    STATUS_PUMP,
    STATUS_STABIL,
    STATUS_FILE,

    STATUS_PANELS_COUNT,
};

ProjectWindow::ProjectWindow(Schema* readySchema) :
    QMainWindow(), SchemaToolWindow(readySchema ? readySchema : new Schema())
{
    setAttribute(Qt::WA_DeleteOnClose);
    Ori::Wnd::setWindowIcon(this, ":/window_icons/main");

    Ori::Settings s;
    s.beginGroup("View");
    s.restoreWindowGeometry("mainWindow", this);

    if (!readySchema)
    {
        schema()->events().disable();
        schema()->setTripType(TripTypes::find(Settings::instance().defaultTripType));
        schema()->events().enable();
    }

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

    if (Settings::instance().showProtocolAtStart)
        showProtocolWindow();
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

    actnFileNew = A_(tr("&New"), _operations, SLOT(newSchemaFile()), ":/toolbar/schema_new", QKeySequence::New);
    actnFileOpen = A_(tr("&Open..."), _operations, SLOT(openSchemaFile()), ":/toolbar/schema_open", QKeySequence::Open);
    actnFileOpenExample = A_(tr("Open &Example..."), this, SLOT(openSchemaExample()), ":/toolbar/schema_sample");
    actnFileSave = A_(tr("&Save"), _operations, SLOT(saveSchemaFile()), ":/toolbar/schema_save", QKeySequence::Save);
    actnFileSaveAs = A_(tr("Save &As..."), _operations, SLOT(saveSchemaFileAs()), nullptr, QKeySequence::SaveAs);
    actnFileSaveCopy = A_(tr("Save &Copy..."), _operations, SLOT(saveSchemaFileCopy()));
    actnFileLambda = A_(tr("Change &Wavelength..."), _operations, SLOT(setupWavelength()), ":/toolbar/wavelength", Qt::Key_F10);
    actnFileTripType = A_(tr("Change &Trip Type..."), _operations, SLOT(setupTripType()));
    actnFilePump = A_(tr("Setup &Input Beam..."), _operations, SLOT(setupPump()), ":/toolbar/pump_edit", Qt::Key_F9);
    actnFileSummary = A_(tr("Summar&y..."), _calculations, SLOT(funcSummary()), ":/toolbar/schema_summary", Qt::CTRL | Qt::Key_I);
    actnFileProps = A_(tr("Prop&erties..."), _operations, SLOT(editSchemaProps()), ":/toolbar/schema_prop");
    actnFileExit = A_(tr("E&xit"), qApp, SLOT(closeAllWindows()), nullptr, Qt::CTRL | Qt::Key_Q);

    actnEditCut = A_(tr("Cu&t"), _mdiArea, SLOT(editableChild_Cut()), ":/toolbar/cut", QKeySequence::Cut);
    actnEditCopy = A_(tr("&Copy"), _mdiArea, SLOT(editableChild_Copy()), ":/toolbar/copy", QKeySequence::Copy);
    actnEditPaste = A_(tr("&Paste"), _mdiArea, SLOT(editableChild_Paste()), ":/toolbar/paste", QKeySequence::Paste);
    actnEditSelectAll = A_(tr("Select &All"), _mdiArea, SLOT(editableChild_SelectAll()), nullptr, QKeySequence::SelectAll);

    actnFuncRoundTrip = A_(tr("&Round-trip Matrix"), _calculations, SLOT(funcRoundTrip()), ":/toolbar/func_round_trip");
    actnFuncMultFwd = A_(tr("Multiply Selected &Forward"), _calculations, SLOT(funcMultFwd()));
    actnFuncMultBkwd = A_(tr("Multiply Selected &Backward"), _calculations, SLOT(funcMultBkwd()));
    actnFuncStabMap = A_(tr("&Stability Map..."), _calculations, SLOT(funcStabMap()), ":/toolbar/func_stab_map");
    actnFuncStabMap2d = A_(tr("&2D Stability Map..."), _calculations, SLOT(funcStabMap2d()), ":/toolbar/func_stab_map_2d");
    actnFuncRepRate = A_(tr("&Intermode Beats Frequency"), _calculations, SLOT(funcRepRate()), ":/toolbar/func_reprate");
    actnFuncCaustic = A_(tr("&Caustic..."), _calculations, SLOT(funcCaustic()), ":/toolbar/func_caustic");
    actnFuncMultiCaustic = A_(tr("&Multicaustic..."), _calculations, SLOT(funcMultiCaustic()), ":/toolbar/func_multi_caustic");
    actnFuncBeamVariation = A_(tr("&Beamsize Variation..."), _calculations, SLOT(funcBeamVariation()), ":/toolbar/func_beam_variation");

    actnToolsCatalog = A_(tr("&Elements Catalog"), this, SLOT(showElementsCatalog()), ":/toolbar/catalog");
    actnToolsGaussCalc = A_(tr("&Gauss Calculator"), this, SLOT(showGaussCalculator()), ":/toolbar/gauss_calculator");
    actnToolsPrefs = A_(tr("Pre&ferences..."), this, SLOT(showPreferences()), ":/toolbar/settings");
    actnToolFlipSchema = A_(tr("&Flip Schema"), this, SLOT(flipSchema()));

    // These common window actions must not have data (action->data()), as data presense indicates that
    // this action is for activation of specific subwindow and _mdiArea is responsible for it.
    actnWndSchema = A_(tr("&Schema"), this, SLOT(showSchemaWindow()), ":/toolbar/schema", Qt::Key_F12);
    actnWndParams = A_(tr("&Parameters"), this, SLOT(showParamsWindow()), ":/toolbar/parameter", Qt::Key_F11);
    actnWndPumps = A_(tr("P&umps"), this, SLOT(showPumpsWindow()), ":/toolbar/pumps");
    actnWndProtocol = A_(tr("P&rotocol"), this, SLOT(showProtocolWindow()), ":/toolbar/protocol");
    actnWndClose = A_(tr("Cl&ose"), _mdiArea, SLOT(closeActiveSubWindow()));
    actnWndCloseAll = A_(tr("Close &All"), _mdiArea, SLOT(closeAllSubWindows()), ":/toolbar/windows_close");
    actnWndTile = A_(tr("&Tile"), _mdiArea, SLOT(tileSubWindows()));
    actnWndCascade = A_(tr("&Cascade"), _mdiArea, SLOT(cascadeSubWindows()));

    auto help = Z::HelpSystem::instance();
    actnHelpContent = A_(tr("&Contents"), help, SLOT(showContents()), ":/toolbar/help");
    actnHelpIndex = A_(tr("&Index"), help, SLOT(showIndex()));
    actnHelpBugReport = A_(tr("&Send Bug Report"), help, SLOT(sendBugReport()), ":/toolbar/bug");
    actnHelpUpdates = A_(tr("Check for &Updates"), help, SLOT(checkUpdates()));
    actnHelpHomepage = A_(tr("&Visit Homepage"), help, SLOT(visitHomePage()));
    actnHelpAbout = A_(tr("&About..."), help, SLOT(showAbout()));

    /* TODO:NEXT-VER
    actnFileSave->setEnabled(false);
    actnEditCut->setVisible(false);
    actnEditCut->setEnabled(false); */

    #undef A_
}

void ProjectWindow::createMenuBar()
{
    menuBar()->setNativeMenuBar(Settings::instance().useNativeMenuBar);

    _mruMenu = new Ori::Widgets::MruMenu(tr("Recent &Files"), CommonData::instance()->mruList(), this);

    menuFile = Ori::Gui::menu(tr("&File"), this,
        { actnFileNew, actnFileOpen, actnFileOpenExample, _mruMenu, nullptr, actnFileSave,
          actnFileSaveAs, actnFileSaveCopy, nullptr,
          actnFileProps, actnFileTripType, actnFileLambda, actnFilePump, actnFileSummary, nullptr, actnFileExit });

    menuEdit = Ori::Gui::menu(tr("&Edit"), this,
        { actnEditCut, actnEditCopy, actnEditPaste, nullptr, actnEditSelectAll });

    _stylesMenu = new Ori::Widgets::StylesMenu(CommonData::instance()->styler(), this);
    _langsMenu = new Ori::Widgets::LanguagesMenu(CommonData::instance()->translator(), ":/toolbar16/langs", this);
    menuView = new QMenu(tr("&View"), this);

    menuFunctions = Ori::Gui::menu(tr("F&unctions"), this,
        { actnFuncRoundTrip, actnFuncMultFwd, actnFuncMultBkwd, nullptr, actnFuncStabMap,
          actnFuncStabMap2d, nullptr, actnFuncCaustic, actnFuncMultiCaustic,
          actnFuncBeamVariation, nullptr, actnFuncRepRate });

    menuTools = Ori::Gui::menu(tr("&Tools", "Menu title"), this,
        { actnToolFlipSchema, nullptr,
          actnToolsGaussCalc, actnToolsCatalog, nullptr, actnToolsPrefs });

    menuWindow = Ori::Gui::menu(tr("&Window"), this,
        { actnWndSchema, actnWndParams, actnWndPumps, actnWndProtocol, nullptr,
          actnWndClose, actnWndCloseAll, nullptr, actnWndTile, actnWndCascade, nullptr });
    connect(menuWindow, SIGNAL(aboutToShow()), _mdiArea, SLOT(populateWindowMenu()));

    menuHelp = Ori::Gui::menu(tr("&Help"), this,
        { actnHelpContent, actnHelpIndex, nullptr,
          actnHelpBugReport, actnHelpUpdates, actnHelpHomepage, nullptr,
          actnHelpAbout });
}

void ProjectWindow::createToolBars()
{
    addToolBar(makeToolBar({
        actnFileNew, Ori::Gui::menuToolButton(_mruMenu, actnFileOpen),
        actnFileSave, nullptr, actnFileProps, actnFilePump, actnFileSummary , nullptr,
        actnEditCut, actnEditCopy, actnEditPaste, nullptr,
        actnFuncRoundTrip, nullptr, actnFuncStabMap, actnFuncStabMap2d, actnFuncBeamVariation, nullptr,
        actnFuncCaustic, actnFuncMultiCaustic, nullptr, actnFuncRepRate, nullptr,
        actnWndParams, actnWndPumps, nullptr,
        actnToolsGaussCalc
    }, true));

    _mdiToolbar = new Ori::Widgets::MdiToolBar(tr("Windows"), _mdiArea);
    Z::WindowUtils::adjustIconSize(_mdiToolbar);
    addToolBar(Qt::BottomToolBarArea, _mdiToolbar);
}

void ProjectWindow::createStatusBar()
{
    auto status = new Ori::Widgets::StatusBar(STATUS_PANELS_COUNT);
    status->connect(STATUS_LAMBDA, SIGNAL(doubleClicked()), _operations, SLOT(setupWavelength()));
    status->connect(STATUS_TRIPTYPE, SIGNAL(doubleClicked()), _operations, SLOT(setupTripType()));
    status->connect(STATUS_PUMP, SIGNAL(doubleClicked()), _operations, SLOT(setupPump()));

    auto versionLabel = new QLabel(" " % Z::Strs::appVersion() % " ");
    versionLabel->setForegroundRole(QPalette::Mid);
    status->addPermanentWidget(versionLabel);

    setStatusBar(status);
}

void ProjectWindow::updateTitle()
{
    if (!schema()->title().isEmpty())
        Ori::Wnd::setWindowProject(this, schema()->title());
    else
        Ori::Wnd::setWindowFilePath(this, schema()->fileName());
}

namespace  {

void activateEditAction(QAction* action, EditableWindow* wnd, EditableWindow::SupportedCommand cmd)
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
    EditableWindow* editable = _mdiArea->activeEditableChild();
    activateEditAction(actnEditCut, editable, EditableWindow::EditCmd_Cut);
    activateEditAction(actnEditCopy, editable, EditableWindow::EditCmd_Copy);
    activateEditAction(actnEditPaste, editable, EditableWindow::EditCmd_Paste);
    activateEditAction(actnEditSelectAll, editable, EditableWindow::EditCmd_SelectAll);

    // Update View menu
    menuView->clear();
    if (child)
    {
        auto viewActions = child->viewActions();
        if (!viewActions.empty())
        {
            for (auto action : viewActions)
                menuView->addAction(action);
            menuView->addSeparator();
        }
    }
    menuView->addMenu(_stylesMenu);
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
    actnFileSave->setEnabled(schema()->modified());
}

void ProjectWindow::updateStatusInfo()
{
    auto status = dynamic_cast<Ori::Widgets::StatusBar*>(statusBar());

    int totalCount = schema()->count();
    int enabledCount = schema()->enabledCount();
    if (totalCount != enabledCount)
        status->setText(STATUS_ELEMS, tr("Elements: %1 (%2)",
                                         "Status text").arg(enabledCount).arg(totalCount));
    else status->setText(STATUS_ELEMS, tr("Elements: %1", "Status text").arg(totalCount));

    if (!schema()->modified()) status->clear(STATUS_MODIF);
    else status->setText(STATUS_MODIF, tr("Modified", "Status text"));

    status->setText(STATUS_LAMBDA, schema()->wavelength().displayStr());

    auto tripTypeInfo = TripTypes::info(schema()->tripType());
    status->setIcon(STATUS_TRIPTYPE, tripTypeInfo.iconPath());
    status->setToolTip(STATUS_TRIPTYPE, tripTypeInfo.toolTip());

    QString pumpHint, pumpIcon;
    if (schema()->isSP())
    {
        auto pump = schema()->activePump();
        if (pump)
        {
            pumpHint = pump->displayStr();
            auto pumpMode = Z::Pump::findByModeName(pump->modeName());
            if (pumpMode)
                pumpIcon = pumpMode->iconPath();
        }
    }
    status->setIcon(STATUS_PUMP, pumpIcon);
    status->setToolTip(STATUS_PUMP, pumpHint);

    if (schema()->fileName().isEmpty()) status->clear(STATUS_FILE);
    else status->setText(STATUS_FILE, schema()->fileName());
}

void ProjectWindow::updateStability()
{
    auto status = qobject_cast<Ori::Widgets::StatusBar*>(statusBar());
    QString icon, hint;
    if (schema()->isResonator())
    {
        auto stable = Calc::isStable(schema());
        if (!stable.T && !stable.S)
        {
            icon = QStringLiteral(":/icons/stability_err");
            hint = tr("System is unstable");
        }
        else if (!stable.T)
        {
            icon = QStringLiteral(":/icons/stability_err");
            hint = tr("System is unstable in T-plane");
        }
        else if (!stable.S)
        {
            icon = QStringLiteral(":/icons/stability_err");
            hint = tr("System is unstable in S-plane");
        }
        else
        {
            icon = QStringLiteral(":/icons/stability_ok");
            hint = tr("System is stable");
        }
    }
    else
    {
        // TODO:NEXT-VER show pump params in tooltip
    }
    status->setIcon(STATUS_STABIL, icon);
    status->setToolTip(STATUS_STABIL, hint);
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

void ProjectWindow::showElementsCatalog()
{
    Z::Dlgs::showElementsCatalog();
}

void ProjectWindow::showPreferences()
{
    Settings::instance().edit(this);
}

void ProjectWindow::showGaussCalculator()
{
    GaussCalculatorWindow::showCalcWindow();
}

void ProjectWindow::flipSchema()
{
    if (Ori::Dlg::yes(tr("Do you want to rearrange elements in the opposite order?")))
        schema()->flip();
}

//------------------------------------------------------------------------------
//                               Window action

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
//                               Schema events

void ProjectWindow::pumpChanged(Schema *s, Z::PumpParams *pump)
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

