#include "PyUtils.h"
#include "PyHelper.h"
#include "PyModuleSchema.h"
#include "PyModuleGlobal.h"

#include <QDebug>

PyHelper::PyHelper() {}

struct PyRunner
{
    PyRunner()
    {
        static bool hasModules = false;
        if (!hasModules) {
            if (PyImport_AppendInittab(PyModuleSchema::name, &PyModuleSchema::init) == -1)
                qWarning() << "Unable to register py module" << PyModuleSchema::name;
            if (PyImport_AppendInittab(PyModuleGlobal::name, &PyModuleGlobal::init) == -1)
                qWarning() << "Unable to register py module" << PyModuleGlobal::name;
            hasModules = true;
            qDebug() << "Python" << Py_GetVersion();
        }
        
        Py_Initialize();
    }
    
    ~PyRunner()
    {
        for (int i = refs.size()-1; i>= 0; i--) {
            Py_DECREF(refs.at(i));
        }
        if (Py_FinalizeEx() < 0) {
            qWarning() << "Failed to finalize Python interpreter";
        } else {
            //qWarning() << "Python interpreter finalized";
        }
    }
    
    void handleError(const QString& msg, std::function<void(const QString&)> print)
    {
        print("ERROR: " + msg);
        auto exc = PyErr_GetRaisedException();
        if (!exc) {
            print("No exception");
            return;
        }
        refs << exc;
            
        auto traceback = PyImport_ImportModule("traceback");
        if (!traceback) {
            print("Unable to import traceback module");
            PyErr_Print();
            return;
        }
        refs << traceback;
        
        auto format = PyObject_GetAttrString(traceback, "format_exception");
        if (!format) {
            print("Unable to import traceback.format_exception");
            PyErr_Print();
            return;
        }
        refs << format;
        
        auto args = PyTuple_New(1);
        if (!args) {
            print("Failed to init args for format_exception");
            PyErr_Print();
            return;
        }
        refs << args;
                        
        if (PyTuple_SetItem(args, 0, exc) < 0) {
            print("Failed to set format_exception arg");
            PyErr_Print();
            return;
        }

        auto items = PyObject_CallObject(format, args);
        if (!items) {
            print("Failed to call format_exception");
            PyErr_Print();
            return;
        }
        refs << items;

        auto iter = PyObject_GetIter(items);
        if (!iter) {
            print("Failed to get iterator for exception lines");
            PyErr_Print();
            return;
        }
        refs << iter;
        
        QStringList lines;
        auto item = PyIter_Next(iter);
        while (item) {
            if (PyUnicode_Check(item))
                lines << QString::fromUtf8(PyUnicode_AsUTF8(item));
            else lines << "Wrong item in exception lines, string expected\n";
            Py_DECREF(item);
            item = PyIter_Next(iter);
        }
        print(lines.join(""));
    }
    
    QVector<PyObject*> refs;
};

void PyHelper::run()
{
    PyModuleSchema::schema = schema;
    PyModuleGlobal::printFunc = logInfo;
    
    PyRunner runner;

    auto bCode = code.toUtf8();
    auto bModuleName = moduleName.toUtf8();
    auto bFileName = moduleName.toUtf8();
    auto bFuncName = funcName.toUtf8();
    
    auto pCompiled = Py_CompileString(bCode.constData(), bFileName.constData(), Py_file_input);
    if (!pCompiled) {
        runner.handleError("Failed to compile py code", logError);
        return;
    }
    runner.refs << pCompiled;
    
    auto pModule = PyImport_ExecCodeModule(bModuleName.constData(), pCompiled);
    if (!pModule) {
        runner.handleError("Failed to execute py module", logError);
        return;
    }
    runner.refs << pModule;
    
    auto pFunc = PyObject_GetAttrString(pModule, bFuncName.constData());
    if (!pFunc) {
        runner.handleError(QString("Failed to get function: ") + funcName, logError);
        return;
    }
    runner.refs << pFunc;
    
    if (!PyCallable_Check(pFunc)) {
        runner.handleError(QString("The object is not callable: ") + funcName, logError);
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
        runner.handleError(QString("Failed to initialize function agrs: ") + funcName, logError);
        return;
    }
    runner.refs << pArgs;
    
    auto pValue = PyObject_CallObject(pFunc, pArgs);
    if (!pValue) {
        runner.handleError(QString("Failed to call function: ") + funcName, logError);
        return;
    }
    runner.refs << pValue;
}
