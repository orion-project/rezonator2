#ifndef PY_CLASS_ELEMENT_H
#define PY_CLASS_ELEMENT_H

#include "PyUtils.h"
#include "Schema.h"
#include "../math/FunctionUtils.h"

namespace PyClass::Element {

const char *moduleName = nullptr;

PyTypeObject* type();
PyObject* make(::Element *elem);

struct Self
{
    PyObject_HEAD
    ::Element *elem;
};

PyObject* ctor(PyTypeObject *Py_UNUSED(type), PyObject *Py_UNUSED(args), PyObject *Py_UNUSED(kwargs))
{
    PyErr_SetString(PyGlobal::SchemaError, "creation of elements from Python code is forbidden");
    return nullptr;
}

PyObject* label(Self *self, PyObject *Py_UNUSED(args))
{
    auto label = self->elem->label().toUtf8();
    return PyUnicode_FromString(label.constData());
}

PyObject* length(Self *self, PyObject *Py_UNUSED(args))
{
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->lengthSI());
    Py_RETURN_NONE;
}

PyObject* axial_length(Self *self, PyObject *Py_UNUSED(args))
{
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->axisLengthSI());
    Py_RETURN_NONE;
}

PyObject* optical_path(Self *self, PyObject *Py_UNUSED(args))
{
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->opticalPathSI());
    Py_RETURN_NONE;
}

PyObject* ior(Self *self, PyObject *Py_UNUSED(args))
{
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->ior());
    Py_RETURN_NONE;
}

PyObject* offset(Self *self, PyObject *Py_UNUSED(args))
{
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->subRangeSI());
    Py_RETURN_NONE;
}

PyObject* index(Self *self, PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    auto idx = SCHEMA->elements().indexOf(self->elem);
    if (idx < 0)
        Py_RETURN_NONE;
    // For python code elements are numbered 1-based
    // as they are shown in the elements table
    return PyLong_FromLong(idx + 1);
}

PyObject* prev(Self *self, PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    auto prevElem = FunctionUtils::prevElem(SCHEMA, self->elem);
    if (!prevElem)
        Py_RETURN_NONE;
    return make(prevElem);
}

PyObject* next(Self *self, PyObject *Py_UNUSED(args))
{
    CHECK_SCHEMA
    auto nextElem = FunctionUtils::nextElem(SCHEMA, self->elem);
    if (!nextElem)
        Py_RETURN_NONE;
    return make(nextElem);
}

PyObject* disabled(Self *self, PyObject *Py_UNUSED(args))
{
    return PyBool_FromLong(self->elem->disabled());
}

PyObject* param(Self *self, PyObject *args)
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

int set_offset(Self *self, PyObject *arg, void *closure)
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

PyTypeObject* type()
{
    static PyMethodDef methods[] = {
        { "param", (PyCFunction)param, METH_VARARGS, "Return element's parameter value (in SI units) by alias" },
        { NULL }
    };
    
    static PyGetSetDef getset[] = {
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
    
    static QByteArray typeName = QString("%1.Element").arg(moduleName).toUtf8();
    
    static PyTypeObject type = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = typeName.constData(),
        .tp_basicsize = sizeof(Self),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = PyDoc_STR("Optical element"),
        .tp_methods = methods,
        .tp_getset = getset,
        .tp_new = ctor,
    };

    return &type;
}

PyObject* make(::Element *elem)
{
    MAKE_OBJECT
    if (obj) {
        obj->elem = elem;
    }
    return (PyObject*)obj;
}

} // namespace PyClass::Element 

#endif // PY_CLASS_ELEMENT_H
