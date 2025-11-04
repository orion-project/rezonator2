#ifndef PY_CLASS_MATRIX_H
#define PY_CLASS_MATRIX_H

#include "Math.h"
#include "PyClassRayVector.h"
#include "PyUtils.h"

namespace PyClass::Matrix {

const char *moduleName = nullptr;

PyTypeObject* type();
PyObject* make(const Z::Matrix& matrix);

struct Self
{
    PyObject_HEAD
    Z::Matrix matrix;
};

PyObject* ctor(PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    double a = 1, b = 0, c = 0, d = 1;
    if (!PyArg_ParseTuple(args, "|dddd", &a, &b, &c, &d))
        return nullptr;
    return make(Z::Matrix(a, b, c, d));
}

#define M_GET(x) \
    PyObject* x(Self *self, PyObject *Py_UNUSED(args)) { \
        if (Z::isReal(self->matrix.x)) \
            return PyFloat_FromDouble(self->matrix.x.real()); \
        return PyComplex_FromDoubles(self->matrix.x.real(), self->matrix.x.imag()); \
    }

#define M_SET(x) \
    int set_##x(Self *self, PyObject *arg) { \
        if (PyComplex_Check(arg)) { \
            auto c = PyComplex_AsCComplex(arg); \
            self->matrix.x = Z::Complex(c.real, c.imag); \
        } else if (PyFloat_Check(arg)) { \
            self->matrix.x = PyFloat_AsDouble(arg); \
        } else if (PyLong_Check(arg)) { \
            self->matrix.x = PyLong_AsLong(arg); \
        } else { \
            CHECK_I(false, TypeError, "unsupported argument type, number expected") \
        } \
        return 0; \
    }

M_GET(A) M_GET(B)
M_GET(C) M_GET(D)

M_SET(A) M_SET(B)
M_SET(C) M_SET(D)

#undef M_GET
#undef M_SET

PyObject* __repr__(Self *self)
{
    auto s = self->matrix.str().toUtf8();
    return PyUnicode_FromString(s.constData());
}

PyObject* __mul__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, type()), TypeError, "left operand must be a Matrix")
    
    if (PyObject_TypeCheck(right, type())) {
        auto m1 = (Self*)left;
        auto m2 = (Self*)right;
        auto result = m1->matrix * m2->matrix;
        return make(result);
    }
    
    if (PyObject_TypeCheck(right, RayVector::type())) {
        auto m = (Self*)left;
        auto v = (RayVector::Self*)right;
        Z::RayVector result(v->vector, m->matrix);
        return RayVector::make(result);
    }
    
    PyErr_SetString(PyExc_TypeError, "right operand must be a Matrix or RayVector");
    return nullptr;
}

PyObject* __imul__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, type()), TypeError, "left operand must be a Matrix")
    CHECK_(PyObject_TypeCheck(right, type()), TypeError, "right operand must be a Matrix")
    auto m1 = (Self*)left;
    auto m2 = (Self*)right;
    m1->matrix *= m2->matrix;
    return make(m1->matrix);
}

Py_ssize_t __len__(Self *Py_UNUSED(self))
{
    return 4;
}

PyObject* __getitem__(Self *self, Py_ssize_t index)
{
    switch(index) {
        case 0: return A(self, nullptr);
        case 1: return B(self, nullptr);
        case 2: return C(self, nullptr);
        case 3: return D(self, nullptr);
        default:
            PyErr_SetString(PyExc_IndexError, "Matrix index out of range (valid indices: 0-3)");
            return nullptr;
    }
}

PyTypeObject* type()
{
    static PyNumberMethods number_methods = {
        .nb_multiply = __mul__,
        .nb_inplace_multiply = __imul__,
    };
    
    static PySequenceMethods sequence_methods = {
        .sq_length = (lenfunc)__len__,
        .sq_item = (ssizeargfunc)__getitem__,
    };
    
    static PyGetSetDef getset[] = {
        GETSET(A, "Matrix element A"),
        GETSET(B, "Matrix element B"),
        GETSET(C, "Matrix element C"),
        GETSET(D, "Matrix element D"),
        { NULL }
    };
    
    static QByteArray typeName = QString("%1.Matrix").arg(moduleName).toUtf8();
    
    static PyTypeObject type = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = typeName.constData(),
        .tp_basicsize = sizeof(Self),
        .tp_itemsize = 0,
        .tp_repr = (reprfunc)__repr__,
        .tp_as_number = &number_methods,
        .tp_as_sequence = &sequence_methods,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = PyDoc_STR("ABCD ray matrix"),
        .tp_getset = getset,
        .tp_new = ctor,
    };
    
    return &type;
}

PyObject* make(const Z::Matrix& matrix)
{
    MAKE_OBJECT
    if (obj) {
        obj->matrix = matrix;
    }
    return (PyObject*)obj;
}

} // namespace PyClass::Matrix

#endif // PY_CLASS_MATRIX_H
