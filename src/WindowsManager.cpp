#include "WindowsManager.h"

#include "core/Schema.h"

static QMap<QString, WindowsManager::Constructor> __schemaWindowCtors;

void WindowsManager::registerConstructor(const QString& type, WindowsManager::Constructor ctor)
{
    if  (!__schemaWindowCtors.contains(type))
        __schemaWindowCtors.insert(type, ctor);
}

WindowsManager::Constructor WindowsManager::getConstructor(const QString& type)
{
    return __schemaWindowCtors.contains(type) ? __schemaWindowCtors[type] : nullptr;
}

void WindowsManager::registerWindow(Schema *schema, SchemaWindow *window)
{
    if (!_windows[schema].contains(window))
        _windows[schema].append(window);
}

void WindowsManager::unregisterWindow(Schema *schema, SchemaWindow *window)
{
    if (_windows.contains(schema))
        _windows[schema].removeOne(window);
}

void WindowsManager::show(SchemaWindow* wnd)
{
    auto mdi = dynamic_cast<QMdiSubWindow*>(wnd);
    if (mdi)
        emit showMdiSubWindow(mdi);
    else
    {
        auto widget = dynamic_cast<QWidget*>(wnd);
        if (widget)
            widget->show();
        else
            qCritical() << "Unable to show window of non-widget type";
    }
}
