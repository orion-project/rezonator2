#include "CalcManager.h"
#include "ElementsCatalogDialog.h"
#include "ProjectWindow.h"
#include "ProjectOperations.h"
#include "ProtocolWindow.h"
#include "SchemaViewWindow.h"
#include "WindowsManager.h"
#include "core/Format.h"
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

#define STATUS_COUNT 5
#define STATUS_ELEMS 0
#define STATUS_MODIF 1
#define STATUS_LAMBDA 2
#define STATUS_STABIL 3
#define STATUS_FILE 4

ProjectWindow::ProjectWindow() : QMainWindow(), SchemaToolWindow(new Schema())
{
    Ori::Wnd::setWindowIcon(this, ":/window_icons/main");

    _calculations = new CalcManager(schema(), this);
    _operations = new ProjectOperations(schema(), this, _calculations);

    auto schemaViewWindow = new SchemaViewWindow(schema(), _calculations);

    loadSettings();

    _mdiArea = new SchemaMdiArea;
    _mdiArea->appendChild(schemaViewWindow);
    setCentralWidget(_mdiArea);

    connect(_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenuBar()));
    connect(WindowsManager::instancePtr(), SIGNAL(showMdiSubWindow(QWidget*)), _mdiArea, SLOT(appendChild(QWidget*)));
    connect(_mruList, SIGNAL(clicked(QString)), _operations, SLOT(openSchemaFile(QString)));
    connect(_operations, SIGNAL(fileNameSelected(QString)), _mruList, SLOT(append(QString)));
    connect(_operations, SIGNAL(protocolRequired()), this, SLOT(showProtocolWindow()));

    createActions();
    createMenuBar();
    createToolBars();
    createStatusBar();

    updateTitle();
    updateActions();
    updateMenuBar();
    updateStatusInfo();
    updateStability();

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
    //actnFileProp = _a(tr("P&roperties..."), _operations, SLOT(setupSchema()), ":/toolbar/schema_prop", Qt::Key_F11);
    actnFileLambda = A_(tr("Change &Wavelength..."), _operations, SLOT(setupWavelength()), ":/toolbar/wavelength", Qt::Key_F10);
    actnFilePump = A_(tr("Setup &Input Beam..."), _operations, SLOT(setupPump()), ":/toolbar/schema_pump", Qt::Key_F9);
    actnFileSummary = A_(tr("Summar&y..."), _calculations, SLOT(funcSummary()), ":/toolbar/schema_summary", Qt::CTRL | Qt::Key_I);
    actnFileExit = A_(tr("E&xit"), qApp, SLOT(closeAllWindows()), 0, Qt::CTRL | Qt::Key_Q);

    actnEditCut = A_(tr("Cu&t"), _mdiArea, SLOT(editableChild_Cut()), ":/toolbar/cut", QKeySequence::Cut);
    actnEditCopy = A_(tr("&Copy"), _mdiArea, SLOT(editableChild_Copy()), ":/toolbar/copy", QKeySequence::Copy);
    actnEditPaste = A_(tr("&Paste"), _mdiArea, SLOT(editableChild_Paste()), ":/toolbar/paste", QKeySequence::Paste);
    actnEditSelectAll = A_(tr("Select &All"), _mdiArea, SLOT(editableChild_SelectAll()), 0, QKeySequence::SelectAll);

    actnFuncRoundTrip = A_(tr("&Round-trip Matrix"), _calculations, SLOT(funcRoundTrip()), ":/toolbar/func_round_trip");
    actnFuncMultFwd = A_(tr("Multiply Selected &Forward"), _calculations, SLOT(funcMultFwd()));
    actnFuncMultBkwd = A_(tr("Multiply Selected &Backward"), _calculations, SLOT(funcMultBkwd()));
    actnFuncStabMap = A_(tr("&Stability Map..."), _calculations, SLOT(funcStabMap()), ":/toolbar/func_stab_map");
    actnFuncStabMap2d = A_(tr("&2D Stability Map..."), _calculations, SLOT(funcStabMap2d()), ":/toolbar/func_stab_map_2d");
    actnFuncRepRate = A_(tr("&Intermode Beats Frequency"), _calculations, SLOT(funcRepRate()), ":/toolbar/func_reprate");
    actnFuncCaustic = A_(tr("&Caustic..."), _calculations, SLOT(funcCaustic()), ":/toolbar/func_caustic");

    actnToolsCatalog = A_(tr("&Elements Catalog"), this, SLOT(showElementsCatalog()), ":/toolbar/catalog");
    //actnToolsBeamCalc = _a(tr("&Beam Calculator"), this, SLOT(showBeamCalculator()), ":/toolbar/gauss");
    actnToolsPrefs = A_(tr("Pre&ferences..."), this, SLOT(showPreferences()), ":/toolbar/options");

    actnWndSchema = A_(tr("Show &Schema Window"), _mdiArea, SLOT(activateChild()), ":/toolbar/schema", Qt::Key_F12);
    actnWndProtocol = A_(tr("Show &Protocol Window"), this, SLOT(showProtocolWindow()), ":/toolbar/protocol");
    actnWndClose = A_(tr("Cl&ose"), _mdiArea, SLOT(closeActiveSubWindow()));
    actnWndCloseAll = A_(tr("Close &All"), _mdiArea, SLOT(closeAllSubWindows()), ":/toolbar/windows_close");
    actnWndTile = A_(tr("&Tile"), _mdiArea, SLOT(tileSubWindows()));
    actnWndCascade = A_(tr("&Cascade"), _mdiArea, SLOT(cascadeSubWindows()));

    actnHelpUpdates = A_(tr("&Check for Updates"), this, SLOT(actionHelpUpdate()));
    actnHelpHomepage = A_(tr("&Visit Homepage"), this, SLOT(actionHelpHomePage()));
    actnHelpAbout = A_(tr("&About..."), this, SLOT(actionHelpAbout()));

    actnFileSave->setEnabled(false);
    actnEditCut->setVisible(false);
    actnEditCut->setEnabled(false);
}

void ProjectWindow::createMenuBar()
{
    _mruMenu = new Ori::Widgets::MruMenu(tr("Recent &Files"), _mruList, this);

    menuFile = Ori::Gui::menu(tr("&File"), this,
        { actnFileNew, actnFileOpen, _mruMenu, 0, actnFileSave,
          actnFileSaveAs, actnFileSaveCopy, 0, /*actnFileProp,*/
          actnFileLambda, actnFilePump, actnFileSummary, 0, actnFileExit });

    menuEdit = Ori::Gui::menu(tr("&Edit"), this,
        { actnEditCut, actnEditCopy, actnEditPaste, 0, actnEditSelectAll });

    menuView = Ori::Gui::menu(tr("&View"), this,
        { new Ori::Widgets::StylesMenu(_styler),
          new Ori::Widgets::LanguagesMenu(_translator, ":/toolbar16/langs") });

    menuFunctions = Ori::Gui::menu(tr("F&unctions"),
        { actnFuncRoundTrip, actnFuncMultFwd, actnFuncMultBkwd, 0, actnFuncStabMap,
          actnFuncStabMap2d, 0, actnFuncCaustic, 0, actnFuncRepRate });

    menuTools = Ori::Gui::menu(tr("&Tools", "Menu title"),
        { /*actnToolsBeamCalc,*/ actnToolsCatalog, 0, actnToolsPrefs });

    menuWindow = Ori::Gui::menu(tr("&Window"), {});
    connect(menuWindow, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuHelp = Ori::Gui::menu(tr("&Help"),
        { actnHelpUpdates, actnHelpHomepage, 0, actnHelpAbout });
}

void ProjectWindow::createToolBars()
{
    addToolBar(makeToolBar(tr("File"),
        { actnFileNew, Ori::Gui::menuToolButton(_mruMenu, actnFileOpen),
          actnFileSave, 0, /*actnFileProp,*/ actnFilePump, actnFileSummary }));

    addToolBar(makeToolBar(tr("Edit"), { actnEditCut, actnEditCopy, actnEditPaste }));

    addToolBar(makeToolBar(tr("Functions"),
        { actnFuncRoundTrip, 0, actnFuncStabMap, actnFuncStabMap2d, 0, actnFuncCaustic, 0,
          actnFuncRepRate }));

    addToolBar(makeToolBar(tr("Tools"), { /*actnToolsBeamCalc,*/ 0, actnWndProtocol }));

    addToolBar(Qt::BottomToolBarArea, new Ori::Widgets::MdiToolBar(tr("Windows"), _mdiArea));
}

void ProjectWindow::createStatusBar()
{
    auto status = new Ori::Widgets::StatusBar(STATUS_COUNT);
    status->connect(STATUS_LAMBDA, SIGNAL(doubleClicked()), _operations, SLOT(setupWavelength()));
    status->connect(STATUS_STABIL, SIGNAL(doubleClicked()), _operations, SLOT(setupPump()));
    setStatusBar(status);
}

void ProjectWindow::updateTitle()
{
    Ori::Wnd::setWindowFilePath(this, schema()->fileName());
}

/// Slot connected to @c mdiArea::subWindowActivated() signal.
/// This method automatically called when mdiArea changes active widget.
///
void ProjectWindow::updateMenuBar()
{
    BasicMdiChild* child = _mdiArea->activeChild();
    EditableWindow* editable = _mdiArea->activeEditableChild();

    actnEditCut->setEnabled(editable);
    actnEditCopy->setEnabled(editable);
    actnEditPaste->setEnabled(editable);
    actnEditSelectAll->setEnabled(editable);

    QMenuBar* menuBar = this->menuBar();
    menuBar->clear();
    menuBar->addMenu(menuFile);
    menuBar->addMenu(menuEdit);
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
    bool isSchemaSP = schema()->tripType() == Schema::SP;

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

    status->setText(STATUS_LAMBDA, schema()->wavelength().str());

    if (schema()->fileName().isEmpty()) status->clear(STATUS_FILE);
    else status->setText(STATUS_FILE, schema()->fileName());
}

void ProjectWindow::updateStability()
{
    auto status = dynamic_cast<Ori::Widgets::StatusBar*>(statusBar());
    Schema::TripType tripType = schema()->tripType();
    QString icon, hint;
    switch (tripType)
    {
    case Schema::SW:
        icon = ":/triptype16/SW";
        hint = tr("Standing wave system (SW)");
        break;
    case Schema::RR:
        icon = ":/triptype16/RR";
        hint = tr("Ring resonator (RR)");
        break;
    case Schema::SP:
        icon = ":/triptype16/SP";
        hint = tr("Single pass system (SP)");
        // TODO: show pump params in tooltip
        break;
    }
/*    if (tripType == Schema::SW || tripType == Schema::RR)
    {
        bool stableT, stableS;
        Calc::isStable(schema(), stableT, stableS);
        if (!stableT && !stableS)
        {
            icon += "_unstable";
            hint += '\n' + tr("System is unstable");
        }
        else if (!stableT)
        {
            icon += "_unstable_T";
            hint += '\n' + tr("System is unstable in T-plane");
        }
        else if (!stableS)
        {
            icon += "_unstable_S";
            hint += '\n' + tr("System is unstable in S-plane");
        }
    }*/
    status->setIcon(STATUS_STABIL, icon);
    status->setToolTip(STATUS_STABIL, hint);
}

/// Updates mdi windows menu. This methods called when "Window" menu popups.
void ProjectWindow::updateWindowMenu()
{
    Ori::Gui::populate(menuWindow, {actnWndSchema, actnWndProtocol, 0, actnWndClose,
                                    actnWndCloseAll, 0, actnWndTile, actnWndCascade, 0});
    _mdiArea->populateMenu(menuWindow);
}

void ProjectWindow::closeEvent(QCloseEvent* ce)
{
    if (_operations->canClose())
        ce->accept();
    else
        ce->ignore();
}

////////////////////////////////////////////////////////////////////////////////
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
    //showBeamCalculatorWindow();
}

////////////////////////////////////////////////////////////////////////////////
//                             Help actions

void ProjectWindow::actionHelpAbout()
{
#ifdef SVN_REV
    auto rev = QLatin1String(SVN_REV);
#else
    auto rev = QLatin1Literal("??");
#endif
    auto title = tr("About %1").arg(qApp->applicationName());
    auto text = tr(
                "<p><font size=4><b>%1 %2.%3</b></font>"
                "<p>Built on %5 at %6 from revision %4"
                "<p>Copyright (C) 2006-2015 by Chunosov N.&nbsp;I."
                "<p>Web: <a href='{www}'>{www}</a>"
                "<p>E-mail: <a href='mailto://{email}'>{email}</a>"
                "<p>Credits: <a href='http://www.qcustomplot.com/'>QCustomPlot</a>, "
                "<a href='http://muparser.beltoforion.de/'>muparser</a>"
                "<p>The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING "
                "THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."
                "<br>&nbsp;")
            .arg(qApp->applicationName()).arg(APP_VER_MAJOR).arg(APP_VER_MINOR)
            .arg(rev).arg(BUILDDATE).arg(BUILDTIME);
    text = text.replace("{www}", Z::Strs::homepage());
    text = text.replace("{email}", Z::Strs::email());
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
    // TODO
}

////////////////////////////////////////////////////////////////////////////////
//                               Window action

void ProjectWindow::showProtocolWindow()
{
    _mdiArea->appendChild(ProtocolWindow::create());
}

////////////////////////////////////////////////////////////////////////////////
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
