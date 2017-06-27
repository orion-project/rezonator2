#ifndef SCHEMA_WINDOWS_H
#define SCHEMA_WINDOWS_H

#include "AppSettings.h"
#include "core/Schema.h"

QT_BEGIN_NAMESPACE
class QMenu;
class QToolBar;
class QVBoxLayout;
QT_END_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
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
    virtual bool canCut() { return false; }
    virtual bool canCopy() { return false; }
    virtual bool canPaste() { return false; }
    virtual void cut() {}
    virtual void copy() {}
    virtual void paste() {}
    virtual void selectAll() {}
};


////////////////////////////////////////////////////////////////////////////////
/**
    Base class for any windows dealing with schema.
    Each SchemaWindow is registered in WindowsManager when constructed.
*/
class SchemaWindow : public SchemaListener
{
public:

//    enum WindowRole
//    {
//        DefaultRole,
//        ProjectWindowRole,
//        SchemaViewRole
//    };

    SchemaWindow(Schema *owner);
    ~SchemaWindow();

    /// Returns schema shown by this window.
    inline Schema* schema() { return _schema; }

//    virtual WindowRole role() const { return DefaultRole; }

private:
    Schema* _schema;
};

////////////////////////////////////////////////////////////////////////////////

class SchemaToolWindow : public SchemaWindow, public SettingsListener
{
public:
    SchemaToolWindow(Schema *owner);

    void settingsChanged() override;

    QToolBar* makeToolBar(bool flat = false);
    QToolBar* makeToolBar(const std::initializer_list<QObject *>& items, bool flat = false);
    QToolBar* makeToolBar(const QString& title, bool flat = false);
    QToolBar* makeToolBar(const QString& title, const std::initializer_list<QObject*>& items, bool flat = false);

    QToolBar* toolbar() const { return _toolbars.isEmpty()? nullptr: _toolbars.first(); }
    const QList<QToolBar*>& toolbars() const { return _toolbars; }

private:
    QList<QToolBar*> _toolbars;
};

////////////////////////////////////////////////////////////////////////////////
#include <QMdiSubWindow>

/**
    Basic mdi-child window.
    Base class for windows presenting schema related data in MDI area of project window.
    For example SchemaViewWindow showing schema elements list or any window presenting
    calculation results (plot windows, table windows).
    \note We inherits BasicMdiChild from QMdiSubWindow not from QWidget to allow
    for each type of child mdi-window to have its own icon. When simple QWidget is
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
        /// (\see BasicMdiChild::setContent).
        initNoDefaultWidget = 0x01,

        /// Do not create default toolbar. Default toolbar is inserted into
        /// layout of default widget, so the option has only meaning if
        /// initNoDefaultWidget option is not set.
        initNoToolBar = 0x02,
    };
    typedef QFlags<InitOption> InitOptions;

public:
    BasicMdiChild(InitOptions options = 0);

    /// Additional menu for main window menu bar.
    /// These additional menus is shown when mdi-child window is activated.
    /// Index is varied from 0 to menuCount()-1. Additional menus are tipicaly
    /// inserted at end of main window menu bar, before Help and Window menus.
    virtual QList<QMenu*> menus() { return QList<QMenu*>(); }

    /// Default toolbar of MDI-child window.
    QToolBar* toolbar() const { return _toolbar; }

    void populateToolbar(std::initializer_list<QObject*> items);

    void setContent(QWidget *content, int row = -1);

    virtual void init() {}

    void settingsChanged() override;

protected:
    void closeEvent(class QCloseEvent*) { emit closing(); }

    QSize sizeHint() const { return QSize(640, 480); }

private:
    QVBoxLayout* _layout = nullptr;
    QToolBar* _toolbar = nullptr;
    QWidget* _content = nullptr;

signals:
    void closing();
};

////////////////////////////////////////////////////////////////////////////////

class SchemaMdiChild : public BasicMdiChild, public SchemaWindow
{
public:
    SchemaMdiChild(Schema* schema, InitOptions options = 0);
};

////////////////////////////////////////////////////////////////////////////////
#include <QMdiArea>

class SchemaMdiArea : public QMdiArea, public SettingsListener
{
    Q_OBJECT

public:
    explicit SchemaMdiArea(QWidget *parent = 0);

    void populateMenu(QMenu*);

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

private:
    void updateBackground();
};

#endif // SCHEMA_WINDOWS_H
