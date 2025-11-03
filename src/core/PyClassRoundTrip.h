#ifndef PY_CLASS_ROUND_TRIP_H
#define PY_CLASS_ROUND_TRIP_H

#include "PyUtils.h"
#include "PyClassElement.h"
#include "PyClassMatrix.h"
#include "../math/BeamCalculator.h"

namespace PyClass::RoundTrip {

const char *moduleName;

PyTypeObject* type();
PyObject* make(BeamCalculator* calc, bool ownCalc);

struct Self
{
    PyObject_HEAD
    BeamCalculator* calc;
    bool ownCalc;
};

PyObject* ctor(PyTypeObject* Py_UNUSED(type), PyObject* Py_UNUSED(args), PyObject* Py_UNUSED(kwargs))
{
    PyErr_SetString(PyGlobal::SchemaError, "direct creation of RoundTrip objects is not allowed, use schema.round_trip()");
    return nullptr;
}

void dtor(Self *self)
{
    if (self->ownCalc)
        delete self->calc;
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* plane(Self *self, PyObject *Py_UNUSED(args))
{
    return PyLong_FromLong(self->calc->plane());
    Py_RETURN_NONE;
}

PyObject* ior(Self *self, PyObject *Py_UNUSED(args))
{
    return PyFloat_FromDouble(self->calc->ior());
    Py_RETURN_NONE;
}

PyObject* ref(Self *self, PyObject *Py_UNUSED(args))
{
    if (!self->calc->ref())
        Py_RETURN_NONE;
    return Element::make(self->calc->ref());
}

PyObject* beam_radius(Self* self, PyObject* Py_UNUSED(arg))
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
    self->calc->multMatrix("py.RoundTrip.beam_radius()");
    return PyFloat_FromDouble(self->calc->beamRadius());
}

PyObject* front_radius(Self* self, PyObject* Py_UNUSED(arg))
{
    self->calc->multMatrix("py.RoundTrip.front_radius()");
    return PyFloat_FromDouble(self->calc->frontRadius());
}

PyObject* half_angle(Self* self, PyObject* Py_UNUSED(arg))
{
    self->calc->multMatrix("py.RoundTrip.half_angle()");
    return PyFloat_FromDouble(self->calc->halfAngle());
}

PyObject* beam(Self* self, PyObject* Py_UNUSED(arg))
{
    self->calc->multMatrix("py.RoundTrip.beam()");
    auto result = self->calc->calc();
    return Py_BuildValue("(ddd)", result.beamRadius, result.frontRadius, result.halfAngle);
}

PyObject* elem(Self* self, PyObject* arg)
{
    CHECK_(PyLong_Check(arg), TypeError, "invalid parameter, integer expected")
    auto elem = self->calc->elem(PyLong_AsLong(arg));
    CHECK_(elem, IndexError, "invalid elem index")
    return Element::make(elem);
}

PyObject* matrix(Self* self, PyObject* args)
{
    int index = -1;
    if (!PyArg_ParseTuple(args, "|i", &index))
        return nullptr;
    if (index >= 0) {
        auto m = self->calc->matrix(index);
        CHECK_(m, IndexError, "invalid matrix index")
        return PyClass::Matrix::make(m.value());
    }
    self->calc->multMatrix("py.RoundTrip.matrix()");
    return PyClass::Matrix::make(self->calc->matrix());
}

PyObject* matrix_count(Self* self, PyObject* Py_UNUSED(arg))
{
    return PyLong_FromLong(self->calc->matrixCount());
}

PyObject* stabil_nor(Self* self, PyObject* Py_UNUSED(args))
{
    self->calc->multMatrix("py.RoundTrip.stabil_nor()");
    auto s = self->calc->stability_normal();
    if (qIsNaN(s))
        Py_RETURN_NONE;
    return PyFloat_FromDouble(s);
}

PyObject* stabil_sqr(Self* self, PyObject* Py_UNUSED(args))
{
    self->calc->multMatrix("py.RoundTrip.stabil_sqr()");
    auto s = self->calc->stability_squared();
    if (qIsNaN(s))
        Py_RETURN_NONE;
    return PyFloat_FromDouble(s);
}

PyTypeObject* type()
{
    static PyMethodDef methods[] = {
        { "beam_radius", (PyCFunction)beam_radius, METH_NOARGS, "Calculate beam radius (in m)" },
        { "front_radius", (PyCFunction)front_radius, METH_NOARGS, "Calculate wavefront radius (in m)" },
        { "half_angle", (PyCFunction)half_angle, METH_NOARGS, "Calculate half of divergence angle in the far-field (in rad)" },
        { "beam", (PyCFunction)beam, METH_NOARGS, "Calculate beam parameters and return as tuple (beam_radius, front_radius, half_angle) in m, m, rad" },
        { "matrix", (PyCFunction)matrix, METH_VARARGS, "Call without arguments for round-trip matrix, or with integer index to get matrix by index" },
        { "elem", (PyCFunction)elem, METH_O, "Return an element owning the index-th matrix of the round-trip" },
        { NULL }
    };
    
    // static PyMemberDef members[] = {
    //     { "plane", Py_T_INT, offsetof(RoundTrip, plane), 0, "Work plane (one of Z.PLANE_T or Z.PLANE_S)" },
    //     { "ior", Py_T_DOUBLE, offsetof(RoundTrip, ior), 0, "Current index of refraction" },
    //     { NULL }
    // };
    
    static PyGetSetDef getset[] = {
        GETTER(ref, "Reference element of the round-trip"),
        GETTER(plane, "Work plane (one of Z.PLANE_T or Z.PLANE_S)"),
        GETTER(ior, "Current index of refraction used for beam parameters calculation"),
        GETTER(matrix_count, "Number of matrices in the round-trip"),
        GETTER(stabil_nor, "Stability parameter (normal mode): P = (A + D)/2"),
        GETTER(stabil_sqr, "Stability parameter (squared mode): P = 1 - ((A + D)/2)^2"),
        { NULL }
    };
    
    static QByteArray typeName = QString("%1.RoundTrip").arg(moduleName).toUtf8();
    
    static PyTypeObject type = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = typeName.constData(),
        .tp_basicsize = sizeof(Self),
        .tp_itemsize = 0,
        .tp_dealloc = (destructor)dtor,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = PyDoc_STR("Round trip calculator"),
        .tp_methods = methods,
        //.tp_members = members,
        .tp_getset = getset,
        .tp_new = ctor,
    };

    return &type;
}

PyObject* make(BeamCalculator* calc, bool ownCalc)
{
    MAKE_OBJECT
    if (obj) {
        obj->calc = calc;
        obj->ownCalc = ownCalc;
    }
    return (PyObject*)obj;
}

} // namespace PyClass::RoundTrip

#endif // PY_CLASS_ROUND_TRIP_H
