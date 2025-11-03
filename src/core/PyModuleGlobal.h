#ifndef PY_MODULE_GLOBAL_H
#define PY_MODULE_GLOBAL_H

#include "PyUtils.h"
#include "PyClassElement.h"
#include "PyClassMatrix3.h"
#include "PyClassRoundTrip.h"

#include "CommonTypes.h"
#include "Format.h"
#include "Math.h"
#include "Units.h"

namespace PyModule::Global {

const char *moduleName = "rezonator";

PyObject* print(PyObject* Py_UNUSED(self), PyObject* args, PyObject *kwargs)
{
    if (!PyGlobal::printFunc) {
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
            // __repr__ gives a string surrounded with quotes here,
            // which is undesirable so do manually
            parts << QString::fromUtf8(PyUnicode_AsUTF8(arg));
        else if (PyFloat_Check(arg))
            parts << Z::format(PyFloat_AsDouble(arg));
        else
        {
            // Try to get string representation using __repr__
            auto repr = PyObject_Repr(arg);
            if (repr) {
                parts << QString::fromUtf8(PyUnicode_AsUTF8(repr));
                Py_DECREF(repr);
            } else {
                PyErr_SetQString(PyExc_TypeError, QString("unsupported type of argument %1").arg(i));
                return nullptr;
            }
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
    PyGlobal::printFunc(spaced ? parts.join(' ') : parts.join(QString()));
    Py_RETURN_NONE;
}

PyObject* format(PyObject* Py_UNUSED(self), PyObject* arg)
{
    double v;
    if (PyFloat_Check(arg))
        v = PyFloat_AsDouble(arg);
    else if (PyLong_Check(arg))
        v = PyLong_AsLong(arg);
    else {
        PyErr_SetString(PyExc_TypeError, "unsupported argument type, number expected");
        return nullptr;
    }
    auto s = Z::format(v).toUtf8();
    return PyUnicode_FromString(s.constData());
}

PyObject* plane_str(PyObject* Py_UNUSED(self), PyObject* arg)
{
    int plane;
    if (!PyArg_ParseTuple(arg, "i", &plane))
        return nullptr;
    if (plane == Z::T)
        return PyUnicode_FromString("T");
    if (plane == Z::S)
        return PyUnicode_FromString("S");
    PyErr_SetQString(PyExc_KeyError, QString("unknown plane constant %1").arg(plane));
    return nullptr;
}

PyObject* version(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(args))
{
    auto ver = Z::Strs::appVersion().toUtf8();
    return PyUnicode_FromString(ver.constData());
}

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

    ADD_TYPE(Element)
    ADD_TYPE(Matrix)
    ADD_TYPE(Matrix3)
    ADD_TYPE(RayVector)
    ADD_TYPE(RoundTrip)

    qDebug() << "Module executed:" << moduleName;
    return 0;
}

PyMethodDef methods[] = {
    { "format", format, METH_O, "Format value into user display string" },
    { "plane_str", plane_str, METH_VARARGS, "Return work plane name" },
    { "print", (PyCFunction)print, METH_VARARGS | METH_KEYWORDS, "Print message" },
    { "version", version, METH_NOARGS, "Return application version" },
    { NULL, NULL, 0, NULL }
};

PyModuleDef_Slot slotes[] = {
    { Py_mod_exec, (void*)on_exec },
    { Py_mod_multiple_interpreters, Py_MOD_MULTIPLE_INTERPRETERS_NOT_SUPPORTED },
    { 0, NULL }
};

PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = moduleName,
    .m_size = 0,
    .m_methods = methods,
    .m_slots = slotes,
};

PyObject* init()
{
    return PyModuleDef_Init(&module);
}

} // namespace PyModule::Global

#endif // PY_MODULE_GLOBAL_H
