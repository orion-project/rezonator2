#ifndef Z_USE_PYTHON

#include "PyRunner.h"

PyRunner::PyRunner() {}
PyRunner::~PyRunner() {}
bool PyRunner::load() { errorLog << "Not emplemented"; return false; }
PyRunner::FuncResult PyRunner::run(const QString&, const Args&, const ResultSpec&) { return {}; }

#else

// Includes Python.h, should be first:
#include "PyUtils.h"
#include "PyRunner.h"
#include "PyModuleSchema.h"
#include "PyModuleGlobal.h"
#include "Units.h"
#include "../math/BeamCalcWrapper.h"

#include <QDebug>
#include <QDir>
#include <QRegularExpression>

#define MODULE_NAME "rezonator_customfunc"

PyRunner::PyRunner()
{
    static bool inited = false;
    if (inited) return;

    // Skip to error if initialization failed
    static QString initError;
    if (!initError.isEmpty()) {
        errorLog << initError;
        return;
    }

    qDebug() << "Python" << Py_GetVersion();

    PyConfig config;
    PyConfig_InitIsolatedConfig(&config);

    QDir homeDir(qApp->applicationDirPath() + "/python");
    if (!homeDir.exists()) {
#ifdef Q_OS_MAC
        homeDir = QDir(qApp->applicationDirPath() + "/../../../../vcpkg_installed/x64-osx");
#endif
#ifdef Q_OS_LINUX
        homeDir = QDir(qApp->applicationDirPath() + "/../vcpkg_installed/x64-linux");
#endif
#ifdef Q_OS_WINDOWS
        //homeDir = QDir(qApp->applicationDirPath() + "/../vcpkg_installed/x64-windows/tools/python3");
#endif
    }

    std::wstring homePath = homeDir.absolutePath().toStdWString();
    config.home = (wchar_t*)PyMem_RawCalloc(homePath.size(), sizeof(wchar_t));
    if (!config.home) {
        qCritical() << "Unable to allocate memory for PYTHONHOME";
        initError = "Unable to allocate memory for PYTHONHOME";
        errorLog << initError;
        PyConfig_Clear(&config);
        return;
    }
    memcpy(config.home, homePath.c_str(), homePath.size() * sizeof(wchar_t));

    ADD_MODULE(PyModules::Global)
    ADD_MODULE(PyModules::Schema)

    PyStatus status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        qCritical() << "Failed to initialize Python:" << status.func << status.err_msg;
        initError = QString("Failed to initialize Python: %1").arg(status.err_msg);
        errorLog << initError;
        PyConfig_Clear(&config);
        return;
    }

    qDebug() << "PYTHONHOME" << QString::fromStdWString(std::wstring(config.home));
    if (config.module_search_paths_set == 1)
        for (int i = 0; i < config.module_search_paths.length; i++)
            qDebug() << "PYTHONPATH" << QString::fromStdWString(std::wstring(config.module_search_paths.items[i]));

    // Import common modules first time to initialize their types (Element, etc.)
    // Don't release returned refs, they safely can exists the whole app lifetime
    INIT_MODULE(PyModules::Global)
    INIT_MODULE(PyModules::Schema)

    PyConfig_Clear(&config);
    inited = true;
}

PyRunner::~PyRunner()
{
    for (int i = _refs.size()-1; i>= 0; i--)
        Py_DECREF((PyObject*)_refs.at(i));

    PyModules::Schema::schema = nullptr;
    PyModules::Global::printFunc = nullptr;
}

struct TmpRefs
{
    ~TmpRefs()
    {
        for (int i = refs.size()-1; i>= 0; i--)
            Py_DECREF(refs.at(i));
    }
    
    void operator << (PyObject *ref) { refs << ref; }

    QVector<PyObject*> refs;
};

bool PyRunner::load()
{
    if (!errorLog.isEmpty()) return false;

    PyModules::Schema::schema = schema;
    PyModules::Global::printFunc = printFunc ? printFunc : [](const QString &s){ qDebug() << s; };
    
    auto bCode = code.toUtf8();
    
    TmpRefs refs;
    
    auto pOldModule = PyImport_ImportModule(MODULE_NAME);
    if (!pOldModule) {
        if (PyErr_ExceptionMatches(PyExc_ModuleNotFoundError)) {
            // This is expected, clean the exception
            refs << PyErr_GetRaisedException();
        } else {
            handleError("Failed to find existing module");
            return false;
        }
    } else {
        refs << pOldModule;
        // Python doesn't replace the whole module when reimports its code
        // It replaces only attributes that exist in the new code
        // If the new code doesn't contain docstring, the old doc is returned
        if (PyObject_SetAttrString(pOldModule, "__doc__", Py_None) < 0) {
            handleError("Failed to clean existing module");
            return false;
        }
    }
    
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
    
    auto pDoc = PyObject_GetAttrString(pModule, "__doc__");
    if (pDoc) {
        refs << pDoc;

        if (PyUnicode_Check(pDoc)) {
            auto doc = QString::fromUtf8(PyUnicode_AsUTF8(pDoc));
            auto lines = doc.split('\n', Qt::SkipEmptyParts);
            if (!lines.empty())
                codeTitle = lines.first();
        } 
    }
    
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
    }
    
    return true;
}

PyRunner::FuncResult PyRunner::run(const QString &funcName, const Args &args, const ResultSpec &resultSpec)
{
    if (!errorLog.isEmpty()) return {};

    if (!_funcRefs.contains(funcName)) {
        errorLog << "Function not found: " + funcName;
        return {};
    }
    auto pFunc = (PyObject*)_funcRefs[funcName];
    
    TmpRefs refs;

    auto pArgs = PyTuple_New(args.size());
    if (!pArgs) {
        handleError("Failed to allocate agrs", funcName);
        return {};
    }
    refs << pArgs;
   
    for (int i = 0; i < args.size(); i++) {
        auto argType = args.at(i).first;
        auto argValue = args.at(i).second;
        PyObject *pArg = nullptr;
        switch (argType) {
        case atElement:
            pArg = PyModules::Schema::Element::make(reinterpret_cast<Element*>(argValue));
            break;
        case atBeamCalc:
            pArg = PyModules::Schema::Calculator::make(reinterpret_cast<BeamCalcWrapper*>(argValue));
            break;
        }
        if (pArg) {
            if (PyTuple_SetItem(pArgs, i, pArg) < 0) {
                handleError(QString("Failed to set argument %1").arg(i), funcName);
                return {};
            }
        } else {
            handleError("Required argument is not provided");
            return {};
        }
    }
    
    auto pResult = PyObject_CallObject(pFunc, pArgs);
    if (!pResult) {
        handleError("Failed to call function", funcName);
        return {};
    }
    refs << pResult;
    
    Records result;

    if (resultSpec.isEmpty())
        return result;
    
    if (!PyList_Check(pResult)) {
        handleError("Bad result type, list expected", funcName);
        return {};
    }
    
    auto resultCount = PyList_Size(pResult);
    for (auto i = 0; i < resultCount; i++) {
        auto pItem = PyList_GetItem(pResult, i);
        if (!pItem) {
            handleError(QString("Unable to get result[%1]").arg(i), funcName);
            return {};
        }
        if (!PyDict_Check(pItem)) {
            handleError(QString("result[%1]: bad type, dict expected").arg(i), funcName);
            return {};
        }
        Record rec;
        for (auto f = resultSpec.cbegin(); f != resultSpec.cend(); f++) {
            auto keyBytes = f.key().toUtf8();
            const char *k = keyBytes.constData();
            #define CHECK_(cond, msg) \
                if (!cond) { \
                    handleError(QString("result[%1]['%2']: %3").arg(i).arg(k, msg), funcName); \
                    return {}; \
                }
            auto pKey = PyUnicode_FromString(k);
            CHECK_(pKey, "failed to convert key to PyObject");
            refs << pKey;
            CHECK_(PyDict_Contains(pItem, pKey), "field not found");
            auto pField = PyDict_GetItemString(pItem, k);
            CHECK_(pField, "failed to get field");
            switch (f.value()) {
            case ftNumber:
                if (PyFloat_Check(pField))
                    rec[k] = PyFloat_AsDouble(pField);
                else if (PyLong_Check(pField))
                    rec[k] = double(PyLong_AsLong(pField));
                else CHECK_(false, "number expected");
                break;
            case ftString:
                CHECK_(PyUnicode_Check(pField), "string expected");
                rec[k] = QString::fromUtf8(PyUnicode_AsUTF8(pField));
                break;
            case ftUnitDim: {
                CHECK_(PyLong_Check(pField), "dimension expected");
                auto dims = Z::Dims::dims();
                int dim = PyLong_AsLong(pField);
                CHECK_((dim >= 0 && dim < dims.size()), "bad dimension");
                rec[k] = QVariant::fromValue(dims.at(dim));
                break;
            }
            }
            #undef CHECK_
        }
        result << rec;
    }

    return result;
}

void PyRunner::handleError(const QString& msg, const QString &funcName)
{
    TmpRefs refs;

    if (funcName.isEmpty())
        errorLog << msg;
    else
        errorLog << msg + QString(" (function: %1)").arg(funcName);
    
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
        refs << item;
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
        item = PyIter_Next(iter);
    }
}

#endif // USE_PYTHON

QString PyRunner::errorText() const
{
    return errorLog.isEmpty() ? QString() : errorLog.first();
};
