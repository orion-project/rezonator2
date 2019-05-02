#include "CalculatorWindow.h"

#include "AppSettings.h"
#include "core/LuaHelper.h"
#include "widgets/Appearance.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriWindows.h"
#include "tools/OriSettings.h"
#include "widgets/OriFlatToolBar.h"

#include <QDebug>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPlainTextEdit>
#include <QSplitter>

void CalculatorWindow::showCalcWindow()
{
    (new CalculatorWindow)->show();
}

CalculatorWindow::CalculatorWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Formula Calculator"));
    setWindowIcon(QIcon(":/window_icons/calculator"));

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
    _errorView->setStyleSheet("QLabel{background:red;color:white;font-weight:bold;padding:3px}");
    Z::Gui::setCodeEditorFont(_errorView);

    auto splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(_logView);
    splitter->addWidget(Ori::Layouts::LayoutV({
        _editor,
        _errorView
    }).setMargin(0).setSpacing(0).makeWidget());

    Ori::Layouts::LayoutV({
            makeToolbar(),
            splitter,
        })
        .setMargin(3)
        .setSpacing(0)
        .useFor(this);

    _lua = new Z::Lua;

    restoreState();
    Ori::Wnd::moveToScreenCenter(this);
}

CalculatorWindow::~CalculatorWindow()
{
    storeState();
    delete _lua;
}

QWidget* CalculatorWindow::makeToolbar()
{
#define A_ Ori::Gui::action

    auto actnCalc = A_(tr("Calculate"), this, SLOT(calculate()), ":/toolbar/equals", Qt::CTRL | Qt::Key_Return);

    auto actionHelp = new QAction(QIcon(":/toolbar/help"), tr("Help"), this);
    actionHelp->setEnabled(false); // TODO:NEXT-VER

    auto toolbar = new Ori::Widgets::FlatToolBar;
    toolbar->setIconSize(Settings::instance().toolbarIconSize());
    Ori::Gui::populate(toolbar, {
        actnCalc, nullptr, actionHelp
    });
    return toolbar;

#undef A_
}

void CalculatorWindow::showError(const QString& error)
{
    _errorView->setText(error);
    _errorView->setVisible(true);
}

void CalculatorWindow::showResult(const QString &code, double value)
{
    _logView->appendPlainText(code); // TODO set code style
    _logView->appendPlainText(QString::number(value)); // TODO set result style
}

void CalculatorWindow::calculate()
{
    auto code = _editor->toPlainText();

    auto res = _lua->calculate(code);
    if (res.ok())
        showResult(code, res.value());
    else
        showError(res.error());
}

QString CalculatorWindow::stateFileName()
{
    Ori::Settings s;
    return s.settings()->fileName().section('.', 0, -2) + ".calc.json";
}

void CalculatorWindow::restoreState()
{
    QJsonObject root;

    auto fileName = stateFileName();
    QFile file(fileName);
    if (file.exists())
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            qWarning() << "CalculatorWindow: failed to load state" << fileName << file.errorString();
        else
            root = QJsonDocument::fromJson(file.readAll()).object();
    }

    int w = root["window_width"].toInt();
    int h = root["window_height"].toInt();
    if (w == 0 || h == 0)
    {
        w = 600;
        h = 400;
    }
    resize(w, h);
}

void CalculatorWindow::storeState()
{
    QJsonObject root;
    root["window_width"] = width();
    root["window_height"] = height();

    auto fileName = stateFileName();
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << "CalculatorWindow: failed to save state" << fileName << file.errorString();
        return;
    }
    QTextStream(&file) << QJsonDocument(root).toJson();
}
