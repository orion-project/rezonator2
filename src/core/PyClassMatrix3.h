#ifndef PY_CLASS_MATRIX_3_H
#define PY_CLASS_MATRIX_3_H

#include "PyUtils.h"
#include "PyClassMatrix.h"
#include "Format.h"

namespace PyClass::Matrix3 {

const char *moduleName = nullptr;

PyTypeObject* type();
PyObject* make();

struct Self
{
    PyObject_HEAD
    Z::Complex A, B, E;
    Z::Complex C, D, F;
    Z::Complex G, H, I;
};

PyObject* ctor(PyTypeObject* Py_UNUSED(type), PyObject* arg, PyObject* Py_UNUSED(kwargs))
{
    // Support initializaiton only via an ABCD matrix; no direct element values
    PyObject* abcd = nullptr;
    if (PyArg_ParseTuple(arg, "O", &abcd)) {
        if (PyObject_TypeCheck(abcd, PyClass::Matrix::type())) {
            auto src = ((PyClass::Matrix::Self*)abcd)->matrix;
            auto tgt = (Self*)make();
            tgt->A = src.A;
            tgt->B = src.B;
            tgt->C = src.C;
            tgt->D = src.D;
            return (PyObject*)tgt;
        }
    }
    
    // Reset error from failed parse
    PyErr_Clear();
    
    // Create identity matrix (default constructor)
    return make();
}

#define M_GET(x) \
    PyObject* x(Self *self, PyObject *Py_UNUSED(args)) { \
        if (Z::isReal(self->x)) \
            return PyFloat_FromDouble(self->x.real()); \
        return PyComplex_FromDoubles(self->x.real(), self->x.imag()); \
    }

#define M_SET(x) \
    int set_##x(Self *self, PyObject *arg) { \
        if (PyComplex_Check(arg)) { \
            auto c = PyComplex_AsCComplex(arg); \
            self->x = Z::Complex(c.real, c.imag); \
        } else if (PyFloat_Check(arg)) { \
            self->x = PyFloat_AsDouble(arg); \
        } else if (PyLong_Check(arg)) { \
            self->x = PyLong_AsLong(arg); \
        } else { \
            CHECK_I(false, TypeError, "unsupported argument type, number expected") \
        } \
        return 0; \
    }

M_GET(A) M_GET(B) M_GET(E)
M_GET(C) M_GET(D) M_GET(F)
M_GET(G) M_GET(H) M_GET(I)

M_SET(A) M_SET(B) M_SET(E)
M_SET(C) M_SET(D) M_SET(F)
M_SET(G) M_SET(H) M_SET(I)

#undef M_GET
#undef M_SET

PyObject* __repr__(Self *self)
{
    #define FMT(x) Z::isReal(self->x) ? Z::str(self->x.real()) : Z::str(self->x)
    QString s = QString("Matrix3("
        "(%1, %2, %3), "
        "(%4, %5, %6), "
        "(%7, %8, %9))").arg(
        FMT(A), FMT(B), FMT(E),
        FMT(C), FMT(D), FMT(F),
        FMT(G), FMT(H), FMT(I));
    auto utf8 = s.toUtf8();
    return PyUnicode_FromString(utf8.constData());
    #undef FMT
}

#define MULT_3(m1, m2) \
    auto a = m1->A * m2->A + m1->B * m2->C + m1->E * m2->G; \
    auto b = m1->A * m2->B + m1->B * m2->D + m1->E * m2->H; \
    auto e = m1->A * m2->E + m1->B * m2->F + m1->E * m2->I; \
    \
    auto c = m1->C * m2->A + m1->D * m2->C + m1->F * m2->G; \
    auto d = m1->C * m2->B + m1->D * m2->D + m1->F * m2->H; \
    auto f = m1->C * m2->E + m1->D * m2->F + m1->F * m2->I; \
    \
    auto g = m1->G * m2->A + m1->H * m2->C + m1->I * m2->G; \
    auto h = m1->G * m2->B + m1->H * m2->D + m1->I * m2->H; \
    auto i = m1->G * m2->E + m1->H * m2->F + m1->I * m2->I;

PyObject* __mul__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, type()), TypeError, "left operand must be a Matrix3")
    CHECK_(PyObject_TypeCheck(right, type()), TypeError, "right operand must be a Matrix3")
    
    auto m1 = (Self*)left;
    auto m2 = (Self*)right;
    auto m = (Self*)make();
    
    MULT_3(m1, m2)
    
    m->A = a; m->B = b; m->E = e;
    m->C = c; m->D = d; m->F = f;
    m->G = g; m->H = h; m->I = i;
    
    return (PyObject*)m;
}

PyObject* __imul__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, type()), TypeError, "left operand must be a Matrix3")
    CHECK_(PyObject_TypeCheck(right, type()), TypeError, "right operand must be a Matrix3")
    
    auto m1 = (Self*)left;
    auto m2 = (Self*)right;
    
    MULT_3(m1, m2)

    m1->A = a; m1->B = b; m1->E = e;
    m1->C = c; m1->D = d; m1->F = f;
    m1->G = g; m1->H = h; m1->I = i;
    
    Py_INCREF(left);
    return left;
}

#undef MULT_3

Py_ssize_t __len__(Self *Py_UNUSED(self))
{
    return 9;
}

PyObject* __getitem__(Self *self, Py_ssize_t index)
{
    switch(index) {
        case 0: return A(self, nullptr);
        case 1: return B(self, nullptr);
        case 2: return E(self, nullptr);
        case 3: return C(self, nullptr);
        case 4: return D(self, nullptr);
        case 5: return F(self, nullptr);
        case 6: return G(self, nullptr);
        case 7: return H(self, nullptr);
        case 8: return I(self, nullptr);
        default:
            PyErr_SetString(PyExc_IndexError, "Matrix3 index out of range (valid indices: 0-8)");
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
        GETSET(A, "Matrix element A (row 1, col 1)"),
        GETSET(B, "Matrix element B (row 1, col 2)"),
        GETSET(E, "Matrix element E (row 1, col 3)"),
        GETSET(C, "Matrix element C (row 2, col 1)"),
        GETSET(D, "Matrix element D (row 2, col 2)"),
        GETSET(F, "Matrix element F (row 2, col 3)"),
        GETSET(G, "Matrix element G (row 3, col 1)"),
        GETSET(H, "Matrix element H (row 3, col 2)"),
        GETSET(I, "Matrix element I (row 3, col 3)"),
        { NULL }
    };
    
    static QByteArray typeName = QString("%1.Matrix3").arg(moduleName).toUtf8();
    
    static PyTypeObject type = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = typeName.constData(),
        .tp_basicsize = sizeof(Self),
        .tp_itemsize = 0,
        .tp_repr = (reprfunc)__repr__,
        .tp_as_number = &number_methods,
        .tp_as_sequence = &sequence_methods,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = PyDoc_STR("3x3 matrix for misalignment calculations"),
        .tp_getset = getset,
        .tp_new = ctor,
    };

    return &type;
}

PyObject* make()
{
    MAKE_OBJECT
    if (obj) {
        obj->A = 1; obj->B = 0; obj->E = 0;
        obj->C = 0; obj->D = 1; obj->F = 0;
        obj->G = 0; obj->H = 0; obj->I = 1;
    }
    return (PyObject*)obj;
}

} // namespace PyClass::Matrix3

#endif // PY_CLASS_MATRIX_3_H
