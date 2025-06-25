#ifndef PY_MODULE_GLOBAL_H
#define PY_MODULE_GLOBAL_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "Math.h"

#include <functional>

#include <QDebug>

namespace PyModuleGlobal {

const char *name = "rezonator";

std::function<void(const QString&)> printFunc;

PyObject* light_speed(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(args))
{
    return PyFloat_FromDouble(Z::Const::LightSpeed);
}

PyObject* print(PyObject* Py_UNUSED(self), PyObject* args)
{
    if (!printFunc) {
        PyErr_SetString(PyExc_NotImplementedError, "rezonator.print is not defined");
        return nullptr;
    }
    const char *msg;
    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return nullptr;
    }
    printFunc(QString::fromUtf8(msg));
    Py_RETURN_NONE;
}

PyMethodDef methods[] = {
    { "light_speed", light_speed, METH_NOARGS, "Return the speed of light (in m/s)." },
    { "print", print, METH_VARARGS, "Print a message" },
    { NULL, NULL, 0, NULL }
};

PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = name,
    .m_size = 0,
    .m_methods = methods,
};

PyObject* init()
{
    return PyModuleDef_Init(&module);
}

} // namespace PyModuleGlobal

#endif // PY_MODULE_GLOBAL_H
