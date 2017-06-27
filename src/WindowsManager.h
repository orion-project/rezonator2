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
    void registerWindow(Schema*, SchemaWindow*);
    void unregisterWindow(Schema*, SchemaWindow*);

    inline const QList<SchemaWindow*> schemaWindows(Schema *schema) const
    {
        return _windows[schema];
    }

    typedef SchemaWindow* (*Constructor)(Schema*);
    static void registerConstructor(const QString& type, Constructor ctor);
    static Constructor getConstructor(const QString& type);

    void show(SchemaWindow* wnd);

signals:
    void showMdiSubWindow(QWidget* wnd);

private:
    QMap<Schema*, QList<SchemaWindow*> > _windows;
};

#endif // WINDOWS_MANAGER_H
