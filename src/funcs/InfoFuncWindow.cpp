#include "InfoFuncWindow.h"

#include "../app/Appearance.h"
#include "../app/HelpSystem.h"
#include "../core/Schema.h"
#include "../math/InfoFunctions.h"
#include "../widgets/FrozenStateButton.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriTools.h"
#include "helpers/OriDialogs.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QTextBrowser>
#include <QToolBar>

static const int WINDOW_W = 540;
static const int WINDOW_H = 260;

void InfoFuncWindow::open(InfoFunction *func, QWidget* parent)
{
    (new InfoFuncWindow(func, parent))->show();
}

InfoFuncWindow::InfoFuncWindow(InfoFunction *func, QWidget *parent) :
    ShortcutHandlerWindow(parent, Qt::Tool), SchemaToolWindow(func->schema()), _function(func)
{
    setWindowTitle(_function->name());
    setAttribute(Qt::WA_DeleteOnClose);

    _editor = new QTextBrowser;
    _editor->setReadOnly(true);
    _editor->setOpenLinks(false);
    _editor->setFont(Z::Gui::ValueFont().get());
    _editor->document()->setDefaultStyleSheet(Z::Gui::reportStyleSheet());
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

    _function->registerListener(this);

    if (!schema()->state().isLoading())
        processCalc(); // else recalc on schemaLoaded
}

InfoFuncWindow::~InfoFuncWindow()
{
    if (_function)
    {
        _function->unregisterListener(this);
        delete _function;
    }
}

void InfoFuncWindow::createToolbar()
{
    actnUpdate = Ori::Gui::V0::action(tr("Update"), this, SLOT(processCalc()), ":/toolbar/update", QKeySequence::Refresh);
    actnFreeze = Ori::Gui::V0::toggledAction(tr("Freeze"), this, SLOT(freeze(bool)), ":/toolbar/freeze", QKeySequence::Find);
    auto actnCopy = Ori::Gui::V0::action(tr("Copy"), _editor, SLOT(copy()), ":/toolbar/copy", QKeySequence::Copy);
    auto actnCopyAll = Ori::Gui::V0::action(tr("Copy All"), this, SLOT(copyAll()), ":/toolbar/copy_all");
    buttonFrozenInfo = new FrozenStateButton(tr("Frozen info"), "frozen_info");

    registerShortcut(QKeySequence::Refresh, actnUpdate);
    registerShortcut(QKeySequence::Find, actnFreeze);
    registerShortcut(QKeySequence::Copy, actnCopy);

    auto toolbar = makeToolBar(true);
    toolbar->addAction(actnUpdate);
    toolbar->addAction(actnFreeze);
    actnFrozenInfo = toolbar->addWidget(buttonFrozenInfo);
    toolbar->addSeparator();
    toolbar->addAction(actnCopy);
    toolbar->addWidget(Ori::Gui::textToolButton(actnCopyAll));
    toolbar->addSeparator();

    if (!_function->actions().empty())
    {
        QToolButton *actionsButton = nullptr;
        foreach (const InfoFuncAction& a, _function->actions())
        {
            auto actn = new QAction(a.title, this);
            if (!a.icon.isEmpty())
                actn->setIcon(QIcon(a.icon));
            if (a.isChecked)
            {
                actn->setCheckable(true);
                actn->setChecked(a.isChecked());
                if (a.checkGroup)
                {
                    if (!_actionGroups.contains(a.checkGroup))
                        _actionGroups[a.checkGroup] = new QActionGroup(this);
                    actn->setActionGroup(_actionGroups[a.checkGroup]);
                }
            }
            connect(actn, &QAction::triggered, a.triggered);
            if (a.showInMenu) {
                if (!actionsButton) {
                    actionsButton = new QToolButton;
                    actionsButton->setPopupMode(QToolButton::InstantPopup);
                    actionsButton->setIcon(QIcon(":/toolbar/options"));
                    actionsButton->setToolTip(tr("Options"));
                }
                actionsButton->addAction(actn);
            } else {
                toolbar->addAction(actn);
            }
        }
        if (actionsButton)
            toolbar->addWidget(actionsButton);
        toolbar->addSeparator();
    }

    if (!_function->helpTopic().isEmpty())
    {
        auto actnHelp = Ori::Gui::V0::action(tr("Help"), this, SLOT(help()), ":/toolbar/help", QKeySequence::HelpContents);
        registerShortcut(QKeySequence::HelpContents, actnHelp);
        toolbar->addAction(actnHelp);
    }
}

void InfoFuncWindow::updateFrozenInfo()
{
    InfoFuncSummary summary(schema());
    summary.calculate();
    buttonFrozenInfo->setInfo(summary.result());
}

void InfoFuncWindow::functionCalculated(FunctionBase*)
{
    QColor pageColor = _function->frozen() ? Z::Gui::midPaperColor() : Z::Gui::yellowPaperColor();
    _editor->setHtml(QStringLiteral("<body bgcolor=%1>%2</body>").arg(pageColor.name(), _function->result()));
}

void InfoFuncWindow::functionDeleted(FunctionBase*)
{
    _function = nullptr;
    close();
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
    _function->calculate();
}

void InfoFuncWindow::freeze(bool frozen)
{
    actnUpdate->setEnabled(!frozen);
    actnFrozenInfo->setVisible(frozen);
    if (frozen) updateFrozenInfo();
    _function->freeze(frozen);
    functionCalculated(_function); // update back color
}

void InfoFuncWindow::help()
{
    Z::HelpSystem::instance()->showTopic(_function->helpTopic());
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
