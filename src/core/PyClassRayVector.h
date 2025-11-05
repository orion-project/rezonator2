#ifndef PY_CLASS_RAY_VECTOR_H
#define PY_CLASS_RAY_VECTOR_H

#include "Math.h"
#include "PyUtils.h"

namespace PyClass::RayVector {

const char *moduleName = nullptr;

PyTypeObject* type();
PyObject* make(const Z::RayVector& vector);

struct Self
{
    PyObject_HEAD
    Z::RayVector vector;
};

PyObject* ctor(PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    double y = 0, v = 0;
    if (!PyArg_ParseTuple(args, "|dd", &y, &v))
        return nullptr;
    return make(Z::RayVector(y, v));
}

PyObject* Y(Self *self, PyObject *Py_UNUSED(args))
{
    return PyFloat_FromDouble(self->vector.Y);
}

PyObject* V(Self *self, PyObject *Py_UNUSED(args))
{
    return PyFloat_FromDouble(self->vector.V);
}

int set_Y(Self *self, PyObject *arg, void *Py_UNUSED(closure))
{
    double v = 0;
    if (PyFloat_Check(arg))
        v = PyFloat_AsDouble(arg);
    else if (PyLong_Check(arg))
        v = PyLong_AsLong(arg);
    else CHECK_I(false, TypeError, "unsupported argument type, number expected")
    self->vector.Y = v;
    return 0;
}

int set_V(Self *self, PyObject *arg, void *Py_UNUSED(closure))
{
    double v = 0;
    if (PyFloat_Check(arg))
        v = PyFloat_AsDouble(arg);
    else if (PyLong_Check(arg))
        v = PyLong_AsLong(arg);
    else CHECK_I(false, TypeError, "unsupported argument type, number expected")
    self->vector.V = v;
    return 0;
}

PyObject* __repr__(Self *self)
{
    auto s = self->vector.str().toUtf8();
    return PyUnicode_FromString(s.constData());
}

PyObject* __add__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, type()), TypeError, "left operand must be a RayVector")
    CHECK_(PyObject_TypeCheck(right, type()), TypeError, "right operand must be a RayVector")
    auto v1 = ((Self*)left);
    auto v2 = ((Self*)right);
    Z::RayVector result(v1->vector.Y + v2->vector.Y, v1->vector.V + v2->vector.V);
    return make(result);
}

PyObject* __iadd__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, type()), TypeError, "left operand must be a RayVector")
    CHECK_(PyObject_TypeCheck(right, type()), TypeError, "right operand must be a RayVector")
    auto v1 = ((Self*)left);
    auto v2 = ((Self*)right);
    Z::RayVector result(v1->vector.Y + v2->vector.Y, v1->vector.V + v2->vector.V);
    return make(result);
}

Py_ssize_t __len__(Self *Py_UNUSED(self))
{
    return 2;
}

PyObject* __getitem__(Self *self, Py_ssize_t index)
{
    switch(index) {
        case 0: return Y(self, nullptr);
        case 1: return V(self, nullptr);
        default:
            PyErr_SetString(PyExc_IndexError, "RayVector index out of range (valid indices: 0-1)");
            return nullptr;
    }
}

PyTypeObject* type()
{
    static PyNumberMethods number_methods = {
        .nb_add = __add__,
        .nb_inplace_add = __iadd__,
    };
    
    static PySequenceMethods sequence_methods = {
        .sq_length = (lenfunc)__len__,
        .sq_item = (ssizeargfunc)__getitem__,
    };
    
    static PyGetSetDef getset[] = {
        GETSET(Y, "Ray vector Y coordinate"),
        GETSET(V, "Ray vector V (angle) coordinate"),
        { NULL }
    };
    
    static QByteArray typeName = QString("%1.RayVector").arg(moduleName).toUtf8();
    
    static PyTypeObject type = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = typeName.constData(),
        .tp_basicsize = sizeof(Self),
        .tp_itemsize = 0,
        .tp_repr = (reprfunc)__repr__,
        .tp_as_number = &number_methods,
        .tp_as_sequence = &sequence_methods,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = PyDoc_STR("Ray vector (Y, V)"),
        .tp_getset = getset,
        .tp_new = ctor,
    };
    
    return &type;
}

PyObject* make(const Z::RayVector& vector)
{
    MAKE_OBJECT
    if (obj) {
        obj->vector = vector;
    }
    return (PyObject*)obj;
}

} // namespace PyClass::RayVector

#endif // PY_CLASS_RAY_VECTOR_H
