#ifndef SCHEMA_WINDOWS_H
#define SCHEMA_WINDOWS_H

#include "AppSettings.h"
#include "core/Schema.h"

#include <QMdiArea>
#include <QMdiSubWindow>

QT_BEGIN_NAMESPACE
class QMenu;
class QToolBar;
class QVBoxLayout;
QT_END_NAMESPACE

//------------------------------------------------------------------------------

namespace Z {
namespace WindowUtils {

QSize toolbarIconSize();
void adjustIconSize(QToolBar* toolbar);
void adjustIconSize(QToolBar* toolbar, const QSize& iconSize);

} // namespace WindowUtils
} // namespace Z

//------------------------------------------------------------------------------
/**
    Interface of a window which can process editing commands.
    Project window holds editing commands. When an editing command is invoked,
    project window asks its active child window if it is editable window.
    If it is, project window calls corresponding method of that child
    to process the command.
*/
class EditableWindow
{
public:
    enum SupportedCommand {
        EditCmd_None = 0x00,
        EditCmd_Cut = 0x01,
        EditCmd_Copy = 0x02,
        EditCmd_Paste = 0x04,
        EditCmd_SelectAll = 0x08
    };
    Q_DECLARE_FLAGS(SupportedCommands, SupportedCommand)

    virtual ~EditableWindow();

    virtual SupportedCommands supportedCommands() { return EditCmd_None; }
    virtual bool canCut() { return false; }
    virtual bool canCopy() { return false; }
    virtual bool canPaste() { return false; }
    virtual void cut() {}
    virtual void copy() {}
    virtual void paste() {}
    virtual void selectAll() {}
};

Q_DECLARE_OPERATORS_FOR_FLAGS(EditableWindow::SupportedCommands)

//------------------------------------------------------------------------------
/**
    Base class for any windows dealing with schema.
    Each SchemaWindow is registered in WindowsManager when constructed.
*/
class SchemaWindow : public SchemaListener
{
public:
    SchemaWindow(Schema *owner);
    ~SchemaWindow();

    /// Returns schema shown by this window.
    inline Schema* schema() { return _schema; }

private:
    Schema* _schema;
};

//------------------------------------------------------------------------------
/**
    Base class for popup windows presenting schema related data.
    For example, these are information function windows.
    This base class does not make derived class to be a widget,
    so derived should also be inherited from some QWidget class,
    see @ref SchemaMdiChild or @ref InfoFuncWindow for example.
*/
class SchemaToolWindow : public SchemaWindow, public SettingsListener
{
public:
    enum InitOption {
        NoOption = 0x00,
    };
    Q_DECLARE_FLAGS(InitOptions, InitOption)

    SchemaToolWindow(Schema *owner, InitOptions options = InitOptions());

    /// Inherited from @ref SettingsListener.
    /// Function is called when application settings are changed.
    void settingsChanged() override;

protected:
    QToolBar* makeToolBar(bool flat = false);
    QToolBar* makeToolBar(const std::initializer_list<QObject *>& items, bool flat = false);
    QToolBar* makeToolBar(const QString& title, bool flat = false);
    QToolBar* makeToolBar(const QString& title, const std::initializer_list<QObject*>& items, bool flat = false);

    QToolBar* toolbar() const { return _toolbars.isEmpty() ? nullptr : _toolbars.first(); }

private:
    QList<QToolBar*> _toolbars;
};

//------------------------------------------------------------------------------
/**
    Base class for windows which are displayed in MDI area of project window.

    @note We inherit BasicMdiChild from QMdiSubWindow not from QWidget to allow
    each type of child mdi-window to have its own icon. When simple QWidget is
    inserted into QMdiArea then default QMdiSubWindow is created and it uses default
    application icon.
*/
class BasicMdiChild : public QMdiSubWindow, public SettingsListener
{
    Q_OBJECT

public:
    /// Initialization options of MDI-child window.
    enum InitOption
    {
        /// Do not create default widget to set as central area.
        /// Ancestors can create its own widget instead of default
        /// (See @ref BasicMdiChild::setContent).
        initNoDefaultWidget = 0x01,

        /// Do not create default toolbar. Default toolbar is inserted into
        /// layout of default widget, so the option has only meaning if
        /// initNoDefaultWidget option is not set.
        initNoToolBar = 0x02,
    };
    Q_DECLARE_FLAGS(InitOptions, InitOption)

public:
    BasicMdiChild(InitOptions options = InitOptions());

    /// Returns additional menu for main window menu bar.
    /// These additional menus is shown when MDI-child window is activated.
    /// Additional menus are tipicaly inserted at end of main window menu bar,
    /// before Help and Window menus.
    virtual QList<QMenu*> menus() { return QList<QMenu*>(); }

    /// Returns additional actions for View menu.
    /// These actions are inserted into View menu when MDI-child window is activated.
    virtual QList<QAction*> viewActions() { return QList<QAction*>(); }

    /// Returns default toolbar of MDI-child window
    /// or nullptr if one was not created (see @ref InitOption::initNoToolBar).
    QToolBar* toolbar() const { return _toolbar; }

    /// Appends items into default toolbar, if one was created (see @ref InitOption::initNoToolBar).
    void populateToolbar(std::initializer_list<QObject*> items);

    void setTitleAndIcon(const QString& title, const char* iconPath);
    void setContent(QWidget *content, int row = -1);

    /// @ref SchemaMdiArea calls this function after subwindow was insterted into MDI-area.
    virtual void init() {}

    /// Inherited from @ref SettingsListener.
    /// Function is called when application settings are changed.
    void settingsChanged() override;

protected:
    void closeEvent(class QCloseEvent*) override { emit closing(); }

    QSize sizeHint() const override { return QSize(640, 480); }

private:
    QVBoxLayout* _layout = nullptr;
    QToolBar* _toolbar = nullptr;
    QWidget* _content = nullptr;

signals:
    void closing();
};

//------------------------------------------------------------------------------
/**
    Base class for windows presenting schema related data in MDI area of project window.
    For example SchemaViewWindow showing schema elements list or any window presenting
    calculation results (plot windows, table windows).
*/
class SchemaMdiChild : public BasicMdiChild, public SchemaWindow
{
public:
    SchemaMdiChild(Schema* schema, InitOptions options = InitOptions());
};

//------------------------------------------------------------------------------

class SchemaMdiArea : public QMdiArea, public SettingsListener
{
    Q_OBJECT

public:
    explicit SchemaMdiArea(QWidget *parent = nullptr);

    BasicMdiChild* activeChild() const;
    EditableWindow* activeEditableChild() const;

    void settingsChanged() override;

public slots:
    void appendChild(QWidget*);
    void appendChild(BasicMdiChild*);
    void activateChild();
    void activateChild(QWidget*);
    void activateChild(BasicMdiChild*);
    void editableChild_Cut();
    void editableChild_Copy();
    void editableChild_Paste();
    void editableChild_SelectAll();

    /// Slot that should be connected to signal aboutToShow() of menu 'Window'
    /// to populate it with actions allowing subwindow activation.
    void populateWindowMenu();

private:
    void updateBackground();
};

#endif // SCHEMA_WINDOWS_H
