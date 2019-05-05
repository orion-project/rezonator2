#include "CalculatorWindow.h"

#include "AppSettings.h"
#include "CustomPrefs.h"
#include "core/LuaHelper.h"
#include "widgets/Appearance.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriWindows.h"
#include "widgets/OriFlatToolBar.h"

#include <QCheckBox>
#include <QDebug>
#include <QFontDialog>
#include <QGroupBox>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QToolButton>

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

    bool overrideFont = __instance->_overrideFont;

    _overrideFontFlag = new QCheckBox(tr("Override default font"));
    _overrideFontFlag->setChecked(overrideFont);
    connect(_overrideFontFlag, &QCheckBox::stateChanged, this, &CalculatorSettingsDlg::overrideFontClicked);

    _chooseFontButton = new QPushButton(tr("Choose font..."));
    _chooseFontButton->setEnabled(overrideFont);
    connect(_chooseFontButton, &QPushButton::clicked, this, &CalculatorSettingsDlg::chooseFontClicked);

    _fontSampleLabel = new QLabel("L / math.sqrt(n^2 - math.sin(a)^2)");
    _fontSampleLabel->setStyleSheet("background-color:white;padding:6px");
    _fontSampleLabel->setFrameShape(QFrame::StyledPanel);
    _fontSampleLabel->setEnabled(overrideFont);
    auto f = _fontSampleLabel->font();
    Z::Gui::adjustCodeEditorFont(f);
    f.setFamily(__instance->_overrideFontName);
    f.setPointSize(__instance->_overrideFontSize);
    _fontSampleLabel->setFont(f);

    auto layout = Ori::Layouts::LayoutV({
        _overrideFontFlag, _chooseFontButton, _fontSampleLabel
    }).setMargin(0).boxLayout();

    mainLayout()->addLayout(layout);
    mainLayout()->addSpacing(6);
}

void CalculatorSettingsDlg::collect()
{
    __instance->_overrideFont = _overrideFontFlag->isChecked();
    __instance->_overrideFontName = _fontSampleLabel->font().family();
    __instance->_overrideFontSize = _fontSampleLabel->font().pointSize();
    accept();
    close();
}

void CalculatorSettingsDlg::overrideFontClicked(int)
{
    _chooseFontButton->setEnabled(_overrideFontFlag->isChecked());
    _fontSampleLabel->setEnabled(_overrideFontFlag->isChecked());
}

void CalculatorSettingsDlg::chooseFontClicked()
{
    bool ok = false;
    auto f = QFontDialog::getFont(&ok, _fontSampleLabel->font(), this, tr("Choose Font"), QFontDialog::DontUseNativeDialog);
    if (ok) _fontSampleLabel->setFont(f);
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
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Formula Calculator"));
    setWindowIcon(QIcon(":/window_icons/calculator"));

    // TODO: make custom items widget using _log as a model
    _logView = new QPlainTextEdit;
    _logView->setReadOnly(true);
    _logView->setPlaceholderText(tr("Calculation results are displayed here"));

    _editor = new QPlainTextEdit;
    _editor->setWordWrapMode(QTextOption::WordWrap);
    _editor->setPlaceholderText(tr("Enter your formula here and\nhit Ctrl+Enter to calculate"));
    connect(_editor, &QPlainTextEdit::textChanged, [this](){
       _errorView->setVisible(false);
    });

    _errorView = new QLabel;
    _errorView->setVisible(false);
    _errorView->setWordWrap(true);
    _errorView->setStyleSheet("QLabel{background:red;color:white;font-weight:bold;padding:3px}");

    auto editor = Ori::Layouts::LayoutV({
        _editor, _errorView}).setMargin(0).setSpacing(0).makeWidget();
    _splitter = Ori::Gui::splitterV(_logView, editor);

    Ori::Layouts::LayoutV({makeToolbar(), _splitter,}).setMargin(3).setSpacing(0).useFor(this);

    _lua = new Z::Lua;

    restoreState();
    Ori::Wnd::moveToScreenCenter(this);

    _editor->setFocus();
}

CalculatorWindow::~CalculatorWindow()
{
    storeState();
    delete _lua;
    __instance = nullptr;
}

QWidget* CalculatorWindow::makeToolbar()
{
#define A_ Ori::Gui::action

    auto actnCalc = A_(tr("Calculate"), this, SLOT(calculate()), ":/toolbar/equals", Qt::CTRL | Qt::Key_Return);
    auto actnClear = A_(tr("Clear Log"), this, SLOT(clearLog()), ":/toolbar/delete_items");

    auto actnSettings = A_(tr("Settings"), this, SLOT(showSettings()), ":/toolbar/settings");
    auto actionHelp = new QAction(QIcon(":/toolbar/help"), tr("Help"), this);
    actionHelp->setEnabled(false); // TODO:NEXT-VER

    auto buttonCalc = Ori::Gui::textToolButton(actnCalc);
    buttonCalc->setToolTip(tr("Calculate<br>(<b>Ctrl + Enter</b>)"));

    auto toolbar = new Ori::Widgets::FlatToolBar;
    toolbar->setIconSize(Settings::instance().toolbarIconSize());
    Ori::Gui::populate(toolbar, {
        buttonCalc, actnClear, nullptr,
        actnSettings, actionHelp
    });
    return toolbar;
#undef A_
}

void CalculatorWindow::adjustFont()
{
    auto f = font();
    Z::Gui::adjustCodeEditorFont(f);
    if (_overrideFont && !_overrideFontName.isEmpty() && _overrideFontSize > 0)
    {
        f.setFamily(_overrideFontName);
        f.setPointSize(_overrideFontSize);
    }
    _editor->setFont(f);
    _logView->setFont(f);
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

    _logView->appendHtml(QStringLiteral(
        "<p style='color:#444'>%1"
        "<p style='font-weight:bold'>&nbsp;&nbsp;&nbsp;%2"
        "<p style='font-size:6px'>&nbsp;"
    ).arg(code).arg(result));
}

void CalculatorWindow::calculate()
{
    auto code = _editor->toPlainText();
    if (code.isEmpty()) return;

    auto res = _lua->calculate(code);
    if (res.ok())
        showResult(code, res.value());
    else
        showError(res.error());
}

void CalculatorWindow::restoreState()
{
    QJsonObject root = CustomDataHelpers::loadCustomData("calc");

    CustomDataHelpers::restoreWindowSize(root, this, 600, 400);

    _overrideFont = root["override_font"].toBool();
    _overrideFontName = root["override_font_name"].toString();
    _overrideFontSize = root["override_font_size"].toInt();
    adjustFont();

    int logH = root["log_height"].toInt();
    int editorH = root["editor_height"].toInt();
    if (logH > 0 && editorH > 0)
        _splitter->setSizes({logH, editorH});

    auto log = root["log"].toArray();
    for (auto it = log.begin(); it != log.end(); it++)
    {
        auto item = (*it).toObject();
        auto code = item["code"].toString();
        auto result = item["result"].toDouble();
        if (!code.isEmpty())
            showResult(code, result);
    }
}

void CalculatorWindow::storeState()
{
    QJsonObject root;

    CustomDataHelpers::storeWindowSize(root, this);

    root["override_font"] = _overrideFont;
    root["override_font_name"] = _overrideFontName;
    root["override_font_size"] = _overrideFontSize;

    auto sizes = _splitter->sizes();
    root["log_height"] = sizes.at(0);
    root["editor_height"] = sizes.at(1);

    QJsonArray log;
    for (auto item : _log)
        log.append(QJsonObject({
            {"code", item.code},
            {"result", item.result}
        }));
    root["log"] = log;

    CustomDataHelpers::saveCustomData(root, "calc");
}

void CalculatorWindow::clearLog()
{
    if (!Ori::Dlg::yes(tr("Do you confirm the deletion of all log items?"))) return;

    _log.clear();
    _logView->clear();
}

void CalculatorWindow::showSettings()
{
    CalculatorSettingsDlg dlg;
    if (dlg.run()) adjustFont();
}
