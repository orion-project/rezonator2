#include "SchemaWindows.h"
#include "WindowsManager.h"
#include "widgets/OriFlatToolBar.h"
#include "helpers/OriWidgets.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QMenu>
#include <QToolButton>

////////////////////////////////////////////////////////////////////////////////
//                               SchemaWindow
////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

QSize toolbarIconSize()
{
    return Settings::instance().smallToolbarImages? QSize(16,16): QSize(24,24);
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

////////////////////////////////////////////////////////////////////////////////
//                               SchemaToolWindow
////////////////////////////////////////////////////////////////////////////////

SchemaToolWindow::SchemaToolWindow(Schema *owner) : SchemaWindow(owner)
{
}

QToolBar* SchemaToolWindow::makeToolBar(const QString& title, bool flat)
{
    QToolBar* toolbar = flat? new Ori::Widgets::FlatToolBar(title): new QToolBar(title);
    toolbar->setIconSize(toolbarIconSize());
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
    auto iconSize = toolbarIconSize();
    for (auto toolbar: _toolbars)
        adjustIconSize(toolbar, iconSize);
}

////////////////////////////////////////////////////////////////////////////////
//                               BasicMdiChild
////////////////////////////////////////////////////////////////////////////////

BasicMdiChild::BasicMdiChild(InitOptions options) : QMdiSubWindow()
{
    setAttribute(Qt::WA_DeleteOnClose);

    if (!options.testFlag(initNoDefaultWidget))
    {
        setWidget(new QWidget);
        _layout = new QVBoxLayout(widget());
        _layout->setMargin(0);
        _layout->setSpacing(0);

        if (!options.testFlag(initNoToolBar))
        {
            _toolbar = new Ori::Widgets::FlatToolBar;
            _toolbar->setIconSize(toolbarIconSize());
            _layout->addWidget(_toolbar);
        }
    }


}

void BasicMdiChild::populateToolbar(std::initializer_list<QObject*> items)
{
    Ori::Gui::populate(_toolbar, items);
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
        adjustIconSize(_toolbar, toolbarIconSize());
}

////////////////////////////////////////////////////////////////////////////////
//                               SchemaMdiChild
////////////////////////////////////////////////////////////////////////////////

SchemaMdiChild::SchemaMdiChild(Schema *schema, InitOptions options) : BasicMdiChild(options), SchemaWindow(schema)
{

}

////////////////////////////////////////////////////////////////////////////////
//                               SchemaMdiArea
////////////////////////////////////////////////////////////////////////////////

SchemaMdiArea::SchemaMdiArea(QWidget *parent): QMdiArea(parent)
{
    updateBackground();
}

BasicMdiChild* SchemaMdiArea::activeChild() const
{
    return dynamic_cast<BasicMdiChild*>(activeSubWindow());
}

EditableWindow* SchemaMdiArea::activeEditableChild() const
{
    return dynamic_cast<EditableWindow*>(activeSubWindow());
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
    }
}


void SchemaMdiArea::activateChild()
{
    activateChild(qobject_cast<QAction*>(sender())->data().value<QWidget*>());
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


void SchemaMdiArea::editableChild_Cut()
{
    EditableWindow *w = dynamic_cast<EditableWindow*>(activeSubWindow());
    if (w && w->canCut()) w->cut();
}

void SchemaMdiArea::editableChild_Copy()
{
    EditableWindow *w = dynamic_cast<EditableWindow*>(activeSubWindow());
    if (w && w->canCopy()) w->copy();
}

void SchemaMdiArea::editableChild_Paste()
{
    EditableWindow *w = dynamic_cast<EditableWindow*>(activeSubWindow());
    if (w && w->canPaste()) w->paste();
}

void SchemaMdiArea::editableChild_SelectAll()
{
    EditableWindow *w = dynamic_cast<EditableWindow*>(activeSubWindow());
    if (w) w->selectAll();
}

void SchemaMdiArea::populateMenu(QMenu* menu)
{
    foreach (QMdiSubWindow* window, subWindowList())
        if (window->isVisible())
        {
            auto action = menu->addAction(window->windowTitle(), this, SLOT(activateChild()));
            action->setCheckable(true);
            action->setChecked(window == activeSubWindow());
            action->setData(QVariant::fromValue((QWidget*)window));
        }
}

void SchemaMdiArea::settingsChanged()
{
    updateBackground();
}

void SchemaMdiArea::updateBackground()
{
    if (Settings::instance().showBackground)
        setBackground(QBrush(QPixmap(qApp->applicationDirPath() + "/background.bmp")));
    else
        setBackground(QBrush(palette().color(QPalette::Background)));
}

