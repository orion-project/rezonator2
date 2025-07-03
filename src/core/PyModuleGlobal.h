#ifndef PY_MODULE_GLOBAL_H
#define PY_MODULE_GLOBAL_H

#include "PyUtils.h"

#include "CommonTypes.h"
#include "Format.h"
#include "Math.h"
#include "Units.h"

#include <functional>

#include <QDebug>

namespace PyModuleGlobal {

const char *name = "rezonator";

std::function<void(const QString&)> printFunc;

namespace Methods {

PyObject* print(PyObject* Py_UNUSED(self), PyObject* args, PyObject *kwargs)
{
    if (!printFunc) {
        PyErr_SetString(PyExc_NotImplementedError, "rezonator.print is not defined");
        return nullptr;
    }
    auto argCount = PyTuple_Size(args);
    if (argCount < 0)
        return nullptr;
    QStringList parts;
    for (int i = 0; i < argCount; i++) {
        auto arg = PyTuple_GetItem(args, i);
        if (!arg)
            return nullptr;
        if (PyUnicode_Check(arg))
            parts << QString::fromUtf8(PyUnicode_AsUTF8(arg));
        else if (PyLong_Check(arg))
            parts << QString::number(PyLong_AsInt(arg));
        else if (PyFloat_Check(arg))
            parts << QString::number(PyFloat_AsDouble(arg), 'g', 12);
        else if (Py_IsNone(arg))
            parts << QStringLiteral("None");
        else {
            auto msg = QString("unsupporter type of argument %1").arg(i).toUtf8();
            PyErr_SetString(PyExc_TypeError, msg.constData());
            return nullptr;
        }
    }
    bool spaced = true;
    if (kwargs) {
        if (auto pSpaced = PyDict_GetItemString(kwargs, "spaced"); pSpaced) {
            if (!PyBool_Check(pSpaced)) {
                PyErr_SetString(PyExc_TypeError, "wrong type of the \"spaced\" arg, bool expected");
                return nullptr;
            }
            spaced = Py_IsTrue(pSpaced);
        }
    }
    printFunc(spaced ? parts.join(' ') : parts.join(QString()));
    Py_RETURN_NONE;
}

PyObject* format(PyObject* Py_UNUSED(self), PyObject* arg)
{
    if (!PyFloat_Check(arg)) {
        PyErr_SetString(PyExc_TypeError, "unsupported argument type, float expected");
        return nullptr;
    }
    auto v = PyFloat_AsDouble(arg);
    auto s = Z::format(v).toUtf8();
    return PyUnicode_FromString(s.constData());
}

} // Methods

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

int on_exec(PyObject *module)
{
    CONST_FLOAT("C", Z::Const::LightSpeed)
    CONST_FLOAT("PI", Z::Const::Pi)

    auto dims = Z::Dims::dims();
    CONST_INT("DIM_NONE", dims.indexOf(Z::Dims::none()))
    CONST_INT("DIM_LINEAR", dims.indexOf(Z::Dims::linear()))
    CONST_INT("DIM_ANGULAR", dims.indexOf(Z::Dims::angular()))
    
    CONST_INT("PLANE_T", Z::WorkPlane::T)
    CONST_INT("PLANE_S", Z::WorkPlane::S)

    qDebug() << "rezonator module executed";
    return 0;
}

#undef CONST_FLOAT
#undef CONST_INT

PyMethodDef methods[] = {
    { "format", (PyCFunction)Methods::format, METH_O, "Format value into user display string" },
    { "print", (PyCFunction)Methods::print, METH_VARARGS | METH_KEYWORDS, "Print message" },
    { NULL, NULL, 0, NULL }
};

PyModuleDef_Slot slotes[] = {
    { Py_mod_exec, (void*)on_exec },
    { Py_mod_multiple_interpreters, Py_MOD_MULTIPLE_INTERPRETERS_NOT_SUPPORTED },
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

} // namespace PyModuleGlobal

#endif // PY_MODULE_GLOBAL_H
