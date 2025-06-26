#ifndef PY_MODULE_SCHEMA_H
#define PY_MODULE_SCHEMA_H

#include "PyUtils.h"
#include "Schema.h"

namespace PyModuleSchema {

const char *name = "schema";

Schema *schema = nullptr;

PyObject *SchemaError = nullptr;

//------------------------------------------------------------------------------
//                                 Element
//------------------------------------------------------------------------------

#define CHECK_ELEM \
    if (!self->elem) { \
        PyErr_SetString(SchemaError, "element reference is not provided"); \
        return nullptr; \
    }

struct Element {
    PyObject_HEAD
    ::Element *elem = nullptr;
    int number;
};

PyObject* Element_new(PyTypeObject *Py_UNUSED(type), PyObject *Py_UNUSED(args), PyObject *Py_UNUSED(kwargs))
{
    PyErr_SetString(SchemaError, "creation of elements from Python code is forbidden");
    return nullptr;
}

void Element_dealloc(Element *self)
{
    //qDebug() << "Dealloc";
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* Element_label(Element *self, PyObject *Py_UNUSED(args))
{
    CHECK_ELEM
    auto label = self->elem->label().toUtf8();
    return PyUnicode_FromString(label.constData());
}

PyObject* Element_length(Element *self, PyObject *Py_UNUSED(args))
{
    CHECK_ELEM
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->lengthSI());
    Py_RETURN_NONE;
}

PyObject* Element_optical_length(Element *self, PyObject *Py_UNUSED(args))
{
    CHECK_ELEM
    if (auto range = Z::Utils::asRange(self->elem); range)
        return PyFloat_FromDouble(range->opticalPathSI());
    Py_RETURN_NONE;
}

PyMethodDef Element_methods[] = {
    { "label", (PyCFunction)Element_label, METH_NOARGS, "Return element label" },
    { "length", (PyCFunction)Element_length, METH_NOARGS, "Return element length (in m) or null if element is not a range" },
    { "optical_length", (PyCFunction)Element_optical_length, METH_NOARGS, "Return element optical length (in m) or null if element is not a range" },
    { NULL }
};

PyMemberDef Element_members[] = {
    { "number", Py_T_INT, offsetof(Element, number), 0, "custom number" },
    { NULL }
};

PyTypeObject ElementType = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "schema.Element",
    .tp_basicsize = sizeof(Element),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)Element_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Optical element"),
    .tp_methods = Element_methods,
    .tp_members = Element_members,
    .tp_new = Element_new,
};

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
        auto err = QStringLiteral("parameter not found: %1").arg(alias).toUtf8();
        PyErr_SetString(SchemaError, err.constData());
        return nullptr;
    }
    return PyFloat_FromDouble(p->value().toSi());
}

PyObject* elem(PyObject* Py_UNUSED(self), PyObject* args)
{
    CHECK_SCHEMA
    auto pArg = PyTuple_GetItem(args, 0);
    if (!pArg)
        return nullptr;
    ::Element *elem = nullptr;
    if (PyLong_Check(pArg)) {
        auto index = PyLong_AsInt(pArg);
        elem = schema->element(index);
        if (!elem) {
            PyErr_SetString(PyExc_IndexError, "element not found");
            return nullptr;
        }
    } else if (PyUnicode_Check(pArg)) {
        auto label = QString::fromUtf8(PyUnicode_AsUTF8(pArg));
        elem = schema->element(label);
        if (!elem) {
            PyErr_SetString(PyExc_KeyError, "element not found");
            return nullptr;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "unsupported type of argument");
        return nullptr;
    }
    auto pElem = (Element*)ElementType.tp_alloc(&ElementType, 0);
    if (pElem) {
        pElem->elem = elem;
        pElem->number = 42;
    }
    return (PyObject*)pElem;
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

int on_exec(PyObject *module)
{
    if (!SchemaError) {
        SchemaError = PyErr_NewException("schema.error", NULL, NULL);
        if (PyModule_AddObjectRef(module, "SchemaError", SchemaError) < 0)
            STOP_MODULE_INIT
    }
    
    if (PyType_Ready(&ElementType) < 0)
        STOP_MODULE_INIT

    if (PyModule_AddObjectRef(module, "Element", (PyObject*)&ElementType) < 0)
        STOP_MODULE_INIT
    
    qDebug() << "schema module executed";
    return 0;
}

PyMethodDef methods[] = {
    { "elem", Methods::elem, METH_VARARGS, "Return element by label or index" },
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

} // namespace PyModuleSchema


#endif // PY_MODULE_SCHEMA_H
