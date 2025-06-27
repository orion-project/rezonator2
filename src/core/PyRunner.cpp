#ifndef Z_USE_PYTHON

#include "PyRunner.h"

PyRunner::PyRunner() {}
PyRunner::~PyRunner() {}
bool PyRunner::load() { logError("Not emplemented"); return false; }
bool PyRunner::run(const QString&) { return false; }

#else

// Includes Python.h, should be first:
#include "PyUtils.h"
#include "PyRunner.h"
#include "PyModuleSchema.h"
#include "PyModuleGlobal.h"

#include <QDebug>
#include <QRegularExpression>

#define MODULE_NAME "rezonator_customfunc"

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

    PyModuleSchema::schema = nullptr;
    PyModuleGlobal::printFunc = nullptr;
}

struct TmpRefs
{
    ~TmpRefs()
    {
        for (int i = refs.size()-1; i>= 0; i--) {
            Py_DECREF(refs.at(i));
        }
    }
    
    void operator << (PyObject *ref) { refs << ref; }

    QVector<PyObject*> refs;
};

bool PyRunner::load()
{
    PyModuleSchema::schema = schema;
    PyModuleGlobal::printFunc = printFunc ? printFunc : [](const QString &s){ qDebug() << s; };
    
    auto bCode = code.toUtf8();
    
    TmpRefs refs;
    
    auto pCompiled = Py_CompileString(bCode.constData(), MODULE_NAME, Py_file_input);
    if (!pCompiled) {
        handleError("Failed to compile code");
        return false;
    }
    refs << pCompiled;
    
    auto pModule = PyImport_ExecCodeModule(MODULE_NAME, pCompiled);
    if (!pModule) {
        handleError("Failed to execute module");
        return false;
    }
    refs << pModule;
    
    for (const QString &funcName : std::as_const(funcNames)) {
        auto bFuncName = funcName.toUtf8();
    
        auto pFunc = PyObject_GetAttrString(pModule, bFuncName.constData());
        if (!pFunc) {
            handleError("Failed to get function " + funcName);
            return false;
        }
        _refs << pFunc;

        if (!PyCallable_Check(pFunc)) {
            handleError("The object is not callable: " + funcName);
            return false;
        }
        _funcRefs.insert(funcName, pFunc);

        auto pDoc = PyObject_GetAttrString(pFunc, "__doc__");
        if (pDoc) {
            refs << pDoc;

            if (PyUnicode_Check(pDoc)) {
                auto doc = QString::fromUtf8(PyUnicode_AsUTF8(pDoc));
                auto lines = doc.split('\n', Qt::SkipEmptyParts);
                if (!lines.empty())
                    funcTitles.insert(funcName, lines.first());
            } 
        }
    }
    
    return true;
}

bool PyRunner::run(const QString &funcName)
{
    if (!_funcRefs.contains(funcName)) {
        errorLog << "Function not found: " + funcName;
        return false;
    }
    auto pFunc = (PyObject*)_funcRefs[funcName];
    
    TmpRefs refs;

    auto pArgs = PyTuple_New(0);
    if (!pArgs) {
        handleError("Failed to allocate agrs for function " + funcName);
        return false;
    }
    refs << pArgs;
    
    auto pValue = PyObject_CallObject(pFunc, pArgs);
    if (!pValue) {
        handleError("Failed to call function " + funcName);
        return false;
    }
    refs << pValue;

    return true;
}

void PyRunner::handleError(const QString& msg)
{
    TmpRefs refs;

    errorLog << msg;
    auto exc = PyErr_GetRaisedException();
    if (!exc) {
        return;
    }
    refs << exc;
        
    auto traceback = PyImport_ImportModule("traceback");
    if (!traceback) {
        errorLog << "Unable to import traceback module";
        PyErr_Print();
        return;
    }
    refs << traceback;
    
    auto format = PyObject_GetAttrString(traceback, "format_exception");
    if (!format) {
        errorLog << "Unable to import traceback.format_exception";
        PyErr_Print();
        return;
    }
    refs << format;
    
    auto args = PyTuple_New(1);
    if (!args) {
        errorLog << "Failed to allocate args for format_exception";
        PyErr_Print();
        return;
    }
    refs << args;
                    
    if (PyTuple_SetItem(args, 0, exc) < 0) {
        errorLog << "Failed to init args for format_exception";
        PyErr_Print();
        return;
    }

    auto items = PyObject_CallObject(format, args);
    if (!items) {
        errorLog << "Failed to call format_exception";
        PyErr_Print();
        return;
    }
    refs << items;

    auto iter = PyObject_GetIter(items);
    if (!iter) {
        errorLog << "Failed to get iterator for exception lines";
        PyErr_Print();
        return;
    }
    refs << iter;
    
    auto item = PyIter_Next(iter);
    while (item) {
        QString line = QString::fromUtf8(PyUnicode_AsUTF8(item));
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.truncate(line.length()-1);
        if (!line.isEmpty()) {
            errorLog << line;
            if (errorLine == 0) {
                static QRegularExpression r(R"(File \"(.+)\",\s+line\s+(\d+))");
                if (auto m = r.match(line); m.hasMatch() && m.captured(1) == MODULE_NAME)
                    errorLine = m.captured(2).toInt();
            }
        }
        refs << item;
        item = PyIter_Next(iter);
    }
}

QString PyRunner::errorText() const
{
    return errorLog.isEmpty() ? QString() : errorLog.first();
};

#endif // USE_PYTHON
