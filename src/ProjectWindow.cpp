#include "CalcManager.h"
#include "CustomPrefs.h"
#include "ElementsCatalogDialog.h"
#include "ProjectWindow.h"
#include "ProjectOperations.h"
#include "ProtocolWindow.h"
#include "SchemaViewWindow.h"
#include "SchemaParamsWindow.h"
#include "WindowsManager.h"
#include "core/Format.h"
#include "funcs/Calculator.h"
#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriWindows.h"
#include "tools/OriMruList.h"
#include "tools/OriStyler.h"
#include "tools/OriTranslator.h"
#include "tools/OriSettings.h"
#include "widgets/OriLangsMenu.h"
#include "widgets/OriMruMenu.h"
#include "widgets/OriMdiToolBar.h"
#include "widgets/OriStatusBar.h"
#include "widgets/OriStylesMenu.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QToolButton>
#include <QUrl>

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

ProjectWindow::ProjectWindow() : QMainWindow(), SchemaToolWindow(new Schema())
{
    Ori::Wnd::setWindowIcon(this, ":/window_icons/main");

    loadSettings();

    _calculations = new CalcManager(schema(), this);
    _operations = new ProjectOperations(schema(), this, _calculations);

    _schemaWindow = new SchemaViewWindow(schema(), _calculations);

    _mdiArea = new SchemaMdiArea;
    _mdiArea->appendChild(_schemaWindow);
    setCentralWidget(_mdiArea);

    connect(_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenuBar()));
    connect(WindowsManager::instancePtr(), SIGNAL(showMdiSubWindow(QWidget*)), _mdiArea, SLOT(appendChild(QWidget*)));
    connect(_mruList, SIGNAL(clicked(QString)), _operations, SLOT(openSchemaFile(QString)));
    connect(_operations, &ProjectOperations::fileNameSelected, _mruList, &Ori::MruFileList::append);
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

    WindowsManager::registerConstructor(SchemaParamsWindowStorable::windowType, SchemaParamsWindowStorable::createWindow);

    QTimer::singleShot(200, _operations, SLOT(checkCmdLine()));
}

ProjectWindow::~ProjectWindow()
{
    saveSettings();
}

void ProjectWindow::loadSettings()
{
    Ori::Settings s;
    s.beginGroup("View");
    _styler = new Ori::Styler(s.strValue("style"), this);
    _translator = new Ori::Translator(s.strValue("language"), this);
    s.restoreWindowGeometry("mainWindow", this);

    s.beginGroup("States");
    _mruList = new Ori::MruFileList(this);
    _mruList->load(s.settings());

    Settings::instance().load();

    CustomPrefs::load(s.settings()->fileName());
}

void ProjectWindow::saveSettings()
{
    Ori::Settings s;
    s.beginGroup("View");
    s.setValue("style", _styler->currentStyle());
    s.setValue("language", _translator->currentLanguage());
    s.storeWindowGeometry("mainWindow", this);

    Settings::instance().save();
}

void ProjectWindow::createActions()
{
    #define A_ Ori::Gui::action

    actnFileNew = A_(tr("&New"), _operations, SLOT(newSchemaFile()), ":/toolbar/schema_new", QKeySequence::New);
    actnFileOpen = A_(tr("&Open..."), _operations, SLOT(openSchemaFile()), ":/toolbar/schema_open", QKeySequence::Open);
    actnFileSave = A_(tr("&Save"), _operations, SLOT(saveSchemaFile()), ":/toolbar/schema_save", QKeySequence::Save);
    actnFileSaveAs = A_(tr("Save &As..."), _operations, SLOT(saveSchemaFileAs()), 0, QKeySequence::SaveAs);
    actnFileSaveCopy = A_(tr("Save &Copy..."), _operations, SLOT(saveSchemaFileCopy()));
    actnFileLambda = A_(tr("Change &Wavelength..."), _operations, SLOT(setupWavelength()), ":/toolbar/wavelength", Qt::Key_F10);
    actnFileTripType = A_(tr("Change &Trip Type..."), _operations, SLOT(setupTripType()));
    actnFilePump = A_(tr("Setup &Input Beam..."), _operations, SLOT(setupPump()), ":/toolbar/schema_pump", Qt::Key_F9);
    actnFileSummary = A_(tr("Summar&y..."), _calculations, SLOT(funcSummary()), ":/toolbar/schema_summary", Qt::CTRL | Qt::Key_I);
    actnFileExit = A_(tr("E&xit"), qApp, SLOT(closeAllWindows()), 0, Qt::CTRL | Qt::Key_Q);

    /* TODO:NEXT-VER
    actnEditCut = A_(tr("Cu&t"), _mdiArea, SLOT(editableChild_Cut()), ":/toolbar/cut", QKeySequence::Cut);
    actnEditCopy = A_(tr("&Copy"), _mdiArea, SLOT(editableChild_Copy()), ":/toolbar/copy", QKeySequence::Copy);
    actnEditPaste = A_(tr("&Paste"), _mdiArea, SLOT(editableChild_Paste()), ":/toolbar/paste", QKeySequence::Paste);
    actnEditSelectAll = A_(tr("Select &All"), _mdiArea, SLOT(editableChild_SelectAll()), 0, QKeySequence::SelectAll); */

    actnFuncRoundTrip = A_(tr("&Round-trip Matrix"), _calculations, SLOT(funcRoundTrip()), ":/toolbar/func_round_trip");
    actnFuncMultFwd = A_(tr("Multiply Selected &Forward"), _calculations, SLOT(funcMultFwd()));
    actnFuncMultBkwd = A_(tr("Multiply Selected &Backward"), _calculations, SLOT(funcMultBkwd()));
    actnFuncStabMap = A_(tr("&Stability Map..."), _calculations, SLOT(funcStabMap()), ":/toolbar/func_stab_map");
    actnFuncStabMap2d = A_(tr("&2D Stability Map..."), _calculations, SLOT(funcStabMap2d()), ":/toolbar/func_stab_map_2d");
    actnFuncRepRate = A_(tr("&Intermode Beats Frequency"), _calculations, SLOT(funcRepRate()), ":/toolbar/func_reprate");
    actnFuncCaustic = A_(tr("&Caustic..."), _calculations, SLOT(funcCaustic()), ":/toolbar/func_caustic");

    actnToolsCatalog = A_(tr("&Elements Catalog"), this, SLOT(showElementsCatalog()), ":/toolbar/catalog");
    // TODO:NEXT-VER actnToolsBeamCalc = _a(tr("&Beam Calculator"), this, SLOT(showBeamCalculator()), ":/toolbar/gauss");
    actnToolsPrefs = A_(tr("Pre&ferences..."), this, SLOT(showPreferences()), ":/toolbar/options");

    // These common window actions must not have data (action->data()), as data presense indicates that
    // this action is for activation of specific subwindow and _mdiArea is responsible for it.
    actnWndSchema = A_(tr("Show &Schema Window"), this, SLOT(showSchemaWindow()), ":/toolbar/schema", Qt::Key_F12);
    actnWndParams = A_(tr("Show Pa&rameters Window"), this, SLOT(showParamsWindow()), ":/toolbar/parameter", Qt::Key_F11);
    actnWndProtocol = A_(tr("Show &Protocol Window"), this, SLOT(showProtocolWindow()), ":/toolbar/protocol");
    actnWndClose = A_(tr("Cl&ose"), _mdiArea, SLOT(closeActiveSubWindow()));
    actnWndCloseAll = A_(tr("Close &All"), _mdiArea, SLOT(closeAllSubWindows()), ":/toolbar/windows_close");
    actnWndTile = A_(tr("&Tile"), _mdiArea, SLOT(tileSubWindows()));
    actnWndCascade = A_(tr("&Cascade"), _mdiArea, SLOT(cascadeSubWindows()));

    actnHelpUpdates = A_(tr("&Check for Updates"), this, SLOT(actionHelpUpdate()));
    actnHelpHomepage = A_(tr("&Visit Homepage"), this, SLOT(actionHelpHomePage()));
    actnHelpAbout = A_(tr("&About..."), this, SLOT(actionHelpAbout()));

    actnHelpUpdates->setVisible(false); //< TODO:NEXT-VER

    /* TODO:NEXT-VER
    actnFileSave->setEnabled(false);
    actnEditCut->setVisible(false);
    actnEditCut->setEnabled(false); */

    #undef A_
}

void ProjectWindow::createMenuBar()
{
    menuBar()->setNativeMenuBar(Settings::instance().useNativeMenuBar);

    _mruMenu = new Ori::Widgets::MruMenu(tr("Recent &Files"), _mruList, this);

    menuFile = Ori::Gui::menu(tr("&File"), this,
        { actnFileNew, actnFileOpen, _mruMenu, 0, actnFileSave,
          actnFileSaveAs, actnFileSaveCopy, 0,
          actnFileLambda, actnFileTripType, actnFilePump, actnFileSummary, 0, actnFileExit });

    /* TODO:NEXT-VER menuEdit = Ori::Gui::menu(tr("&Edit"), this,
        { actnEditCut, actnEditCopy, actnEditPaste, 0, actnEditSelectAll }); */

    menuView = Ori::Gui::menu(tr("&View"), this,
        { new Ori::Widgets::StylesMenu(_styler),
          new Ori::Widgets::LanguagesMenu(_translator, ":/toolbar16/langs") });

    menuFunctions = Ori::Gui::menu(tr("F&unctions"), this,
        { actnFuncRoundTrip, actnFuncMultFwd, actnFuncMultBkwd, 0, actnFuncStabMap,
          actnFuncStabMap2d, 0, actnFuncCaustic, 0, actnFuncRepRate });

    menuTools = Ori::Gui::menu(tr("&Tools", "Menu title"), this,
        { /*TODO:NEXT-VER actnToolsBeamCalc,*/ actnToolsCatalog, 0, actnToolsPrefs });

    menuWindow = Ori::Gui::menu(tr("&Window"), this,
        { actnWndSchema, actnWndParams, actnWndProtocol, 0,
          actnWndClose, actnWndCloseAll, 0, actnWndTile, actnWndCascade, 0 });
    connect(menuWindow, SIGNAL(aboutToShow()), _mdiArea, SLOT(populateWindowMenu()));

    menuHelp = Ori::Gui::menu(tr("&Help"), this,
        { actnHelpUpdates, actnHelpHomepage, 0, actnHelpAbout });
}

void ProjectWindow::createToolBars()
{
    addToolBar(makeToolBar(tr("File"),
        { actnFileNew, Ori::Gui::menuToolButton(_mruMenu, actnFileOpen),
          actnFileSave, 0, actnFilePump, actnFileSummary }));

    // TODO:NEXT-VER addToolBar(makeToolBar(tr("Edit"), { actnEditCut, actnEditCopy, actnEditPaste }));

    addToolBar(makeToolBar(tr("Functions"),
        { actnFuncRoundTrip, 0, actnFuncStabMap, actnFuncStabMap2d, 0, actnFuncCaustic, 0,
          actnFuncRepRate }));

    addToolBar(makeToolBar(tr("Misc"),
        { actnWndParams }));

    // TODO:NEXT-VER addToolBar(makeToolBar(tr("Tools"), { actnToolsBeamCalc }));

    _mdiToolbar = new Ori::Widgets::MdiToolBar(tr("Windows"), _mdiArea);
    Z::WindowUtils::adjustIconSize(_mdiToolbar);
    addToolBar(Qt::BottomToolBarArea, _mdiToolbar);
}

void ProjectWindow::createStatusBar()
{
    auto status = new Ori::Widgets::StatusBar(STATUS_PANELS_COUNT);
    status->connect(STATUS_LAMBDA, SIGNAL(doubleClicked()), _operations, SLOT(setupWavelength()));
    status->connect(STATUS_TRIPTYPE, SIGNAL(doubleClicked()), _operations, SLOT(setupTripType()));
    status->connect(STATUS_STABIL, SIGNAL(doubleClicked()), _operations, SLOT(setupPump()));
    setStatusBar(status);
}

void ProjectWindow::updateTitle()
{
    Ori::Wnd::setWindowFilePath(this, schema()->fileName());
}

void ProjectWindow::updateMenuBar()
{
    BasicMdiChild* child = _mdiArea->activeChild();
    /* TODO:NEXT-VER
    EditableWindow* editable = _mdiArea->activeEditableChild();

    actnEditCut->setEnabled(editable);
    actnEditCopy->setEnabled(editable);
    actnEditPaste->setEnabled(editable);
    actnEditSelectAll->setEnabled(editable); */

    QMenuBar* menuBar = this->menuBar();
    menuBar->clear();
    menuBar->addMenu(menuFile);
    // TODO:NEXT-VER menuBar->addMenu(menuEdit);
    menuBar->addMenu(menuView);
    menuBar->addMenu(menuFunctions);
    if (child)
        foreach (QMenu* menu, child->menus())
            menuBar->addMenu(menu);
    menuBar->addMenu(menuTools);
    menuBar->addMenu(menuWindow);
    menuBar->addMenu(menuHelp);
}

void ProjectWindow::updateActions()
{
    bool isSchemaSP = schema()->isSP();

    actnFilePump->setEnabled(isSchemaSP);
    actnFilePump->setVisible(isSchemaSP);
    actnFuncStabMap->setVisible(!isSchemaSP);
    actnFuncStabMap2d->setVisible(false /* TODO:NEXT-VER !isSchemaSP */);
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

    status->setText(STATUS_LAMBDA, schema()->wavelength().str());

    auto tripTypeInfo = TripTypes::info(schema()->tripType());
    status->setIcon(STATUS_TRIPTYPE, tripTypeInfo.iconPath());
    status->setToolTip(STATUS_TRIPTYPE, tripTypeInfo.toolTip());

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
    if (_operations->canClose())
        ce->accept();
    else
        ce->ignore();
}

void ProjectWindow::settingsChanged()
{
    SchemaToolWindow::settingsChanged();
    Z::WindowUtils::adjustIconSize(_mdiToolbar);
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

void ProjectWindow::showBeamCalculator()
{
    // TODO:NEXT-VER showBeamCalculatorWindow();
}

//------------------------------------------------------------------------------
//                             Help actions

void ProjectWindow::actionHelpAbout()
{
    auto title = tr("About %1").arg(qApp->applicationName());
    auto text = tr(
                "<p><font size=4><b>{app} {ver_short}</b></font>"
                "<p>Version: {ver_long}-<b>{ver_codename}</b>"
                "<p>Built: {build_date}"
                "<p>Revision: <a href='{ver_link}'>{ver_sha}</a>"
                "<p>Copyright: Chunosov N.&nbsp;I. © 2006-2018"
                "<p>Web: <a href='{www}'>{www}</a>"
                "<p>E-mail: <a href='mailto://{email}'>{email}</a>"
                "<p>Credits: <a href='http://www.qcustomplot.com/'>QCustomPlot</a>"
                ", <a href='http://muparser.beltoforion.de/'>muparser</a>"
                "<p>{app} is open-source laser resonator calculation tool, its "
                "source code is located at <a href='{www_sources}'>{www_sources}</a>."
                "<p>The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING "
                "THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."
                )
            .replace("{app}", qApp->applicationName())
            .replace("{ver_short}", Z::Strs::appVersionShort())
            .replace("{ver_long}", Z::Strs::appVersionLong())
            .replace("{ver_codename}", Z::Strs::appVersionCodename())
            .replace("{build_date}", Z::Strs::appVersionDate())
            .replace("{ver_link}", Z::Strs::appVersionLink())
            .replace("{ver_sha}", Z::Strs::appVersionSHA().left(7))
            .replace("{www}", Z::Strs::homepage())
            .replace("{email}", Z::Strs::email())
            .replace("{www_sources}", Z::Strs::sourcepage());
    QMessageBox about(QMessageBox::NoIcon, title, text, QMessageBox::Ok, this);
    about.setIconPixmap(QPixmap(":/window_icons/main").
        scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    auto button = about.addButton(tr("About Qt"), QMessageBox::ActionRole);
    connect(button, SIGNAL(clicked()), qApp, SLOT(aboutQt()));
    about.exec();
}

void ProjectWindow::actionHelpHomePage()
{
    QDesktopServices::openUrl(QUrl(Z::Strs::homepage()));
}

void ProjectWindow::actionHelpUpdate()
{
    // TODO:NEXT-VER
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

//------------------------------------------------------------------------------
//                               Schema events

void ProjectWindow::schemaChanged(Schema* s)
{
    actnFileSave->setEnabled(s->state().isModified());

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
