#ifndef Z_SCHEMA_STORABLE_H
#define Z_SCHEMA_STORABLE_H

#include <QString>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

/**
    Interface for a window which wants to be saved into schema file.

    To add storage behaviour to some window, follow these steps:

    1) Only subclasses of @a SchemaWindow can be stored in schema file.
       It is because of these classes can register itself in @a WindowsManager,
       and only objects contained in @a WindowsManager are used to store.

    2) Inherit ISchemaWindowStorable interface and implement its methods.

    3) Implement factory function that can produce windows of desired type.
       For example @see windowConstructor() function in CalcManager.cpp

    4) Register this function via @a WindowsManager::registerConstructor() method somewhere.
       For example @see CalcManager::CalcManager(), it registers all function windows.
       Or @see SchemaParamsWindowStorable namespace and how it is used in @a ProjectWindow ctor.
*/
class ISchemaWindowStorable
{
public:
    /// Returns unique object type key.
    ///
    /// This key is used to find an window constructor in @a WindowsManager
    /// when window is going to be restored from schema file.
    ///
    /// Probably the most easy way is to return window class name,
    /// then the compiler will be in charge of its uniqueness.
    ///
    virtual QString storableType() const = 0;

    /// Derived function should implement all storage logic.
    /// By default, only fact of window existence will be saved.
    /// Function should return error text if window can't be stored, or empty string when all is OK.
    ///
    virtual QString storableWrite(QJsonObject& root) { Q_UNUSED(root); return QString(); }

    /// Derived function should implement all restoring logic.
    /// By default, default constructed window will be restored.
    /// Function should return error text if window can't be restored, or empty string when all is OK.
    ///
    virtual QString storableRead(const QJsonObject& root) { Q_UNUSED(root); return QString(); }
};

#endif // Z_SCHEMA_STORABLE_H
