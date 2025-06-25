#ifndef PY_MODULE_SCHEMA_H
#define PY_MODULE_SCHEMA_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "Schema.h"

namespace PyModuleSchema {

const char *name = "schema";

Schema *schema = nullptr;

PyObject *schemaError = nullptr;

#define CHECK_SCHEMA \
    if (!schema) { \
        PyErr_SetString(schemaError, "schema reference is not provided"); \
        return nullptr; \
    }

PyObject* wavelength(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(args))
{
    CHECK_SCHEMA
    return PyFloat_FromDouble(schema->wavelength().value().toSi());
}

PyObject* param(PyObject* Py_UNUSED(self), PyObject* args)
{
    CHECK_SCHEMA
    const char *alias;
    if (!PyArg_ParseTuple(args, "s", &alias))
        return nullptr;
    auto p = schema->param(alias);
    if (!p) {
        auto err = QStringLiteral("parameter not found: %1").arg(alias).toUtf8();
        PyErr_SetString(schemaError, err.constData());
        return nullptr;
    }
    return PyFloat_FromDouble(p->value().toSi());
}

int on_exec(PyObject *module)
{
    if (!schemaError) {
        schemaError = PyErr_NewException("schema.error", NULL, NULL);
        if (PyModule_AddObjectRef(module, "SchemaError", schemaError) < 0) {
            qCritical() << "Unable to register an error type for the schema module";
            return -1;
        }
    }
    qDebug() << "Schema module executed";
    return 0;
}

PyMethodDef methods[] = {
    { "wavelength", wavelength, METH_NOARGS, "Return current wavelength (in m)." },
    { "param", param, METH_VARARGS, "Return value of global parameter (in SI units)." },
    { NULL, NULL, 0, NULL }
};

PyModuleDef_Slot slotes[] = {
    { Py_mod_exec, (void*)on_exec },
    { 0, NULL }
};

PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = name,
    .m_size = 0,
    .m_methods = methods,
    .m_slots = slotes,
};

PyObject* init()
{
    return PyModuleDef_Init(&module);
}

} // namespace PyModuleSchema


#endif // PY_MODULE_SCHEMA_H
