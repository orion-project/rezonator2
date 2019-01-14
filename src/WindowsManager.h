#ifndef WINDOWS_MANAGER_H
#define WINDOWS_MANAGER_H

#include "core/OriTemplates.h"
#include "SchemaWindows.h"

#include <QMap>
#include <QList>
#include <QObject>

class Schema;
class SchemaWindow;

class WindowsManager : public QObject, public Singleton<WindowsManager>
{
    Q_OBJECT

public:
    /// Appends a window to specific schema's window list.
    /// Is called from SchemaWindow constructor.
    void registerWindow(Schema*, SchemaWindow*);

    /// Removes a window from specific schema's window list.
    /// Is called from SchemaWindow destructor.
    void unregisterWindow(Schema*, SchemaWindow*);

    /// Returns a list of windows owned by given schema.
    inline const QList<SchemaWindow*> schemaWindows(Schema *schema) const
    {
        return _windows[schema];
    }

    typedef SchemaWindow* (*Constructor)(Schema*);
    static void registerConstructor(const QString& type, Constructor ctor);
    static Constructor getConstructor(const QString& type);

    /// Shows given window as mdi-subwindow or as regular popup window depending on its type.
    /// All schema dealing windows should be shown using this method, but not directly.
    void show(SchemaWindow* wnd);

signals:
    void showMdiSubWindow(QWidget* wnd);

private:
    QMap<Schema*, QList<SchemaWindow*> > _windows;
};

#endif // WINDOWS_MANAGER_H
