#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "PyHelper.h"

#include <QDebug>

PyHelper::PyHelper() {}

void PyHelper::foo()
{
    PyStatus status;
    PyConfig config;
    PyConfig_InitPythonConfig(&config);
    
    status = PyConfig_SetBytesString(&config, &config.program_name, "rezonator");
    if (PyStatus_Exception(status)) {
        goto exception;
    }
    
    qDebug() << "PyConfig.program_name" << config.program_name;
    qDebug() << "PyConfig.pythonpath_env" << config.pythonpath_env;
    qDebug() << "PyConfig.home" << config.home;
    qDebug() << "PyConfig.stdlib_dir" << config.stdlib_dir;
    qDebug() << "PyConfig.executable" << config.executable;
    qDebug() << "PyConfig.base_executable" << config.base_executable;
    qDebug() << "PyConfig.prefix" << config.prefix;
    qDebug() << "PyConfig.base_prefix" << config.base_prefix;
    qDebug() << "PyConfig.exec_prefix" << config.exec_prefix;
    qDebug() << "PyConfig.base_exec_prefix" << config.base_exec_prefix;
    qDebug() << "PyConfig.run_command" << config.run_command;
    qDebug() << "PyConfig.run_module" << config.run_module;
    qDebug() << "PyConfig.run_filename" << config.run_filename;
    qDebug() << "PyConfig.sys_path_0" << config.sys_path_0;
    
    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        goto exception;
    }
    PyConfig_Clear(&config);
    
    PyRun_SimpleString("from time import time,ctime\n"
                       "print('Today is', ctime(time()))\n");
    
    if (Py_FinalizeEx() < 0) {
        qDebug() << "Failed to finalize Python";
    }
    
    qDebug() << "Runned";
    return;
    
exception:
    PyConfig_Clear(&config);
    Py_ExitStatusException(status);
}

#include "PyModuleSchema.h"
#include "PyModuleGlobal.h"

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
            qWarning() << "Python interpreter finalized";
        }
    }
    
    void handleError(const QString& msg, PyHelper::Logger &log)
    {
        log.error("ERROR: " + msg);
        auto exc = PyErr_GetRaisedException();
        if (!exc) {
            log.error("No exception");
            return;
        }
        refs << exc;
            
        auto traceback = PyImport_ImportModule("traceback");
        if (!traceback) {
            log.error("Unable to import traceback module");
            PyErr_Print();
            return;
        }
        refs << traceback;
        
        auto format = PyObject_GetAttrString(traceback, "format_exception");
        if (!format) {
            log.error("Unable to import traceback.format_exception");
            PyErr_Print();
            return;
        }
        refs << format;
        
        auto args = PyTuple_New(1);
        if (!args) {
            log.error("Failed to init args for format_exception");
            PyErr_Print();
            return;
        }
        refs << args;
                        
        if (PyTuple_SetItem(args, 0, exc) < 0) {
            log.error("Failed to set format_exception arg");
            PyErr_Print();
            return;
        }

        auto items = PyObject_CallObject(format, args);
        if (!items) {
            log.error("Failed to call format_exception");
            PyErr_Print();
            return;
        }
        refs << items;

        auto iter = PyObject_GetIter(items);
        if (!iter) {
            log.error("Failed to get iterator for exception lines");
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
        log.error(lines.join(""));
    }
    
    QVector<PyObject*> refs;
};

void PyHelper::foo4(Schema *schema, const QString &code)
{
    PyModuleSchema::schema = schema;
    PyModuleGlobal::printFunc = log.info;
    
    PyRunner runner;

    const char *funcName = "show_lambda";

    auto codeBytes = code.toUtf8();
    
    PyObject *pCompiled = Py_CompileString(codeBytes.constData(), "customfunc.py", Py_file_input);
    if (!pCompiled) {
        runner.handleError("Failed to compile py code", log);
        return;
    }
    runner.refs << pCompiled;
    
    PyObject *pModule = PyImport_ExecCodeModule("customcode", pCompiled);
    if (!pModule) {
        runner.handleError("Failed to create py module", log);
        return;
    }
    runner.refs << pModule;
    
    PyObject *pFunc = PyObject_GetAttrString(pModule, funcName);
    if (!pFunc) {
        runner.handleError(QString("Failed to get function: ") + funcName, log);
        return;
    }
    runner.refs << pFunc;
    
    if (!PyCallable_Check(pFunc)) {
        runner.handleError(QString("The object is not callable: ") + funcName, log);
        return;
    }
    
    PyObject *pArgs = PyTuple_New(0);
    if (!pArgs) {
        runner.handleError(QString("Failed to initialize function agrs: ") + funcName, log);
        return;
    }
    runner.refs << pArgs;
    
    PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
    if (!pValue) {
        runner.handleError(QString("Failed to call function: ") + funcName, log);
        return;
    }
    runner.refs << pValue;

    qDebug() << "Result of call" << PyFloat_AsDouble(pValue);
}
