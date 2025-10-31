#ifndef PY_MODULE_SCHEMA_H
#define PY_MODULE_SCHEMA_H

#include "PyUtils.h"
#include "Schema.h"
#include "../math/BeamCalculator.h"
#include "../math/FunctionUtils.h"

namespace PyModules::Schema {

const char *name = "schema";

::Schema *schema = nullptr;

PyObject *SchemaError = nullptr;

#define CHECK_SCHEMA \
    if (!schema) { \
        PyErr_SetString(SchemaError, "schema reference is not provided"); \
        return nullptr; \
    }

#define CHECK_(cond, type, msg) \
    if(!(cond)) { \
        PyErr_SetString(PyExc_##type, msg); \
        return nullptr; \
    }

// For function requiring an interger error flag, e.g. attribute setters
#define CHECK_I(cond, type, msg) \
    if(!(cond)) { \
        PyErr_SetString(PyExc_##type, msg); \
        return -1; \
    }

//------------------------------------------------------------------------------
//                                 Element
//------------------------------------------------------------------------------

namespace Element {

struct Element {
    PyObject_HEAD
    ::Element *elem;
};

PyObject* make(::Element *elem);

PyObject* ctor(PyTypeObject *Py_UNUSED(type), PyObject *Py_UNUSED(args), PyObject *Py_UNUSED(kwargs))
{
    PyErr_SetString(SchemaError, "creation of elements from Python code is forbidden");
    return nullptr;
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

PyObject* axial_length(Element *self, PyObject *Py_UNUSED(args))
{
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->axisLengthSI());
    Py_RETURN_NONE;
}

PyObject* optical_path(Element *self, PyObject *Py_UNUSED(args))
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

PyObject* offset(Element *self, PyObject *Py_UNUSED(args))
{
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->subRangeSI());
    Py_RETURN_NONE;
}

PyObject* index(Element *self, PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    auto idx = schema->elements().indexOf(self->elem);
    if (idx < 0)
        Py_RETURN_NONE;
    // For python code elements are numbered 1-based
    // as they are shown in the elements table
    return PyLong_FromLong(idx + 1);
}

PyObject* prev(Element *self, PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    auto prevElem = FunctionUtils::prevElem(schema, self->elem);
    if (!prevElem)
        Py_RETURN_NONE;
    return make(prevElem);
}

PyObject* next(Element *self, PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    auto nextElem = FunctionUtils::nextElem(schema, self->elem);
    if (!nextElem)
        Py_RETURN_NONE;
    return make(nextElem);
}

PyObject* disabled(Element *self, PyObject *Py_UNUSED(args))
{
    return PyBool_FromLong(self->elem->disabled());
}

PyObject* param(Element *self, PyObject *arg)
{
    CHECK_(PyUnicode_Check(arg), TypeError, "unsupported argument type, string expected")
    auto alias = QString::fromUtf8(PyUnicode_AsUTF8(arg));
    auto param = self->elem->param(alias);
    CHECK_(param, KeyError, "parameter not found")
    return PyFloat_FromDouble(param->value().toSi());
}

int set_offset(Element *self, PyObject *arg, void *closure)
{
    auto range = Z::Utils::asRange(self->elem);
    CHECK_I(range, AssertionError, "element does not have length parameter")
    double v = 0;
    if (PyFloat_Check(arg))
        v = PyFloat_AsDouble(arg);
    else if (PyLong_Check(arg))
        v = PyLong_AsLong(arg);
    else CHECK_I(false, TypeError, "unsupported argument type, number expected")
    range->setSubRangeSI(v);
    return 0;
}

PyMethodDef methods[] = {
    { "param", (PyCFunction)param, METH_O, "Return element's parameter value (in SI units) by alias" },
    { NULL }
};

PyGetSetDef getset[] = {
    GETTER(label, "Element's label"),
    GETTER(index, "Element's index in schema (1-based) or none if not found"),
    GETTER(length, "Length parameter (in m) or none if element is not a range"),
    GETTER(axial_length, "Axial length (in m) or none if element is not a range"),
    GETTER(optical_path, "Optical path (in m) or none if element is not a range"),
    GETTER(ior, "Refraction index or none if it's not supported in the element"),
    GETSET(offset, "Offset inside the element or none if element is not a range"),
    GETTER(prev, "Previous element respecting round-trip rules for different schema kinds (SW, SP, RR)"),
    GETTER(next, "Next element respecting round-trip rules for different schema kinds (SW, SP, RR)"),
    GETTER(disabled, "Whether the element is disabled"),
    { NULL }
};

PyTypeObject type = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "schema.Element",
    .tp_basicsize = sizeof(Element),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Optical element"),
    .tp_methods = methods,
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
//                                 Matrix
//------------------------------------------------------------------------------

namespace Matrix {

struct Matrix {
    PyObject_HEAD
    Z::Matrix matrix;
};

PyObject* ctor(PyTypeObject* Py_UNUSED(type), PyObject* Py_UNUSED(args), PyObject* Py_UNUSED(kwargs))
{
    PyErr_SetString(SchemaError, "direct creation of Matrix objects is not allowed");
    return nullptr;
}

PyObject* A(Matrix *self, PyObject *Py_UNUSED(args))
{
    if (Z::isReal(self->matrix.A))
        return PyFloat_FromDouble(self->matrix.A.real());
    return PyComplex_FromDoubles(self->matrix.A.real(), self->matrix.A.imag());
}

PyObject* B(Matrix *self, PyObject *Py_UNUSED(args))
{
    if (Z::isReal(self->matrix.B))
        return PyFloat_FromDouble(self->matrix.B.real());
    return PyComplex_FromDoubles(self->matrix.B.real(), self->matrix.B.imag());
}

PyObject* C(Matrix *self, PyObject *Py_UNUSED(args))
{
    if (Z::isReal(self->matrix.C))
        return PyFloat_FromDouble(self->matrix.C.real());
    return PyComplex_FromDoubles(self->matrix.C.real(), self->matrix.C.imag());
}

PyObject* D(Matrix *self, PyObject *Py_UNUSED(args))
{
    if (Z::isReal(self->matrix.D))
        return PyFloat_FromDouble(self->matrix.D.real());
    return PyComplex_FromDoubles(self->matrix.D.real(), self->matrix.D.imag());
}

PyObject* str_repr(Matrix *self)
{
    auto s = self->matrix.str().toUtf8();
    return PyUnicode_FromString(s.constData());
}

PyGetSetDef getset[] = {
    GETTER(A, "Matrix element A"),
    GETTER(B, "Matrix element B"),
    GETTER(C, "Matrix element C"),
    GETTER(D, "Matrix element D"),
    { NULL }
};

PyTypeObject type = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "schema.Matrix",
    .tp_basicsize = sizeof(Matrix),
    .tp_itemsize = 0,
    .tp_repr = (reprfunc)str_repr,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("ABCD ray matrix"),
    .tp_getset = getset,
    .tp_new = ctor,
};

PyObject* make(const Z::Matrix& matrix)
{
    CHECK_TYPE_READY
    auto obj = type.tp_alloc(&type, 0);
    if (obj) {
        ((Matrix*)obj)->matrix = matrix;
    }
    return obj;
}

} // namespace Matrix

//------------------------------------------------------------------------------
//                               RoundTrip
//------------------------------------------------------------------------------

namespace RoundTrip {

struct RoundTrip {
    PyObject_HEAD
    BeamCalculator* calc;
    bool ownCalc;
};

PyObject* ctor(PyTypeObject* Py_UNUSED(type), PyObject* Py_UNUSED(args), PyObject* Py_UNUSED(kwargs))
{
    PyErr_SetString(SchemaError, "direct creation of RoundTrip objects is not allowed, use schema.round_trip()");
    return nullptr;
}

void dtor(RoundTrip *self)
{
    if (self->ownCalc)
        delete self->calc;
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* plane(RoundTrip *self, PyObject *Py_UNUSED(args))
{
    return PyLong_FromLong(self->calc->plane());
    Py_RETURN_NONE;
}

PyObject* ior(RoundTrip *self, PyObject *Py_UNUSED(args))
{
    return PyFloat_FromDouble(self->calc->ior());
    Py_RETURN_NONE;
}

PyObject* ref(RoundTrip *self, PyObject *Py_UNUSED(args))
{
    if (!self->calc->ref())
        Py_RETURN_NONE;
    return Element::make(self->calc->ref());
}

PyObject* beam_radius(RoundTrip* self, PyObject* Py_UNUSED(arg))
{
    // We do calculate round-trip matrix before each call for some beam parameter (size, ROC, angle)
    // Often this is unnecessary work and such cases are optimized in c++ core (table and plot funcs)
    // but custom Python code is not for fastest computations but rather for convenience
    // This excessive multiplications allows for code like
    //
    //     rt = schema.round_trip(ref=elem)
    //
    //     elem.offset = 0
    //     w1 = rt.beam_size()
    //
    //     elem.offset = elem.length/2
    //     w2 = rt.beam_size()
    //
    // without, we'd have to re-create round-trip after each offse change
    //
    self->calc->multMatrix("py.schema.beam_radius()");
    return PyFloat_FromDouble(self->calc->beamRadius());
}

PyObject* front_radius(RoundTrip* self, PyObject* Py_UNUSED(arg))
{
    self->calc->multMatrix("py.schema.front_radius()");
    return PyFloat_FromDouble(self->calc->frontRadius());
}

PyObject* half_angle(RoundTrip* self, PyObject* Py_UNUSED(arg))
{
    self->calc->multMatrix("py.schema.half_angle()");
    return PyFloat_FromDouble(self->calc->halfAngle());
}

PyObject* beam(RoundTrip* self, PyObject* Py_UNUSED(arg))
{
    self->calc->multMatrix("py.schema.beam()");
    auto result = self->calc->calc();
    return Py_BuildValue("(ddd)", result.beamRadius, result.frontRadius, result.halfAngle);
}

PyObject* matrix(RoundTrip* self, PyObject* Py_UNUSED(args))
{
    self->calc->multMatrix("py.schema.matrix()");
    return Matrix::make(self->calc->matrix());
}

PyObject* stabil_nor(RoundTrip* self, PyObject* Py_UNUSED(args))
{
    self->calc->multMatrix("py.schema.stabil_nor()");
    auto s = self->calc->stability_normal();
    if (qIsNaN(s))
        Py_RETURN_NONE;
    return PyFloat_FromDouble(s);
}

PyObject* stabil_sqr(RoundTrip* self, PyObject* Py_UNUSED(args))
{
    self->calc->multMatrix("py.schema.stabil_sqr()");
    auto s = self->calc->stability_squared();
    if (qIsNaN(s))
        Py_RETURN_NONE;
    return PyFloat_FromDouble(s);
}

PyMethodDef methods[] = {
    { "beam_radius", (PyCFunction)beam_radius, METH_NOARGS, "Calculate beam radius (in m)" },
    { "front_radius", (PyCFunction)front_radius, METH_NOARGS, "Calculate wavefront radius (in m)" },
    { "half_angle", (PyCFunction)half_angle, METH_NOARGS, "Calculate half of divergence angle in the far-field (in rad)" },
    { "beam", (PyCFunction)beam, METH_NOARGS, "Calculate beam parameters and return as tuple (beam_radius, front_radius, half_angle) in m, m, rad" },
    { NULL }
};

// PyMemberDef members[] = {
//     { "plane", Py_T_INT, offsetof(RoundTrip, plane), 0, "Work plane (one of Z.PLANE_T or Z.PLANE_S)" },
//     { "ior", Py_T_DOUBLE, offsetof(RoundTrip, ior), 0, "Current index of refraction" },
//     { NULL }
// };

PyGetSetDef getset[] = {
    GETTER(ref, "Reference element of the round-trip"),
    GETTER(plane, "Work plane (one of Z.PLANE_T or Z.PLANE_S)"),
    GETTER(ior, "Current index of refraction used for beam parameters calculation"),
    GETTER(matrix, "Round-trip matrix"),
    GETTER(stabil_nor, "Stability parameter (normal mode): P = (A + D)/2"),
    GETTER(stabil_sqr, "Stability parameter (squared mode): P = 1 - ((A + D)/2)^2"),
    { NULL }
};

PyTypeObject type = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "schema.RoundTrip",
    .tp_basicsize = sizeof(RoundTrip),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)dtor,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Round trip calculator"),
    .tp_methods = methods,
    //.tp_members = members,
    .tp_getset = getset,
    .tp_new = ctor,
};

PyObject* make(BeamCalculator* calc, bool ownCalc)
{
    CHECK_TYPE_READY
    auto obj = type.tp_alloc(&type, 0);
    if (obj) {
        ((RoundTrip*)obj)->calc = calc;
        ((RoundTrip*)obj)->ownCalc = ownCalc;
    }
    return obj;
}

} // namespace RoundTrip

//------------------------------------------------------------------------------
//                                 Schema
//------------------------------------------------------------------------------

namespace Methods {

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
        elem = schema->element(index-1);
        CHECK_(elem, IndexError, "element not found")
    } else if (PyUnicode_Check(arg)) {
        auto label = QString::fromUtf8(PyUnicode_AsUTF8(arg));
        elem = schema->element(label);
        CHECK_(elem, KeyError, "element not found")
    } else
        CHECK_(false, TypeError, "unsupported type of argument, integer or string expected")
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

PyObject* round_trip(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args), PyObject *kwargs)
{
    CHECK_SCHEMA
    if (schema->activeCount() == 0) {
        PyErr_SetString(PyExc_KeyError, "there are no active elements in the schema");
        return nullptr;
    }
    auto splitRange = false;
    auto workPlane = Z::WorkPlane::T;
    ::Element *refElem = nullptr;
    if (auto arg = PyDict_GetItemString(kwargs, "ref"); arg) {
        if (PyLong_Check(arg)) {
            auto index = PyLong_AsLong(arg);
            // For python code elemens are numbered 1-based
            // as they are shown in the elements table
            refElem = schema->element(index-1);
            CHECK_(refElem, IndexError, "reference element not found")
        } else if (PyUnicode_Check(arg)) {
            auto label = QString::fromUtf8(PyUnicode_AsUTF8(arg));
            refElem = schema->element(label);
            CHECK_(refElem, KeyError, "reference element not found")
        } else if (PyObject_TypeCheck(arg, &Element::type)) {
            refElem = ((Element::Element*)arg)->elem;
            CHECK_(refElem, ValueError, "element reference is null")
            CHECK_(schema->elements().contains(refElem), ValueError, "reference element not found")
        } else {
            CHECK_(false, TypeError, "wrong type of the 'ref' arg, integer, string, or Element expected");
        }
    }
    if (auto arg = PyDict_GetItemString(kwargs, "plane"); arg) {
        CHECK_(PyLong_Check(arg), TypeError, "wrong type of the 'plane' arg, integer expected")
        auto plane = PyLong_AsLong(arg);
        CHECK_(plane == Z::WorkPlane::T || plane == Z::WorkPlane::S, ValueError, 
            "unexpected value of the 'plane' arg, expected one of Z.PLANE_T or Z.PLANE_S")
        workPlane = (Z::WorkPlane)plane;
    }
    if (auto arg = PyDict_GetItemString(kwargs, "inside"); arg) {
        CHECK_(PyBool_Check(arg), TypeError, "wrong type of the 'inside' arg, bool expected")
        splitRange = Py_IsTrue(arg);
    }
    if (!refElem)
        refElem = schema->elements().last();
    auto beamCalc = new BeamCalculator(schema);
    if (!beamCalc->ok()) {
        PyErr_SetQString(PyExc_AssertionError, beamCalc->error());
        delete beamCalc;
        return nullptr;
    }
    beamCalc->calcRoundTrip(refElem, splitRange, "py.schema.round_trip()");
    beamCalc->setPlane(workPlane);
    beamCalc->setIor(FunctionUtils::ior(schema, refElem, splitRange));
    return RoundTrip::make(beamCalc, true);
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
    ADD_TYPE(Matrix)
    ADD_TYPE(RoundTrip)

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
    { "round_trip", (PyCFunction)Methods::round_trip, METH_VARARGS | METH_KEYWORDS,
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

#undef CHECK_
#undef CHECK_I
#undef CHECK_SCHEMA
#endif // PY_MODULE_SCHEMA_H
