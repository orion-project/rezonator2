#include "SchemaWindows.h"
#include "WindowsManager.h"
#include "widgets/OriFlatToolBar.h"
#include "helpers/OriWidgets.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QTimer>
#include <QToolButton>

namespace Z {
namespace WindowUtils {

QSize toolbarIconSize()
{
    return AppSettings::instance().smallToolbarImages? QSize(16,16): QSize(24,24);
}

void adjustIconSize(QToolBar* toolbar)
{
    adjustIconSize(toolbar, toolbarIconSize());
}

void adjustIconSize(QToolBar* toolbar, const QSize& iconSize)
{
    toolbar->setIconSize(iconSize);

    for (auto a: toolbar->actions())
    {
        auto w = toolbar->widgetForAction(a);
        auto b = qobject_cast<QToolButton*>(w);
        if (b) b->setIconSize(iconSize);
    }

    toolbar->adjustSize();
}

} // namespace WindowUtils
} // namespace Z

//------------------------------------------------------------------------------
//                               IEditableWindow
//------------------------------------------------------------------------------

IEditableWindow::~IEditableWindow()
{
}

//------------------------------------------------------------------------------
//                               IPrintableWindow
//------------------------------------------------------------------------------

IPrintableWindow::~IPrintableWindow()
{
}

//------------------------------------------------------------------------------
//                               SchemaWindow
//------------------------------------------------------------------------------

SchemaWindow::SchemaWindow(Schema *owner) : _schema(owner)
{
    WindowsManager::instance().registerWindow(_schema, this);
    if (_schema) _schema->registerListener(this);
}

SchemaWindow::~SchemaWindow()
{
    WindowsManager::instance().unregisterWindow(_schema, this);
    if (_schema) _schema->unregisterListener(this);
}

//------------------------------------------------------------------------------
//                               SchemaToolWindow
//------------------------------------------------------------------------------

SchemaToolWindow::SchemaToolWindow(Schema *owner, InitOptions options) : SchemaWindow(owner)
{
    Q_UNUSED(options)
}

QToolBar* SchemaToolWindow::makeToolBar(const QString& title, bool flat)
{
    QToolBar* toolbar = flat? new Ori::Widgets::FlatToolBar(title): new QToolBar(title);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setIconSize(Z::WindowUtils::toolbarIconSize());
    toolbar->setObjectName("toolBar_" % title);
    _toolbars << toolbar;
    return toolbar;
}

QToolBar* SchemaToolWindow::makeToolBar(const QString& title, const std::initializer_list<QObject*>& items, bool flat)
{
    return Ori::Gui::populate(makeToolBar(title, flat), items);
}

QToolBar* SchemaToolWindow::makeToolBar(bool flat)
{
    return makeToolBar(QString(), flat);
}

QToolBar* SchemaToolWindow::makeToolBar(const std::initializer_list<QObject*>& items, bool flat)
{
    return makeToolBar(QString(), items, flat);
}

void SchemaToolWindow::settingsChanged()
{
    for (auto toolbar: _toolbars)
        Z::WindowUtils::adjustIconSize(toolbar);
}

//------------------------------------------------------------------------------
//                               BasicMdiChild
//------------------------------------------------------------------------------

BasicMdiChild::BasicMdiChild(InitOptions options) : QMdiSubWindow()
{
    setAttribute(Qt::WA_DeleteOnClose);

    if (!options.testFlag(initNoDefaultWidget))
    {
        setWidget(new QWidget);
        _layout = new QVBoxLayout(widget());
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setSpacing(0);

        if (!options.testFlag(initNoToolBar))
        {
            _toolbar = new Ori::Widgets::FlatToolBar;
            _toolbar->setIconSize(Z::WindowUtils::toolbarIconSize());
            _layout->addWidget(_toolbar);
        }
    }


}

void BasicMdiChild::populateToolbar(std::initializer_list<QObject*> items)
{
    Ori::Gui::populate(_toolbar, items);
}

void BasicMdiChild::setTitleAndIcon(const QString& title, const char* iconPath)
{
#ifdef Q_OS_MACOS
    // On MacOS the title and icon of MDI-child are tightly glued together
    // and that looks ugly, so we simulate intendation with space
    setWindowTitle(" " + title);
#else
    setWindowTitle(title);
#endif
    setWindowIcon(QIcon(iconPath));
}

void BasicMdiChild::setContent(QWidget *content, int row)
{
    if (content)
    {
        if (_layout)
            _layout->insertWidget(row, content);
        else
            setWidget(content);
    }
}

void BasicMdiChild::settingsChanged()
{
    if (_toolbar)
        Z::WindowUtils::adjustIconSize(_toolbar);
}

//------------------------------------------------------------------------------
//                               SchemaMdiChild
//------------------------------------------------------------------------------

SchemaMdiChild::SchemaMdiChild(Schema *schema, InitOptions options) : BasicMdiChild(options), SchemaWindow(schema)
{

}

//------------------------------------------------------------------------------
//                               SchemaMdiArea
//------------------------------------------------------------------------------

SchemaMdiArea::SchemaMdiArea(QWidget *parent): QMdiArea(parent)
{
    updateBackground();
}

BasicMdiChild* SchemaMdiArea::activeChild() const
{
    return dynamic_cast<BasicMdiChild*>(activeSubWindow());
}

IEditableWindow* SchemaMdiArea::activeEditableChild() const
{
    return dynamic_cast<IEditableWindow*>(activeSubWindow());
}

IPrintableWindow* SchemaMdiArea::activePrintableChild() const
{
    return dynamic_cast<IPrintableWindow*>(activeSubWindow());
}

void SchemaMdiArea::appendChild(QWidget* window)
{
    appendChild(qobject_cast<BasicMdiChild*>(window));
}

void SchemaMdiArea::appendChild(BasicMdiChild* window)
{
    if (window)
    {
        if (!subWindowList().contains(window))
        {
            addSubWindow(window);
            window->init();
            window->show();
        }
        setActiveSubWindow(window);

#ifdef Q_OS_LINUX
        // WORKAROUND: For some reasons, on Ubuntu (Unity and xfce) previous subwindow
        // is activated again after this window was activated. So we have to reactivate
        // it one more time slightly later. Reactivating can't help if done immediately,
        // but there is ugly flikering if delay is too long, should be better solution.
        QTimer::singleShot(100, [window, this](){ setActiveSubWindow(window); });
#endif
    }
}


void SchemaMdiArea::activateChild()
{
    auto action = qobject_cast<QAction*>(sender());
    auto window = reinterpret_cast<BasicMdiChild*>(action->data().value<void*>());
    activateChild(window);
}

void SchemaMdiArea::activateChild(QWidget *window)
{
    activateChild(qobject_cast<BasicMdiChild*>(window));
}

void SchemaMdiArea::activateChild(BasicMdiChild* window)
{
    if (window)
    {
        if (window->windowState() & Qt::WindowMinimized)
            window->showNormal();
        else if (!window->isVisible())
            window->show();
        setActiveSubWindow(window);
    }
}

void SchemaMdiArea::editableChild_Undo()
{
    IEditableWindow *w = dynamic_cast<IEditableWindow*>(activeSubWindow());
    if (w && w->canUndo()) w->undo();
}

void SchemaMdiArea::editableChild_Redo()
{
    IEditableWindow *w = dynamic_cast<IEditableWindow*>(activeSubWindow());
    if (w && w->canRedo()) w->redo();
}

void SchemaMdiArea::editableChild_Cut()
{
    IEditableWindow *w = dynamic_cast<IEditableWindow*>(activeSubWindow());
    if (w && w->canCut()) w->cut();
}

void SchemaMdiArea::editableChild_Copy()
{
    IEditableWindow *w = dynamic_cast<IEditableWindow*>(activeSubWindow());
    if (w && w->canCopy()) w->copy();
}

void SchemaMdiArea::editableChild_Paste()
{
    IEditableWindow *w = dynamic_cast<IEditableWindow*>(activeSubWindow());
    if (w && w->canPaste()) w->paste();
}

void SchemaMdiArea::editableChild_SelectAll()
{
    IEditableWindow *w = dynamic_cast<IEditableWindow*>(activeSubWindow());
    if (w) w->selectAll();
}

void SchemaMdiArea::printableChild_SendToPrinter()
{
    auto w = dynamic_cast<IPrintableWindow*>(activeSubWindow());
    if (w) w->sendToPrinter();
}

void SchemaMdiArea::printableChild_PrintPreview()
{
    auto w = dynamic_cast<IPrintableWindow*>(activeSubWindow());
    if (w) w->printPreview();
}

void SchemaMdiArea::populateWindowMenu()
{
    QMenu* menu = qobject_cast<QMenu*>(sender());
    if (!menu) return;

    auto subWindows = subWindowList();

    // Remove action of windows that are no more exist
    Q_FOREACH (auto action, menu->actions())
    {
        if (action->data().isNull()) continue;
        // If we interpret variant data as QMdiSubWindow*, Qt seems to call for some of their
        // methods, and app is crashed because of window is already deleted. So use plain void*.
        auto window = reinterpret_cast<QMdiSubWindow*>(action->data().value<void*>());
        if (subWindows.contains(window)) continue;
        menu->removeAction(action);
        delete action;
    }

    // Append actions for windows that are not in menu
    auto actions = menu->actions();
    auto activeWindow = activeSubWindow();
    for (auto window : subWindows)
    {
        if (!window->isVisible()) continue;

        bool windowInMenu = false;
        for (auto action: actions)
            if (action->data().value<void*>() == window)
            {
                action->setChecked(window == activeWindow);
                windowInMenu = true;
                break;
            }
        if (windowInMenu) continue;

        auto action = menu->addAction(window->windowTitle(), this, SLOT(activateChild()));
        action->setCheckable(true);
        action->setChecked(window == activeWindow);
        action->setData(QVariant::fromValue<void*>(window));
    }
}

void SchemaMdiArea::settingsChanged()
{
    updateBackground();
}

void SchemaMdiArea::updateBackground()
{
    if (AppSettings::instance().showBackground)
        setBackground(QBrush(QPixmap(":/misc/mdi_background")));
    else
        setBackground(QBrush(palette().color(QPalette::Window)));
}

//------------------------------------------------------------------------------
//                               SchemaPopupWindow
//------------------------------------------------------------------------------

SchemaPopupWindow::SchemaPopupWindow(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
}

void SchemaPopupWindow::registerShortcut(QKeySequence::StandardKey stdKey, QAction*action)
{
    _actionShortcuts.append(stdKey);
    _actionWithShortcuts.insert(stdKey, action);
}

bool SchemaPopupWindow::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride)
    {
        auto keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent)
        {
            Q_FOREACH(auto shortcut, _actionShortcuts)
            {
                if (keyEvent->matches(shortcut))
                {
                    keyEvent->accept();
                    return true;
                }
            }
        }
    }
    return QWidget::event(event);
}

void SchemaPopupWindow::keyPressEvent(QKeyEvent *event)
{
    Q_FOREACH (auto shortcut, _actionShortcuts)
    {
        if (event->matches(shortcut))
        {
            _actionWithShortcuts[shortcut]->trigger();
            event->accept();
            return;
        }
    }
    QWidget::keyPressEvent(event);
}

//------------------------------------------------------------------------------
//                               IShortcutListener
//------------------------------------------------------------------------------

IShortcutListener::~IShortcutListener()
{
}
