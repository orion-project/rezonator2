#ifndef PY_UTILS_H
#define PY_UTILS_H

#include "Python.h"

#include <QDebug>
#include <QString>

#include <functional>

class Schema;

/// Global data a python script operates on.
/// Must be initialized on script loading, @a PyRunner::load().
namespace PyGlobal {

/// Current schema.
/// Currently, one app instance can operate on a single schema.
::Schema *schema = nullptr;

/// Exception class used for rezonator specific errors.
PyObject *SchemaError = nullptr;

/// A function used by Z.print()
/// It does qDebug() for scripts that do not show a code window
/// Where there is a code window, the output target is its log panel
std::function<void(const QString&)> printFunc;

} // namespace PyGlobal

#define SCHEMA PyGlobal::schema

#define ADD_MODULE(module) \
    if (PyImport_AppendInittab(module::moduleName, &module::init) == -1) \
        qWarning() << "Unable to register py module" << module::moduleName;

#define INIT_MODULE(module) \
    if (!PyImport_ImportModule(module::moduleName)) \
        qCritical() << "Unable to initialize module" << module::moduleName;

#define STOP_MODULE_INIT { \
    qCritical() << Q_FUNC_INFO << "Failure"; \
    return -1; \
}

#define ADD_TYPE(name) \
    PyClass::name::moduleName = moduleName; \
    if (PyType_Ready(PyClass::name::type()) < 0) STOP_MODULE_INIT \
    if (PyModule_AddObjectRef(module, #name, (PyObject*)PyClass::name::type()) < 0) STOP_MODULE_INIT \
    qDebug() << "Type registered:" << PyClass::name::type()->tp_name;

#define CONST_FLOAT(name, value) { \
    auto p = PyFloat_FromDouble(value); \
    if (!p) \
        STOP_MODULE_INIT \
    if (PyModule_AddObjectRef(module, name, p) < 0) { \
        Py_DECREF(p); \
        STOP_MODULE_INIT \
    } \
    Py_DECREF(p); \
}

#define CONST_INT(name, value) \
    if (PyModule_AddIntConstant(module, name, value) < 0) STOP_MODULE_INIT;

#define GETTER(name, docstr) { #name, (getter)name, nullptr, docstr }
#define GETSET(name, docstr) { #name, (getter)name, (setter)set_##name, docstr }

#define MAKE_OBJECT \
    if (!type()->tp_alloc) { \
        PyErr_SetQString(PyExc_TypeError, QString("type %1 is not initialized, add 'import %2'").arg(type()->tp_name, moduleName)); \
        return nullptr; \
    } \
    auto obj = (Self*)type()->tp_alloc(type(), 0);

#define CHECK_SCHEMA \
    if (!PyGlobal::schema) { \
        PyErr_SetString(PyGlobal::SchemaError, "schema reference is not provided"); \
        return nullptr; \
    }

// For function requiring to return a py object
#define CHECK_(cond, type, msg) \
    if(!(cond)) { \
        PyErr_SetString(PyExc_##type, msg); \
        return nullptr; \
    }

// For function requiring to return an interger error flag, e.g. attribute setters
#define CHECK_I(cond, type, msg) \
    if(!(cond)) { \
        PyErr_SetString(PyExc_##type, msg); \
        return -1; \
    }

void PyErr_SetQString(PyObject *exc, const QString &err)
{
    auto msg = err.toUtf8();
    PyErr_SetString(exc, msg.constData());
}

#endif // PY_UTILS_H
