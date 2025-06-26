#include "PyUtils.h"
#include "PyRunner.h"
#include "PyModuleSchema.h"
#include "PyModuleGlobal.h"

#include <QDebug>

PyRunner::PyRunner()
{
    static bool inited = false;
    if (!inited) {
        if (PyImport_AppendInittab(PyModuleSchema::name, &PyModuleSchema::init) == -1)
            qWarning() << "Unable to register py module" << PyModuleSchema::name;
        if (PyImport_AppendInittab(PyModuleGlobal::name, &PyModuleGlobal::init) == -1)
            qWarning() << "Unable to register py module" << PyModuleGlobal::name;
        qDebug() << "Python" << Py_GetVersion();
        Py_Initialize();
        inited = true;
    }
}

PyRunner::~PyRunner()
{
    for (int i = _refs.size()-1; i>= 0; i--) {
        Py_DECREF((PyObject*)_refs.at(i));
    }
}

void PyRunner::run()
{
    PyModuleSchema::schema = schema;
    PyModuleGlobal::printFunc = logInfo;
    
    auto bCode = code.toUtf8();
    auto bModuleName = moduleName.toUtf8();
    auto bFileName = moduleName.toUtf8();
    auto bFuncName = funcName.toUtf8();
    
    auto pCompiled = Py_CompileString(bCode.constData(), bFileName.constData(), Py_file_input);
    if (!pCompiled) {
        handleError("Failed to compile py code");
        return;
    }
    _refs << pCompiled;
    
    auto pModule = PyImport_ExecCodeModule(bModuleName.constData(), pCompiled);
    if (!pModule) {
        handleError("Failed to execute py module");
        return;
    }
    _refs << pModule;
    
    auto pFunc = PyObject_GetAttrString(pModule, bFuncName.constData());
    if (!pFunc) {
        handleError(QString("Failed to get function: ") + funcName);
        return;
    }
    _refs << pFunc;
    
    if (!PyCallable_Check(pFunc)) {
        handleError(QString("The object is not callable: ") + funcName);
        return;
    }
    auto pDoc = PyObject_GetAttrString(pFunc, "__doc__");
    if (pDoc) {
        if (PyUnicode_Check(pDoc)) {
            auto doc = QString::fromUtf8(PyUnicode_AsUTF8(pDoc));
            auto lines = doc.split('\n', Qt::SkipEmptyParts);
            if (!lines.empty())
                funcTitle = lines.first();
        } 
        Py_DECREF(pDoc);
    }
    
    auto pArgs = PyTuple_New(0);
    if (!pArgs) {
        handleError(QString("Failed to initialize function agrs: ") + funcName);
        return;
    }
    _refs << pArgs;
    
    auto pValue = PyObject_CallObject(pFunc, pArgs);
    if (!pValue) {
        handleError(QString("Failed to call function: ") + funcName);
        return;
    }
    _refs << pValue;
}

void PyRunner::handleError(const QString& msg)
{
    logError("ERROR: " + msg);
    auto exc = PyErr_GetRaisedException();
    if (!exc) {
        logError("No exception");
        return;
    }
    _refs << exc;
        
    auto traceback = PyImport_ImportModule("traceback");
    if (!traceback) {
        logError("Unable to import traceback module");
        PyErr_Print();
        return;
    }
    _refs << traceback;
    
    auto format = PyObject_GetAttrString(traceback, "format_exception");
    if (!format) {
        logError("Unable to import traceback.format_exception");
        PyErr_Print();
        return;
    }
    _refs << format;
    
    auto args = PyTuple_New(1);
    if (!args) {
        logError("Failed to init args for format_exception");
        PyErr_Print();
        return;
    }
    _refs << args;
                    
    if (PyTuple_SetItem(args, 0, exc) < 0) {
        logError("Failed to set format_exception arg");
        PyErr_Print();
        return;
    }

    auto items = PyObject_CallObject(format, args);
    if (!items) {
        logError("Failed to call format_exception");
        PyErr_Print();
        return;
    }
    _refs << items;

    auto iter = PyObject_GetIter(items);
    if (!iter) {
        logError("Failed to get iterator for exception lines");
        PyErr_Print();
        return;
    }
    _refs << iter;
    
    QStringList lines;
    auto item = PyIter_Next(iter);
    while (item) {
        if (PyUnicode_Check(item))
            lines << QString::fromUtf8(PyUnicode_AsUTF8(item));
        else lines << "Wrong item in exception lines, string expected\n";
        Py_DECREF(item);
        item = PyIter_Next(iter);
    }
    logError(lines.join(""));
}
