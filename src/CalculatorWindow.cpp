#include "CalculatorWindow.h"

#include "Appearance.h"
#include "CustomPrefs.h"
#include "core/LuaHelper.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriWindows.h"

#include <QCheckBox>
#include <QDebug>
#include <QGroupBox>
#include <QFontDialog>
#include <QGroupBox>
#include <QHeaderView>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QTableWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>

using namespace Ori::Layouts;

namespace  {

CalculatorWindow* __instance = nullptr;

} //  namespace

//--------------------------------------------------------------------------------
//                             CalculatorSettingsDlg
//--------------------------------------------------------------------------------

CalculatorSettingsDlg::CalculatorSettingsDlg() : RezonatorDialog(RezonatorDialog::DontDeleteOnClose, __instance)
{
    setTitleAndIcon(tr("Formula Calculator Settings"), ":/window_icons/calculator");
    setObjectName("ElementPropsDialog");

    _fontSampleLabel = new QLabel("L / sqrt(n^2 - sin(deg2rad(a))^2)");
    _fontSampleLabel->setStyleSheet("background-color:white;padding:6px");
    _fontSampleLabel->setFrameShape(QFrame::StyledPanel);
    auto f = Z::Gui::CodeEditorFont().get();
    if (!__instance->_overrideFontName.isEmpty() and __instance->_overrideFontSize > 0)
    {
        f.setFamily(__instance->_overrideFontName);
        f.setPointSize(__instance->_overrideFontSize);
    }
    _fontSampleLabel->setFont(f);

    _chooseFontButton = new QPushButton(tr("Choose font..."));
    connect(_chooseFontButton, &QPushButton::clicked, this, &CalculatorSettingsDlg::chooseFontCliked);

    _groupFont = new QGroupBox(tr("Override default font"));
    _groupFont->setCheckable(true);
    _groupFont->setChecked(__instance->_overrideFont);
    LayoutV({_chooseFontButton, _fontSampleLabel}).useFor(_groupFont);

    mainLayout()->addWidget(_groupFont);
    mainLayout()->addSpacing(6);
}

void CalculatorSettingsDlg::collect()
{
    __instance->_overrideFont = _groupFont->isChecked();
    __instance->_overrideFontName = _fontSampleLabel->font().family();
    __instance->_overrideFontSize = _fontSampleLabel->font().pointSize();
    accept();
    close();
}

void CalculatorSettingsDlg::chooseFontCliked()
{
    bool ok = false;
    auto f0 = _fontSampleLabel->font();
    auto f1 = QFontDialog::getFont(&ok, f0, this, tr("Choose Font"));
    if (ok)
    {
        f0.setFamily(f1.family());
        f0.setPointSize(f1.pointSize());
        _fontSampleLabel->setFont(f0);
    }
}

//--------------------------------------------------------------------------------
//                               CalculatorWindow
//--------------------------------------------------------------------------------

void CalculatorWindow::showWindow()
{
    if (!__instance)
        __instance = new CalculatorWindow;
    __instance->show();
    __instance->activateWindow();
}

CalculatorWindow::CalculatorWindow(QWidget *parent) : QWidget(parent)
{
    Ori::Wnd::initWindow(this, tr("Formula Calculator"), ":/window_icons/calculator");

    // TODO: make custom items widget using _log as a model
    _logView = new QPlainTextEdit;
    _logView->setReadOnly(true);
    _logView->setPlaceholderText(tr("Calculation results are displayed here"));
    _logView->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    _varsView = new QTableWidget;
    _varsView->setColumnCount(2);
    _varsView->setShowGrid(false);
    _varsView->horizontalHeader()->setVisible(false);
    _varsView->verticalHeader()->setVisible(false);
    _varsView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _varsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _varsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _varsView->setAlternatingRowColors(true);
    _varsView->setWordWrap(false);

    _sessionSplitter = Ori::Gui::splitterH(_logView, _varsView);
    _sessionSplitter->setStretchFactor(0, 3);
    _sessionSplitter->setStretchFactor(1, 1);

    _editor = new QPlainTextEdit;
    _editor->setWordWrapMode(QTextOption::WordWrap);
    _editor->setPlaceholderText(tr("Enter your formula here and hit Ctrl+Enter to calculate"));
    connect(_editor, &QPlainTextEdit::textChanged, [this](){ _errorView->setVisible(false); });

    _errorView = new QLabel;
    _errorView->setVisible(false);
    _errorView->setWordWrap(true);
    _errorView->setStyleSheet("QLabel{background:red;color:white;font-weight:bold;padding:3px}");

    auto editor = LayoutV({_editor, _errorView}).setMargin(0).setSpacing(0).makeWidget();
    _mainSplitter = Ori::Gui::splitterV(_sessionSplitter, editor);
    _mainSplitter->setStretchFactor(0, 3);
    _mainSplitter->setStretchFactor(1, 1);

    LayoutV({makeToolbar(), _mainSplitter,}).setMargin(3).setSpacing(0).useFor(this);

    _lua = new Z::Lua;
    if (reopenLua()) // should be opened to load global vars from prev session
        restoreState();

    Ori::Wnd::moveToScreenCenter(this);

    _editor->setFocus();
}

CalculatorWindow::~CalculatorWindow()
{
    if (_lua)
    {
        storeState();
        delete _lua;
    }
    __instance = nullptr;
}

bool CalculatorWindow::reopenLua()
{
    QString res = _lua->open();
    if (res.isEmpty())
        return true;

    // Disable action those can refer to _lua
    _actnCalc->setEnabled(false);
    _actnClear->setEnabled(false);

    showError(res);
    delete _lua;
    _lua = nullptr;
    return false;
}

QWidget* CalculatorWindow::makeToolbar()
{
#define A_ Ori::Gui::action

    _actnCalc = A_(tr("Calculate"), this, SLOT(calculate()), ":/toolbar/equals", Qt::CTRL | Qt::Key_Return);
    _actnClear = A_(tr("Clear Session"), this, SLOT(clearLog()), ":/toolbar/delete_items");

    auto actnReuse = A_(tr("Reuse Selected<br>(<b>Ctrl + D</b>)"), this,
        SLOT(reuseItem()), ":/toolbar/duplicate_page", Qt::CTRL + Qt::Key_D);

    auto actnSettings = A_(tr("Settings"), this, SLOT(showSettings()), ":/toolbar/settings");

    auto buttonCalc = Ori::Gui::textToolButton(_actnCalc);
    buttonCalc->setToolTip(tr("Calculate<br>(<b>Ctrl + Enter</b>)"));

    return Z::Gui::makeToolbar({
        buttonCalc, _actnClear, nullptr, actnReuse, nullptr, actnSettings
    }, "calc_formula");

#undef A_
}

void CalculatorWindow::adjustFont()
{
    auto f = Z::Gui::CodeEditorFont().get();
    if (_overrideFont && !_overrideFontName.isEmpty() && _overrideFontSize > 0)
    {
        f.setFamily(_overrideFontName);
        f.setPointSize(_overrideFontSize);
    }
    _editor->setFont(f);
    _logView->setFont(f);
    _varsView->setFont(f);
}

void CalculatorWindow::showError(const QString& error)
{
    _errorView->setText(error);
    _errorView->setVisible(true);
}

void CalculatorWindow::showResult(const QString &code, double result)
{
    LogItem item;
    item.code = code;
    item.result = result;
    _log.append(item);

    auto codeHtml = QString(code);
    codeHtml.replace('\n', QStringLiteral("<br>"));

    _logView->appendHtml(QStringLiteral(
        "<p style='color:#669'>%1"
        "<p style='font-weight:bold'>&nbsp;&nbsp;&nbsp;%2"
        "<p style='font-size:6px'>&nbsp;"
    ).arg(codeHtml).arg(result));

    auto c = _logView->textCursor();
    c.movePosition(QTextCursor::End);
    _logView->setTextCursor(c);
}

void CalculatorWindow::calculate()
{
    auto code = _editor->toPlainText();
    if (code.isEmpty()) return;

    auto res = _lua->calculate(code);
    if (res.ok())
    {
        showResult(code, res.value());
        populateVars();
    }
    else
        showError(res.error());
}

void CalculatorWindow::restoreState()
{
    QJsonObject root = CustomDataHelpers::loadCustomData("calc");

    CustomDataHelpers::restoreWindowSize(root, this, 600, 400);

    // Restore font
    _overrideFont = root["override_font"].toBool();
    _overrideFontName = root["override_font_name"].toString();
    _overrideFontSize = root["override_font_size"].toInt();
    adjustFont();

    // Restore splitters
    int logH = root["log_height"].toInt();
    int editorH = root["editor_height"].toInt();
    if (logH > 0 and editorH > 0)
        _mainSplitter->setSizes({logH, editorH});

    int logW = root["log_width"].toInt();
    int varsW = root["vars_width"].toInt();
    if (logW > 0 and varsW > 0)
        _sessionSplitter->setSizes({logW, varsW});

    // Restore calculation history
    auto log = root["log"].toArray();
    for (auto it = log.begin(); it != log.end(); it++)
    {
        auto item = (*it).toObject();
        auto code = item["code"].toString();
        auto result = item["result"].toDouble();
        if (!code.isEmpty())
            showResult(code, result);
    }

    // Restore session vars
    QJsonObject varsJson = root["vars"].toObject();
    for (auto key : varsJson.keys())
        _lua->setGlobalVar(key, varsJson[key].toDouble());
    populateVars();
}

void CalculatorWindow::storeState()
{
    QJsonObject root;

    CustomDataHelpers::storeWindowSize(root, this);

    // Store font
    root["override_font"] = _overrideFont;
    root["override_font_name"] = _overrideFontName;
    root["override_font_size"] = _overrideFontSize;

    // Store splitters
    auto sizes = _mainSplitter->sizes();
    root["log_height"] = sizes.at(0);
    root["editor_height"] = sizes.at(1);

    sizes = _sessionSplitter->sizes();
    root["log_width"] = sizes.at(0);
    root["vars_width"] = sizes.at(1);

    // Store calculation history
    QJsonArray log;
    for (auto item : _log)
        log.append(QJsonObject({
            {"code", item.code},
            {"result", item.result}
        }));
    root["log"] = log;

    // Store session vars
    QJsonObject varsJson;
    QMap<QString, double> vars = _lua->getGlobalVars();
    QMapIterator<QString, double> it(vars);
    while (it.hasNext())
    {
        it.next();
        varsJson[it.key()] = it.value();
    }
    root["vars"] = varsJson;

    CustomDataHelpers::saveCustomData(root, "calc");
}

void CalculatorWindow::clearLog()
{
    if (!Ori::Dlg::yes(tr("This will erase all log items and variables. Confirm?"))) return;

    _log.clear();
    _logView->clear();
    _varsView->clear();

    reopenLua();
}

void CalculatorWindow::showSettings()
{
    CalculatorSettingsDlg dlg;
    if (dlg.run()) adjustFont();
}

void CalculatorWindow::reuseItem()
{
    auto text = _logView->textCursor().selectedText().trimmed();
    if (text.isEmpty()) return;

    _editor->setPlainText(text);
    _editor->setFocus();

    auto c = _editor->textCursor();
    c.setPosition(text.length());
    _editor->setTextCursor(c);
}

void CalculatorWindow::populateVars()
{
    _varsView->clearContents();

    QMap<QString, double> vars = _lua->getGlobalVars();
    _varsView->setRowCount(vars.size());
    QMapIterator<QString, double> it(vars);
    int row = 0;
    while (it.hasNext())
    {
        it.next();

        _varsView->setItem(row, 0, new QTableWidgetItem(it.key()));
        _varsView->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
        _varsView->resizeRowToContents(row);

        row++;
    }
    _varsView->resizeColumnToContents(0);
    _varsView->setColumnWidth(0, _varsView->columnWidth(0) + 10);
}
