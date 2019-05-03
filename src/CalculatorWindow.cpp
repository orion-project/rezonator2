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

#include <QDebug>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QToolButton>

namespace  {

CalculatorWindow* __instance = nullptr;

} //  namespace

void CalculatorWindow::showWindow()
{
    if (!__instance)
        __instance = new CalculatorWindow;
    __instance->show();
}

CalculatorWindow::CalculatorWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Formula Calculator"));
    setWindowIcon(QIcon(":/window_icons/calculator"));

    // TODO: make custom items widget using _log as a model
    _logView = new QPlainTextEdit;
    _logView->setReadOnly(true);
    Z::Gui::setCodeEditorFont(_logView);

    _editor = new QPlainTextEdit;
    _editor->setWordWrapMode(QTextOption::WordWrap);
    connect(_editor, &QPlainTextEdit::textChanged, [this](){
       _errorView->setVisible(false);
    });
    Z::Gui::setCodeEditorFont(_editor);

    _errorView = new QLabel;
    _errorView->setVisible(false);
    _errorView->setWordWrap(true);
    _errorView->setStyleSheet("QLabel{background:red;color:white;font-weight:bold;padding:3px}");
    Z::Gui::setCodeEditorFont(_errorView);

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

    auto actionHelp = new QAction(QIcon(":/toolbar/help"), tr("Help"), this);
    actionHelp->setEnabled(false); // TODO:NEXT-VER

    auto toolbar = new Ori::Widgets::FlatToolBar;
    toolbar->setIconSize(Settings::instance().toolbarIconSize());
    Ori::Gui::populate(toolbar, {
        Ori::Gui::textToolButton(actnCalc), actnClear, nullptr, actionHelp
    });
    return toolbar;
#undef A_
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
        "<p>&nbsp;"
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
