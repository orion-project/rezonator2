#ifndef PY_MODULE_SCHEMA_H
#define PY_MODULE_SCHEMA_H

#include "PyUtils.h"
#include "PyClassElement.h"
#include "PyClassRoundTrip.h"
#include "Schema.h"
#include "../math/BeamCalculator.h"
#include "../math/FunctionUtils.h"

namespace PyModule::Schema {

const char *moduleName = "schema";

PyObject* wavelength(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(args))
{
    CHECK_SCHEMA
    return PyFloat_FromDouble(SCHEMA->wavelength().value().toSi());
}

PyObject* param(PyObject* Py_UNUSED(self), PyObject* args)
{
    CHECK_SCHEMA
    const char *alias;
    if (!PyArg_Parse(args, "s", &alias))
        return nullptr;
    auto p = SCHEMA->param(alias);
    CHECK_(p, KeyError, "parameter not found")
    return PyFloat_FromDouble(p->value().toSi());
}

PyObject* elem(PyObject* Py_UNUSED(self), PyObject* arg)
{
    CHECK_SCHEMA
    ::Element *elem = nullptr;
    if (PyLong_Check(arg)) {
        auto index = PyLong_AsLong(arg);
        // For python code elemens are numbered 1-based
        // as they are shown in the elements table
        elem = SCHEMA->element(index-1);
        CHECK_(elem, IndexError, "element not found")
    } else if (PyUnicode_Check(arg)) {
        auto label = QString::fromUtf8(PyUnicode_AsUTF8(arg));
        elem = SCHEMA->element(label);
        CHECK_(elem, KeyError, "element not found")
    } else
        CHECK_(false, TypeError, "unsupported type of argument, integer or string expected")
    return PyClass::Element::make(elem);
}

PyObject* elem_count(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    return PyLong_FromSize_t(SCHEMA->elements().size());
}

PyObject* is_sp(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    return PyBool_FromLong(SCHEMA->isSP());
}

PyObject* is_sw(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    return PyBool_FromLong(SCHEMA->isSW());
}

PyObject* is_rr(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    return PyBool_FromLong(SCHEMA->isRR());
}

PyObject* round_trip(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args), PyObject *kwargs)
{
    CHECK_SCHEMA
    if (SCHEMA->activeCount() == 0) {
        PyErr_SetString(PyExc_KeyError, "there are no active elements in the schema");
        return nullptr;
    }
    auto splitRange = false;
    auto workPlane = Z::WorkPlane::T;
    ::Element *refElem = nullptr;
    if (kwargs) {
        if (auto arg = PyDict_GetItemString(kwargs, "ref"); arg) {
            if (PyLong_Check(arg)) {
                auto index = PyLong_AsLong(arg);
                // For python code elemens are numbered 1-based
                // as they are shown in the elements table
                refElem = SCHEMA->element(index-1);
                CHECK_(refElem, IndexError, "reference element not found")
            } else if (PyUnicode_Check(arg)) {
                auto label = QString::fromUtf8(PyUnicode_AsUTF8(arg));
                refElem = SCHEMA->element(label);
                CHECK_(refElem, KeyError, "reference element not found")
            } else if (PyObject_TypeCheck(arg, PyClass::Element::type())) {
                refElem = ((PyClass::Element::Self*)arg)->elem;
                CHECK_(refElem, ValueError, "element reference is null")
                CHECK_(SCHEMA->elements().contains(refElem), ValueError, "reference element not found")
            } else {
                CHECK_(false, TypeError, "wrong type of the 'ref' arg, integer or string or Element expected");
            }
        }
        if (auto arg = PyDict_GetItemString(kwargs, "plane"); arg) {
            if (PyUnicode_Check(arg)) {
                auto plane = QString::fromUtf8(PyUnicode_AsUTF8(arg)).toUpper();
                if (plane == Z::planeName(Z::T))
                    workPlane = Z::T;
                else if (plane == Z::planeName(Z::S))
                    workPlane = Z::S;
                else
                    CHECK_(false, ValueError, "wrong work plane name, T or S expected")
            } else if (PyLong_Check(arg)) {
                auto plane = PyLong_AsLong(arg);
                CHECK_(plane == Z::WorkPlane::T || plane == Z::WorkPlane::S, ValueError, 
                    "unexpected value of the 'plane' arg, expected one of Z.PLANE_T or Z.PLANE_S")
                workPlane = (Z::WorkPlane)plane;
            } else {
                CHECK_(false, TypeError, "wrong type of the 'plane' arg, string or integer expected")
            }
        }
        if (auto arg = PyDict_GetItemString(kwargs, "inside"); arg) {
            CHECK_(PyBool_Check(arg), TypeError, "wrong type of the 'inside' arg, bool expected")
            splitRange = Py_IsTrue(arg);
        }
    }
    if (!refElem)
        refElem = SCHEMA->elements().last();
    auto beamCalc = new BeamCalculator(SCHEMA);
    if (!beamCalc->ok()) {
        PyErr_SetQString(PyExc_AssertionError, beamCalc->error());
        delete beamCalc;
        return nullptr;
    }
    beamCalc->calcRoundTrip(refElem, splitRange, "py.schema.round_trip()");
    beamCalc->setPlane(workPlane);
    beamCalc->setIor(FunctionUtils::ior(SCHEMA, refElem, splitRange));
    return PyClass::RoundTrip::make(beamCalc, true);
}

int on_exec(PyObject *module)
{
    PyGlobal::SchemaError = PyErr_NewException("schema.error", NULL, NULL);
    if (PyModule_AddObjectRef(module, "SchemaError", PyGlobal::SchemaError) < 0)
        STOP_MODULE_INIT
    
    qDebug() << "Module executed:" << moduleName;
    return 0;
}

PyMethodDef methods[] = {
    { "elem", elem, METH_O, "Return element by label or number" },
    { "elem_count", elem_count, METH_NOARGS, "Return number of elements in schema" },
    { "is_sp", is_sp, METH_NOARGS, "If schema is single pass system" },
    { "is_sw", is_sw, METH_NOARGS, "If schema is standing wave rezonator" },
    { "is_rr", is_rr, METH_NOARGS, "If schema is ring rezonator" },
    { "param", param, METH_O, "Return value of global parameter (in SI units)." },
    { "wavelength", wavelength, METH_NOARGS, "Return current wavelength (in m)." },
    { "round_trip", (PyCFunction)round_trip, METH_VARARGS | METH_KEYWORDS,
        "Return a RoundTrip object that can be used for basic calculations." },
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

} // namespace PyModule::Schema

#endif // PY_MODULE_SCHEMA_H
