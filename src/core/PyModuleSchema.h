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

PyObject* param(Element *self, PyObject *args)
{
    char *alias;
    double def = qQNaN();
    if (!PyArg_ParseTuple(args, "s|d", &alias, &def))
        return nullptr;
    auto param = self->elem->param(QString::fromUtf8(alias));
    if (!param) {
        if (!qIsNaN(def))
            return PyFloat_FromDouble(def);
        Py_RETURN_NONE;
    }
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
    { "param", (PyCFunction)param, METH_VARARGS, "Return element's parameter value (in SI units) by alias" },
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
//                                 RayVector
//------------------------------------------------------------------------------

namespace RayVector {

struct RayVector {
    PyObject_HEAD
    Z::RayVector vector;
};

PyObject* make(const Z::RayVector& vector);
PyTypeObject* __class__();

PyObject* ctor(PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    double y = 0, v = 0;
    if (!PyArg_ParseTuple(args, "|dd", &y, &v))
        return nullptr;
    return make(Z::RayVector(y, v));
}

PyObject* Y(RayVector *self, PyObject *Py_UNUSED(args))
{
    return PyFloat_FromDouble(self->vector.Y);
}

PyObject* V(RayVector *self, PyObject *Py_UNUSED(args))
{
    return PyFloat_FromDouble(self->vector.V);
}

int set_Y(RayVector *self, PyObject *arg, void *Py_UNUSED(closure))
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

int set_V(RayVector *self, PyObject *arg, void *Py_UNUSED(closure))
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

PyObject* __repr__(RayVector *self)
{
    auto s = self->vector.str().toUtf8();
    return PyUnicode_FromString(s.constData());
}

PyObject* __add__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, __class__()), TypeError, "left operand must be a RayVector")
    CHECK_(PyObject_TypeCheck(right, __class__()), TypeError, "right operand must be a RayVector")
    auto v1 = ((RayVector*)left);
    auto v2 = ((RayVector*)right);
    Z::RayVector result(v1->vector.Y + v2->vector.Y, v1->vector.V + v2->vector.V);
    return make(result);
}

PyObject* __iadd__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, __class__()), TypeError, "left operand must be a RayVector")
    CHECK_(PyObject_TypeCheck(right, __class__()), TypeError, "right operand must be a RayVector")
    auto v1 = ((RayVector*)left);
    auto v2 = ((RayVector*)right);
    Z::RayVector result(v1->vector.Y + v2->vector.Y, v1->vector.V + v2->vector.V);
    return make(result);
}

PyNumberMethods number_methods = {
    .nb_add = __add__,
    .nb_inplace_add = __iadd__,
};

Py_ssize_t __len__(RayVector *Py_UNUSED(self))
{
    return 2;
}

PyObject* __getitem__(RayVector *self, Py_ssize_t index)
{
    switch(index) {
        case 0: return Y(self, nullptr);
        case 1: return V(self, nullptr);
        default:
            PyErr_SetString(PyExc_IndexError, "RayVector index out of range (valid indices: 0-1)");
            return nullptr;
    }
}

PySequenceMethods sequence_methods = {
    .sq_length = (lenfunc)__len__,
    .sq_item = (ssizeargfunc)__getitem__,
};

PyGetSetDef getset[] = {
    GETSET(Y, "Ray vector Y coordinate"),
    GETSET(V, "Ray vector V (angle) coordinate"),
    { NULL }
};

PyTypeObject type = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "schema.RayVector",
    .tp_basicsize = sizeof(RayVector),
    .tp_itemsize = 0,
    .tp_repr = (reprfunc)__repr__,
    .tp_as_number = &number_methods,
    .tp_as_sequence = &sequence_methods,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Ray vector (Y, V)"),
    .tp_getset = getset,
    .tp_new = ctor,
};

PyTypeObject* __class__()
{
    return &type;
}

PyObject* make(const Z::RayVector& vector)
{
    CHECK_TYPE_READY
    auto obj = type.tp_alloc(&type, 0);
    if (obj) {
        ((RayVector*)obj)->vector = vector;
    }
    return obj;
}

} // namespace RayVector

//------------------------------------------------------------------------------
//                                 Matrix
//------------------------------------------------------------------------------

namespace Matrix {

struct Matrix {
    PyObject_HEAD
    Z::Matrix matrix;
};

PyObject* make(const Z::Matrix& matrix);
PyTypeObject* __class__();

PyObject* ctor(PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    double a = 1, b = 0, c = 0, d = 1;
    if (!PyArg_ParseTuple(args, "|dddd", &a, &b, &c, &d))
        return nullptr;
    return make(Z::Matrix(a, b, c, d));
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

PyObject* __repr__(Matrix *self)
{
    auto s = self->matrix.str().toUtf8();
    return PyUnicode_FromString(s.constData());
}

PyObject* __mul__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, __class__()), TypeError, "left operand must be a Matrix")
    
    if (PyObject_TypeCheck(right, __class__())) {
        auto m1 = ((Matrix*)left);
        auto m2 = ((Matrix*)right);
        auto result = m1->matrix * m2->matrix;
        return make(result);
    }
    
    if (PyObject_TypeCheck(right, &RayVector::type)) {
        auto m = (Matrix*)left;
        auto v = (RayVector::RayVector*)right;
        Z::RayVector result(v->vector, m->matrix);
        return RayVector::make(result);
    }
    
    PyErr_SetString(PyExc_TypeError, "right operand must be a Matrix or RayVector");
    return nullptr;
}

PyObject* __imul__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, __class__()), TypeError, "left operand must be a Matrix")
    CHECK_(PyObject_TypeCheck(right, __class__()), TypeError, "right operand must be a Matrix")
    auto m1 = ((Matrix*)left);
    auto m2 = ((Matrix*)right);
    m1->matrix *= m2->matrix;
    return make(m1->matrix);
}

PyNumberMethods number_methods = {
    .nb_multiply = __mul__,
    .nb_inplace_multiply = __imul__,
};

Py_ssize_t __len__(Matrix *Py_UNUSED(self))
{
    return 4;
}

PyObject* __getitem__(Matrix *self, Py_ssize_t index)
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

PySequenceMethods sequence_methods = {
    .sq_length = (lenfunc)__len__,
    .sq_item = (ssizeargfunc)__getitem__,
};

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
    .tp_repr = (reprfunc)__repr__,
    .tp_as_number = &number_methods,
    .tp_as_sequence = &sequence_methods,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("ABCD ray matrix"),
    .tp_getset = getset,
    .tp_new = ctor,
};

PyTypeObject* __class__()
{
    return &type;
}

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
//                                 Matrix3
//------------------------------------------------------------------------------

namespace Matrix3 {

struct Matrix3 {
    PyObject_HEAD
    double A, B, E;
    double C, D, F;
    double G, H;
    // Third row is always (G, H, 1)
};

PyObject* make(double a, double b, double e, double c, double d, double f, double g, double h);
PyTypeObject* __class__();

PyObject* ctor(PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    PyObject* matrixObj = nullptr;
    
    // Try to parse as Matrix object
    if (PyArg_ParseTuple(args, "O", &matrixObj)) {
        if (PyObject_TypeCheck(matrixObj, &Matrix::type)) {
            auto m = ((Matrix::Matrix*)matrixObj);
            // Initialize from Matrix: copy A, B, C, D and set E, F, G, H to 0
            double a = Z::isReal(m->matrix.A) ? m->matrix.A.real() : m->matrix.A.real();
            double b = Z::isReal(m->matrix.B) ? m->matrix.B.real() : m->matrix.B.real();
            double c = Z::isReal(m->matrix.C) ? m->matrix.C.real() : m->matrix.C.real();
            double d = Z::isReal(m->matrix.D) ? m->matrix.D.real() : m->matrix.D.real();
            return make(a, b, 0, c, d, 0, 0, 0);
        }
    }
    
    // Reset error from failed parse
    PyErr_Clear();
    
    // Create identity matrix (default constructor)
    return make(1, 0, 0, 0, 1, 0, 0, 0);
}

#define M_GET(x) \
    PyObject* x(Matrix3 *self, PyObject *Py_UNUSED(args)) { \
        return PyFloat_FromDouble(self->x); \
    }

#define M_SET(x) \
    int set_##x(Matrix3 *self, PyObject *arg) { \
        double v = 0; \
        if (PyFloat_Check(arg)) \
            v = PyFloat_AsDouble(arg); \
        else if (PyLong_Check(arg)) \
            v = PyLong_AsLong(arg); \
        else CHECK_I(false, TypeError, "unsupported argument type, number expected") \
        self->x = v; \
        return 0; \
    }

M_GET(A)
M_GET(B)
M_GET(C)
M_GET(D)
M_GET(E)
M_GET(F)
M_GET(G)
M_GET(H)

M_SET(E)
M_SET(F)
M_SET(G)
M_SET(H)

#undef M_GET
#undef M_SET

PyObject* __repr__(Matrix3 *self)
{
    QString s = QString("Matrix3((%1, %2, %3), (%4, %5, %6), (%7, %8, 1))")
        .arg(self->A).arg(self->B).arg(self->E)
        .arg(self->C).arg(self->D).arg(self->F)
        .arg(self->G).arg(self->H);
    auto utf8 = s.toUtf8();
    return PyUnicode_FromString(utf8.constData());
}

PyObject* __mul__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, __class__()), TypeError, "left operand must be a Matrix3")
    CHECK_(PyObject_TypeCheck(right, __class__()), TypeError, "right operand must be a Matrix3")
    
    auto m1 = (Matrix3*)left;
    auto m2 = (Matrix3*)right;
    
    // Matrix multiplication: result = m1 * m2
    // Row 1: (A, B, E) * matrix2
    double a = m1->A * m2->A + m1->B * m2->C + m1->E * m2->G;
    double b = m1->A * m2->B + m1->B * m2->D + m1->E * m2->H;
    double e = m1->A * m2->E + m1->B * m2->F + m1->E * 1.0;
    
    // Row 2: (C, D, F) * matrix2
    double c = m1->C * m2->A + m1->D * m2->C + m1->F * m2->G;
    double d = m1->C * m2->B + m1->D * m2->D + m1->F * m2->H;
    double f = m1->C * m2->E + m1->D * m2->F + m1->F * 1.0;
    
    // Row 3: (G, H, 1) * matrix2
    double g = m1->G * m2->A + m1->H * m2->C + 1.0 * m2->G;
    double h = m1->G * m2->B + m1->H * m2->D + 1.0 * m2->H;
    // Last element would be: m1->G * m2->E + m1->H * m2->F + 1.0 * 1.0, but we keep it as 1
    
    return make(a, b, e, c, d, f, g, h);
}

PyObject* __imul__(PyObject *left, PyObject *right)
{
    CHECK_(PyObject_TypeCheck(left, __class__()), TypeError, "left operand must be a Matrix3")
    CHECK_(PyObject_TypeCheck(right, __class__()), TypeError, "right operand must be a Matrix3")
    
    auto m1 = (Matrix3*)left;
    auto m2 = (Matrix3*)right;
    
    // Save original values
    double a1 = m1->A, b1 = m1->B, e1 = m1->E;
    double c1 = m1->C, d1 = m1->D, f1 = m1->F;
    double g1 = m1->G, h1 = m1->H;
    
    // Multiply and store in-place
    m1->A = a1 * m2->A + b1 * m2->C + e1 * m2->G;
    m1->B = a1 * m2->B + b1 * m2->D + e1 * m2->H;
    m1->E = a1 * m2->E + b1 * m2->F + e1 * 1.0;
    
    m1->C = c1 * m2->A + d1 * m2->C + f1 * m2->G;
    m1->D = c1 * m2->B + d1 * m2->D + f1 * m2->H;
    m1->F = c1 * m2->E + d1 * m2->F + f1 * 1.0;
    
    m1->G = g1 * m2->A + h1 * m2->C + 1.0 * m2->G;
    m1->H = g1 * m2->B + h1 * m2->D + 1.0 * m2->H;
    
    Py_INCREF(left);
    return left;
}

PyNumberMethods number_methods = {
    .nb_multiply = __mul__,
    .nb_inplace_multiply = __imul__,
};

Py_ssize_t __len__(Matrix3 *Py_UNUSED(self))
{
    return 9;
}

PyObject* __getitem__(Matrix3 *self, Py_ssize_t index)
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
        case 8: return PyFloat_FromDouble(1.0);
        default:
            PyErr_SetString(PyExc_IndexError, "Matrix3 index out of range (valid indices: 0-8)");
            return nullptr;
    }
}

PySequenceMethods sequence_methods = {
    .sq_length = (lenfunc)__len__,
    .sq_item = (ssizeargfunc)__getitem__,
};

PyGetSetDef getset[] = {
    GETTER(A, "Matrix element A (row 1, col 1)"),
    GETTER(B, "Matrix element B (row 1, col 2)"),
    GETSET(E, "Matrix element E (row 1, col 3)"),
    GETTER(C, "Matrix element C (row 2, col 1)"),
    GETTER(D, "Matrix element D (row 2, col 2)"),
    GETSET(F, "Matrix element F (row 2, col 3)"),
    GETSET(G, "Matrix element G (row 3, col 1)"),
    GETSET(H, "Matrix element H (row 3, col 2)"),
    { NULL }
};

PyTypeObject type = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "schema.Matrix3",
    .tp_basicsize = sizeof(Matrix3),
    .tp_itemsize = 0,
    .tp_repr = (reprfunc)__repr__,
    .tp_as_number = &number_methods,
    .tp_as_sequence = &sequence_methods,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("3x3 matrix for misalignment calculations"),
    .tp_getset = getset,
    .tp_new = ctor,
};

PyTypeObject* __class__()
{
    return &type;
}

PyObject* make(double a, double b, double e, double c, double d, double f, double g, double h)
{
    CHECK_TYPE_READY
    auto obj = type.tp_alloc(&type, 0);
    if (obj) {
        auto m = (Matrix3*)obj;
        m->A = a;
        m->B = b;
        m->E = e;
        m->C = c;
        m->D = d;
        m->F = f;
        m->G = g;
        m->H = h;
    }
    return obj;
}

} // namespace Matrix3

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

PyObject* elem(RoundTrip* self, PyObject* arg)
{
    CHECK_(PyLong_Check(arg), TypeError, "invalid parameter, integer expected")
    auto elem = self->calc->elem(PyLong_AsLong(arg));
    CHECK_(elem, IndexError, "invalid elem index")
    return Element::make(elem);
}

PyObject* matrix(RoundTrip* self, PyObject* args)
{
    int index = -1;
    if (!PyArg_ParseTuple(args, "|i", &index))
        return nullptr;
    if (index >= 0) {
        auto m = self->calc->matrix(index);
        CHECK_(m, IndexError, "invalid matrix index")
        return Matrix::make(m.value());
    }
    self->calc->multMatrix("py.schema.matrix()");
    return Matrix::make(self->calc->matrix());
}

PyObject* matrix_count(RoundTrip* self, PyObject* Py_UNUSED(arg))
{
    return PyLong_FromLong(self->calc->matrixCount());
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
    { "matrix", (PyCFunction)matrix, METH_VARARGS, "Call without arguments for round-trip matrix, or with integer index to get matrix by index" },
    { "elem", (PyCFunction)elem, METH_O, "Return an element owning the index-th matrix of the round-trip" },
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
    GETTER(matrix_count, "Number of matrices in the round-trip"),
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
    if (kwargs) {
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
    ADD_TYPE(Matrix3)
    ADD_TYPE(RayVector)
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
