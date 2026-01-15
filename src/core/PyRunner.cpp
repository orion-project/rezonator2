#ifndef Z_USE_PYTHON

#include "PyRunner.h"

PyRunner::PyRunner() {}
PyRunner::~PyRunner() {}
bool PyRunner::load(const ModuleProps&) { errorLog << "Not emplemented"; return false; }
PyRunner::FuncResult PyRunner::run(const QString&, const Args&, const ResultSpec&) { return {}; }

#else

// Includes Python.h, should be first:
#include "PyUtils.h"
#include "PyRunner.h"
#include "PyModuleGlobal.h"
#include "PyModuleSchema.h"
#include "Units.h"
#include "../math/BeamCalculator.h"

#include <QDebug>
#include <QDir>
#include <QRegularExpression>

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
        // To make the dev env to be close to the users' one
        // place python312.zip and python312.dll to the bin dir (nearby the rezonator.exe)
        //homeDir = QDir(qApp->applicationDirPath() + "/../vcpkg_installed/x64-windows/tools/python3");
#endif
    }

    std::wstring homePath = homeDir.absolutePath().toStdWString();
    // +1 byte for zero-terminator. PyMem_RawCalloc initializes memory to zeros
    config.home = (wchar_t*)PyMem_RawCalloc(homePath.size() + 1, sizeof(wchar_t));
    if (!config.home) {
        qCritical() << "Unable to allocate memory for PYTHONHOME";
        initError = "Unable to allocate memory for PYTHONHOME";
        errorLog << initError;
        PyConfig_Clear(&config);
        return;
    }
    memcpy(config.home, homePath.c_str(), homePath.size() * sizeof(wchar_t));

    ADD_MODULE(PyModule::Global)
    ADD_MODULE(PyModule::Schema)

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
    INIT_MODULE(PyModule::Global)
    INIT_MODULE(PyModule::Schema)

    PyConfig_Clear(&config);
    inited = true;
}

PyRunner::~PyRunner()
{
    for (int i = _refs.size()-1; i>= 0; i--)
    {
        const auto &r = _refs.at(i);
        if (Py_REFCNT(r.ref) <= 0)
            qCritical() << "Try to decref object which is already freed: PyRunner :" << r.id << Py_REFCNT(r.ref);
        else
            Py_DECREF(r.ref);
    }

    PyGlobal::schema = nullptr;
    PyGlobal::printFunc = nullptr;
}

struct TmpRefs
{
    TmpRefs(const char *func): func(func) {}
    ~TmpRefs()
    {
        for (int i = refs.size()-1; i>= 0; i--)
        {
            const auto &r = refs.at(i);
            if (Py_REFCNT(r.ref) <= 0)
                qCritical() << "Try to decref object which is already freed:" << func << ':' << r.id << Py_REFCNT(r.ref);
            else
                Py_DECREF(r.ref);
        }
    }
    
    void operator << (const PyRunner::TmpRef &ref)
    {
        //qDebug() << "Add tmp ref:" << func << ':' << ref.id << Py_REFCNT(ref.ref);
        refs << ref;
    }
    
    void operator >> (PyObject *ref)
    {
        for (int i = 0; i < refs.size(); i++)
            if (refs.at(i).ref == ref)
            {
                //qDebug() << "Rem tmp ref:" << func << ':' << refs.at(i).id << Py_REFCNT(ref);
                refs.removeAt(i);
                break;
            }
    }

    const char *func;
    QVector<PyRunner::TmpRef> refs;
};

#define TMP_REF(ref) TmpRef{#ref, ref}

bool PyRunner::load(const ModuleProps &props)
{
    if (!errorLog.isEmpty()) return false;
    
    if (moduleName.isEmpty()) {
        handleError("Custom module name is not provided");
        return false;
    }

    PyGlobal::schema = schema;
    PyGlobal::printFunc = printFunc ? printFunc : [](const QString &s){ qDebug() << s; };
    
    auto bCode = code.toUtf8();
    auto bModuleName = moduleName.toUtf8();
    auto szModuleName = bModuleName.constData();
    
    TmpRefs refs("PyRunner::load");
    
    auto pOldModule = PyImport_ImportModule(szModuleName);
    if (!pOldModule) {
        if (PyErr_ExceptionMatches(PyExc_ModuleNotFoundError)) {
            // This is expected, clean the exception
            refs << TmpRef{"ModuleNotFoundError", PyErr_GetRaisedException()};
        } else {
            handleError("Failed to find existing custom module");
            return false;
        }
    } else {
        refs << TMP_REF(pOldModule);
        // Python doesn't replace the whole module when reimports its code
        // It replaces only attributes that exist in the new code
        // If the new code doesn't contain docstring, the old doc is returned
        if (PyObject_SetAttrString(pOldModule, "__doc__", Py_None) < 0) {
            handleError("Failed to clean existing custom module");
            return false;
        }
    }
    
    auto pCompiled = Py_CompileString(bCode.constData(), szModuleName, Py_file_input);
    if (!pCompiled) {
        handleError("Failed to compile custom code");
        return false;
    }
    refs << TMP_REF(pCompiled);
    
    auto pModule = PyImport_ExecCodeModule(szModuleName, pCompiled);
    if (!pModule) {
        handleError("Failed to execute custom module");
        return false;
    }
    refs << TMP_REF(pModule);
    
    for (auto it = props.consts.cbegin(); it != props.consts.cend(); it++) {
        if (PyModule_AddIntConstant(pModule, it.key(), it.value()) < 0) {
            handleError(QString("Failed to add module constant %1=%2").arg(it.key()).arg(it.value()));
            return false;
        }
    }
    
    auto pDoc = PyObject_GetAttrString(pModule, "__doc__");
    if (pDoc) {
        refs << TMP_REF(pDoc);

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
        _refs << TmpRef{funcName, pFunc};

        if (!PyCallable_Check(pFunc)) {
            handleError("The object is not callable: " + funcName);
            return false;
        }
        _funcRefs.insert(funcName, pFunc);
    }

    for (const QString &funcName : std::as_const(funcNamesOptional)) {
        auto bFuncName = funcName.toUtf8();
    
        auto pFunc = PyObject_GetAttrString(pModule, bFuncName.constData());
        if (!pFunc) {
            PyErr_Clear();
            continue;
        }
        _refs << TmpRef{funcName, pFunc};

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
    
    TmpRefs refs("PyRunner::run");

    auto pArgs = PyTuple_New(args.size());
    if (!pArgs) {
        handleError("Failed to allocate agrs", funcName);
        return {};
    }
    refs << TMP_REF(pArgs);
   
    for (int i = 0; i < args.size(); i++) {
        auto argType = args.at(i).first;
        auto argValue = args.at(i).second;
        PyObject *pArg = nullptr;
        switch (argType) {
        case atElement:
            pArg = PyClass::Element::make((Element*)(argValue.value<void*>()));
            break;
        case atRoundTrip:
            pArg = PyClass::RoundTrip::make((BeamCalculator*)(argValue.value<void*>()), false);
            break;
        case atInt:
            pArg = PyLong_FromLong(argValue.toInt());
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

    Records result;
    
    auto pResult = PyObject_CallObject(pFunc, pArgs);
    if (!pResult) {
        handleError("Failed to call function", funcName);
        return {};
    }
    if (Py_IsNone(pResult)) {
        // None is a valid result even for functions having resultSpec.
        // It should show that the function is not applicable for particular conditions
        // but it doesn't fail, just silently skip
        return result;
    }
    refs << TMP_REF(pResult);
    
    if (resultSpec.isEmpty()) {
        qWarning() << "Function" << funcName << "returns something but "
            "it's not specified how to parse it, probably the function is not expected to return";
        return result;
    }
    
    QVector<PyObject*> resultItems;
    
    if (PyDict_Check(pResult)) {
        resultItems << pResult;
    } else if (PyList_Check(pResult)) {
        auto resultCount = PyList_Size(pResult);
        for (auto i = 0; i < resultCount; i++) {
            auto pItem = PyList_GetItem(pResult, i);
            if (!pItem) {
                handleError(QString("Unable to get result[%1]").arg(i), funcName);
                return {};
            }
            resultItems << pItem;
        }
    } else {
        handleError("Bad result type, dict or list expected", funcName);
        return {};
    }
    
    for (int i = 0; i < resultItems.size(); i++) {
        auto pItem = resultItems.at(i);
        if (!PyDict_Check(pItem)) {
            handleError(QString("result[%1]: bad type, dict expected").arg(i), funcName);
            return {};
        }
        Record rec;
        for (auto f = resultSpec.cbegin(); f != resultSpec.cend(); f++) {
            auto keyBytes = f.key().toUtf8();
            const char *k = keyBytes.constData();
            #define CHECK_E(cond, msg) \
                if (!cond) { \
                    handleError(QString("result[%1]['%2']: %3").arg(i).arg(k, msg), funcName); \
                    return {}; \
                }
            auto pKey = PyUnicode_FromString(k);
            CHECK_E(pKey, "failed to convert key to PyObject");
            refs << TMP_REF(pKey);
            PyObject *pField = nullptr;
            if (PyDict_Contains(pItem, pKey)) {
                pField = PyDict_GetItemString(pItem, k);
                CHECK_E(pField, "failed to get field");
                if (pField == Py_None)
                    pField = nullptr;
            }
            if (!pField) {
                if (f.value() == ftStringOptional) {
                    rec[k] = QString();
                    continue;
                }
                CHECK_E(false, "field not found");
            }
            switch (f.value()) {
            case ftNumber:
                if (PyFloat_Check(pField))
                    rec[k] = PyFloat_AsDouble(pField);
                else if (PyLong_Check(pField))
                    rec[k] = double(PyLong_AsLong(pField));
                else CHECK_E(false, "number expected");
                break;
            case ftNumberArray: {
                CHECK_E(PyList_Check(pField), "list expected");
                auto listSize = PyList_Size(pField);
                QVector<double> numbers;
                numbers.reserve(listSize);
                for (Py_ssize_t j = 0; j < listSize; j++) {
                    auto pItem = PyList_GetItem(pField, j);
                    CHECK_E(pItem, QString("failed to get list item %1").arg(j));
                    if (PyFloat_Check(pItem))
                        numbers.append(PyFloat_AsDouble(pItem));
                    else if (PyLong_Check(pItem))
                        numbers.append(double(PyLong_AsLong(pItem)));
                    else CHECK_E(false, QString("list item %1 is not a number").arg(j));
                }
                rec[k] = QVariant::fromValue(numbers);
                break;
            }
            case ftString:
            case ftStringOptional:
                CHECK_E(PyUnicode_Check(pField), "string expected");
                rec[k] = QString::fromUtf8(PyUnicode_AsUTF8(pField));
                break;
            case ftUnitDim: {
                CHECK_E(PyLong_Check(pField), "dimension expected");
                auto dims = Z::Dims::dims();
                int dim = PyLong_AsLong(pField);
                CHECK_E((dim >= 0 && dim < dims.size()), "bad dimension");
                rec[k] = QVariant::fromValue(dims.at(dim));
                break;
            }
            }
            #undef CHECK_E
        }
        result << rec;
    }

    return result;
}

void PyRunner::handleError(const QString& msg, const QString &funcName)
{
    TmpRefs refs("PyRunner::handleError");

    if (funcName.isEmpty())
        errorLog << msg;
    else
        errorLog << msg + QString(" (function: %1)").arg(funcName);
    
    auto exc = PyErr_GetRaisedException();
    if (!exc) {
        return;
    }
    refs << TMP_REF(exc);
        
    auto traceback = PyImport_ImportModule("traceback");
    if (!traceback) {
        errorLog << "Unable to import traceback module";
        PyErr_Print();
        return;
    }
    refs << TMP_REF(traceback);
    
    auto format = PyObject_GetAttrString(traceback, "format_exception");
    if (!format) {
        errorLog << "Unable to import traceback.format_exception";
        PyErr_Print();
        return;
    }
    refs << TMP_REF(format);
    
    auto args = PyTuple_New(1);
    if (!args) {
        errorLog << "Failed to allocate args for format_exception";
        PyErr_Print();
        return;
    }
    refs << TMP_REF(args);
                    
    if (PyTuple_SetItem(args, 0, exc) < 0) {
        errorLog << "Failed to init args for format_exception";
        PyErr_Print();
        return;
    }
    refs >> exc; // args now handles the ref

    auto items = PyObject_CallObject(format, args);
    if (!items) {
        errorLog << "Failed to call format_exception";
        PyErr_Print();
        return;
    }
    refs << TMP_REF(items);

    auto iter = PyObject_GetIter(items);
    if (!iter) {
        errorLog << "Failed to get iterator for exception lines";
        PyErr_Print();
        return;
    }
    refs << TMP_REF(iter);

    auto item = PyIter_Next(iter);
    while (item) {
        refs << TMP_REF(item);
        QString line = QString::fromUtf8(PyUnicode_AsUTF8(item));
        while (line.endsWith('\n') || line.endsWith('\r'))
            line.truncate(line.length()-1);
        if (!line.isEmpty()) {
            errorLog << line;
            if (errorLine == 0) {
                static QRegularExpression r(R"(File \"(.+)\",\s+line\s+(\d+))");
                if (auto m = r.match(line); m.hasMatch() && m.captured(1) == moduleName)
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
