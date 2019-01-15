#include "InfoFuncWindow.h"

#include "../core/Schema.h"
#include "../HelpSystem.h"
#include "../funcs/InfoFunctions.h"
#include "../widgets/Appearance.h"
#include "../widgets/FrozenStateButton.h"
#include "widgets/OriFlatToolBar.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriTools.h"
#include "helpers/OriDialogs.h"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QTextBrowser>

static const int WINDOW_W = 540;
static const int WINDOW_H = 260;

void InfoFuncWindow::open(InfoFunction *func, QWidget* parent)
{
    (new InfoFuncWindow(func, parent))->show();
}

InfoFuncWindow::InfoFuncWindow(InfoFunction *func, QWidget *parent) :
    QWidget(parent, Qt::Tool), SchemaToolWindow(func->schema()), _function(func)
{
    setWindowTitle(_function->name());
    setAttribute(Qt::WA_DeleteOnClose);

    _editor = new QTextBrowser;
    _editor->setReadOnly(true);
    _editor->setOpenLinks(false);
    Z::Gui::setValueFont(_editor);
    connect(_editor, SIGNAL(anchorClicked(QUrl)), this, SLOT(linkClicked(QUrl)));

    createToolbar();

    Ori::Layouts::LayoutV({
        toolbar(),
        Ori::Layouts::LayoutV({_editor}).setMargin(3)
    }).setMargin(0).setSpacing(0).useFor(this);

    resize(WINDOW_W, WINDOW_H);
    if (parent)
        move(parent->pos() + parent->rect().center() - rect().center());
    // TODO: iterate through all the information windows and move this if it overlaps some of existed

    updateResultText();

    if (!schema()->state().isLoading())
        processCalc(); // else recalc on schemaLoaded
}

InfoFuncWindow::~InfoFuncWindow()
{
    delete _function;
}

void InfoFuncWindow::createToolbar()
{
    auto toolbar = makeToolBar(true);
    toolbar->addAction(actnUpdate = Ori::Gui::action(tr("Update"), this, SLOT(processCalc()), ":/toolbar/update", Qt::Key_F5));
    toolbar->addAction(actnFreeze = Ori::Gui::toggledAction(tr("Freeze"), this, SLOT(freeze(bool)), ":/toolbar/freeze", Qt::CTRL | Qt::Key_F));
    actnFrozenInfo = toolbar->addWidget(buttonFrozenInfo = new FrozenStateButton(tr("Frozen info"), "frozen_info"));
    toolbar->addSeparator();
    toolbar->addAction(Ori::Gui::action(tr("Copy"), _editor, SLOT(copy()), ":/toolbar/copy", Qt::Key_Copy));
    auto actnCopyAll = Ori::Gui::action(tr("Copy All"), this, SLOT(copyAll()), ":/toolbar/copy_all");
    toolbar->addWidget(Ori::Gui::textToolButton(actnCopyAll));
    toolbar->addSeparator();
    toolbar->addAction(Ori::Gui::action(tr("Help"), this, SLOT(help()), ":/toolbar/help", Qt::Key_Help));
}

void InfoFuncWindow::updateFrozenInfo()
{
    if (_frozen)
        buttonFrozenInfo->setInfo(InfoFuncSummary(schema()).calculate());
}

void InfoFuncWindow::updateResultText()
{
    _editor->setHtml(QStringLiteral("<body bgcolor=\"%1\">%2</body>").arg(paperColor(), _result));
}

QString InfoFuncWindow::paperColor()
{
    QPalette p;
    QColor color = p.color(QPalette::ToolTipBase);
    if (_frozen) color = Ori::Color::blend(color, p.color(QPalette::Window), 0.5);
    return color.name();
}

void InfoFuncWindow::elementDeleting(Schema*, Element *elem)
{
    switch (_function->elementDeleting(elem))
    {
    case FunctionBase::Frozen:
        freeze(true);
        actnFreeze->setEnabled(false);
        actnFreeze->setVisible(false);
        break;

    case FunctionBase::Dead:
        close();
        break;

    default:
        processCalc();
        break;
    }
}

void InfoFuncWindow::processCalc()
{
    if (!_frozen)
    {
        _result = _function->calculate();
        updateResultText();
    }
    else
        _needRecalc = true;
}

void InfoFuncWindow::freeze(bool frozen)
{
    _frozen = frozen;
    actnUpdate->setEnabled(!_frozen);
    actnFrozenInfo->setVisible(_frozen);
    updateFrozenInfo();
    if (!_frozen and _needRecalc)
        processCalc();
    else updateResultText();
}

void InfoFuncWindow::help()
{
    Z::Help::show(_function->helpTopic());
}

void InfoFuncWindow::linkClicked(const QUrl& url)
{
    if (url.scheme() == "func" && url.host() == "viewmatrix")
    {
        int id = Ori::Tools::getParamInt(url, "elem");
        if (id > 0)
        {
            Element *elem = schema()->elementById(id);
            if (elem)
                return open(new InfoFuncMatrix(schema(), elem), parentWidget());
        }
        Ori::Dlg::error(tr("This element does not exist in the schema anymore."));
    }
}

void InfoFuncWindow::copyAll()
{
    qApp->clipboard()->setText(_editor->toHtml());
}
