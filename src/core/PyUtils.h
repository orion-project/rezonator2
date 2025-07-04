#ifndef PY_UTILS_H
#define PY_UTILS_H

#include "Python.h"

#include <QString>

#define STOP_MODULE_INIT { \
    qCritical() << Q_FUNC_INFO << "Failure"; \
    return -1; \
}

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

#define CHECK_TYPE_READY \
    if (!type.tp_alloc) { \
        PyErr_SetQString(PyExc_TypeError, QString("type %1 is not initialized, add 'import %2'").arg(type.tp_name, name)); \
        return nullptr; \
    }

void PyErr_SetQString(PyObject *exc, const QString &err)
{
    auto msg = err.toUtf8();
    PyErr_SetString(exc, msg.constData());
}

#define ADD_MODULE(module) \
    if (PyImport_AppendInittab(module::name, &module::init) == -1) \
        qWarning() << "Unable to register py module" << module::name;

#define INIT_MODULE(module) \
    if (!PyImport_ImportModule(module::name)) \
        qCritical() << "Unable to initialize module" << module::name;

#endif // PY_UTILS_H
