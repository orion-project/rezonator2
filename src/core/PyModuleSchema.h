#ifndef PY_MODULE_SCHEMA_H
#define PY_MODULE_SCHEMA_H

#include "PyUtils.h"
#include "Schema.h"
#include "../math/BeamCalcWrapper.h"

namespace PyModules::Schema {

const char *name = "schema";

::Schema *schema = nullptr;

PyObject *SchemaError = nullptr;

//------------------------------------------------------------------------------
//                                 Element
//------------------------------------------------------------------------------

namespace Element {

struct Element {
    PyObject_HEAD
    ::Element *elem;
};

PyObject* ctor(PyTypeObject *Py_UNUSED(type), PyObject *Py_UNUSED(args), PyObject *Py_UNUSED(kwargs))
{
    PyErr_SetString(SchemaError, "creation of elements from Python code is forbidden");
    return nullptr;
}

void dtor(Element *self)
{
    //qDebug() << "Dealloc" << self->elem->label();
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* label(Element *self, PyObject *Py_UNUSED(args))
{
    auto label = self->elem->label().toUtf8();
    return PyUnicode_FromString(label.constData());
}

PyObject* length(Element *self, PyObject *Py_UNUSED(args))
{
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->lengthSI());
    Py_RETURN_NONE;
}

PyObject* axis_length(Element *self, PyObject *Py_UNUSED(args))
{
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->axisLengthSI());
    Py_RETURN_NONE;
}

PyObject* optical_length(Element *self, PyObject *Py_UNUSED(args))
{
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->opticalPathSI());
    Py_RETURN_NONE;
}

PyObject* ior(Element *self, PyObject *Py_UNUSED(args))
{
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->ior());
    Py_RETURN_NONE;
}

PyGetSetDef getset[] = {
    GETTER(label, "Return element's label"),
    GETTER(length, "Return element's length parameter (in m) or none if element is not a range"),
    GETTER(axis_length, "Return elemen's axis length (in m) or none if element is not a range"),
    GETTER(optical_length, "Return element's optical length (in m) or none if element is not a range"),
    GETTER(ior, "Return element's refraction index or none if it's not supported in the element"),
    { NULL }
};

PyTypeObject type = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "schema.Element",
    .tp_basicsize = sizeof(Element),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)dtor,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Optical element"),
    .tp_getset = getset,
    .tp_new = ctor,
};

PyObject* make(::Element *elem)
{
    CHECK_TYPE_READY
    auto obj = type.tp_alloc(&type, 0);
    if (obj) {
        ((Element*)obj)->elem = elem;
    }
    return obj;
}

} // namespace Element 

//------------------------------------------------------------------------------
//                               Calculator
//------------------------------------------------------------------------------

namespace Calculator {

struct Calculator {
    PyObject_HEAD
    BeamCalcWrapper *calc;
    Z::WorkPlane plane;
    double ior;
};

PyObject* ctor(PyTypeObject* Py_UNUSED(type), PyObject* Py_UNUSED(args), PyObject* Py_UNUSED(kwargs))
{
    PyErr_SetString(SchemaError, "direct creation of Calculator it not allowed, use schema.make_calc()");
    return nullptr;
}

PyObject* beam_radius(Calculator* self, PyObject* Py_UNUSED(arg))
{
    return PyFloat_FromDouble(self->calc->beamRadius());
}

PyObject* front_radius(Calculator* self, PyObject* Py_UNUSED(arg))
{
    return PyFloat_FromDouble(self->calc->frontRadius());
}

PyObject* half_angle(Calculator* self, PyObject* Py_UNUSED(arg))
{
    return PyFloat_FromDouble(self->calc->halfAngle());
}

PyMethodDef methods[] = {
    { "beam_radius", (PyCFunction)beam_radius, METH_NOARGS, "Calculate beam radius (in m)" },
    { "front_radius", (PyCFunction)front_radius, METH_NOARGS, "Calculate wavefront radius (in m)" },
    { "half_angle", (PyCFunction)half_angle, METH_NOARGS, "Calculate half of divergence angle in the far-field (in rad)" },
    { NULL }
};

PyMemberDef members[] = {
    { "plane", Py_T_INT, offsetof(Calculator, plane), 0, "Work plane (one of Z.PLANE_T or Z.PLANE_S)" },
    { "ior", Py_T_DOUBLE, offsetof(Calculator, ior), 0, "Current index of refraction" },
    { NULL }
};

PyTypeObject type = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "schema.Calculator",
    .tp_basicsize = sizeof(Calculator),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Beam calculator"),
    .tp_methods = methods,
    .tp_members = members,
    .tp_new = ctor,
};

PyObject* make(BeamCalcWrapper* calc)
{
    CHECK_TYPE_READY
    auto obj = type.tp_alloc(&type, 0);
    if (obj) {
        ((Calculator*)obj)->calc = calc;
        ((Calculator*)obj)->plane = calc->plane();
        ((Calculator*)obj)->ior = calc->ior();
    }
    return obj;
}

} // namespace BeamCalc

//------------------------------------------------------------------------------
//                                 Schema
//------------------------------------------------------------------------------

namespace Methods {

#define CHECK_SCHEMA \
    if (!schema) { \
        PyErr_SetString(SchemaError, "schema reference is not provided"); \
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
    if (!PyArg_Parse(args, "s", &alias))
        return nullptr;
    auto p = schema->param(alias);
    if (!p) {
        PyErr_SetQString(SchemaError, QString("parameter not found: %1").arg(alias));
        return nullptr;
    }
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
        elem = schema->element(index-1);
        if (!elem) {
            PyErr_SetString(PyExc_IndexError, "element not found");
            return nullptr;
        }
    } else if (PyUnicode_Check(arg)) {
        auto label = QString::fromUtf8(PyUnicode_AsUTF8(arg));
        elem = schema->element(label);
        if (!elem) {
            PyErr_SetString(PyExc_KeyError, "element not found");
            return nullptr;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "unsupported type of argument");
        return nullptr;
    }
    return Element::make(elem);
}

PyObject* elem_count(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    return PyLong_FromSize_t(schema->elements().size());
}

PyObject* is_sp(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    return PyBool_FromLong(schema->isSP());
}

PyObject* is_sw(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    return PyBool_FromLong(schema->isSW());
}

PyObject* is_rr(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    return PyBool_FromLong(schema->isRR());
}

} // Methods

#define ADD_TYPE(name) \
    if (PyType_Ready(&name::type) < 0) STOP_MODULE_INIT \
    if (PyModule_AddObjectRef(module, #name, (PyObject*)&name::type) < 0) STOP_MODULE_INIT

int on_exec(PyObject *module)
{
    SchemaError = PyErr_NewException("schema.error", NULL, NULL);
    if (PyModule_AddObjectRef(module, "SchemaError", SchemaError) < 0)
        STOP_MODULE_INIT
    
    ADD_TYPE(Element)
    ADD_TYPE(Calculator)

    qDebug() << "schema module executed";
    return 0;
}

#undef ADD_TYPE

PyMethodDef methods[] = {
    { "elem", Methods::elem, METH_O, "Return element by label or number" },
    { "elem_count", Methods::elem_count, METH_NOARGS, "Return number of elements in schema" },
    { "is_sp", Methods::is_sp, METH_NOARGS, "If schema is single pass system" },
    { "is_sw", Methods::is_sw, METH_NOARGS, "If schema is standing wave rezonator" },
    { "is_rr", Methods::is_rr, METH_NOARGS, "If schema is ring rezonator" },
    { "param", Methods::param, METH_O, "Return value of global parameter (in SI units)." },
    { "wavelength", Methods::wavelength, METH_NOARGS, "Return current wavelength (in m)." },
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

} // namespace PyModules::Schema


#endif // PY_MODULE_SCHEMA_H
