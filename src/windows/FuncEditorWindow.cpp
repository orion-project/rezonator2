#include "FuncEditorWindow.h"

#include "../app/Appearance.h"

#include "helpers/OriWidgets.h"
#include "helpers/OriDialogs.h"

#include <QAction>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QTemporaryFile>
#include <QTextStream>
#include <QToolButton>
#include <QVBoxLayout>

namespace FuncEditorWindowStorable
{
    SchemaWindow* createWindow(Schema* schema)
    {
        return FuncEditorWindow::create(schema);
    }
} // namespace FuncEditorWindowStorable

//------------------------------------------------------------------------------
//                              FuncEditorWindow
//------------------------------------------------------------------------------

FuncEditorWindow* FuncEditorWindow::create(Schema* owner)
{
    return new FuncEditorWindow(owner);
}

FuncEditorWindow::FuncEditorWindow(Schema *owner) : SchemaMdiChild(owner), _pythonProcess(nullptr)
{
    setTitleAndIcon(tr("Custom Function"), ":/toolbar/protocol");

    createActions();
    createMenuBar();
    createToolBar();
    createContent();
}

FuncEditorWindow::~FuncEditorWindow()
{
    cleanupTempFiles();
    
    if (_pythonProcess)
    {
        if (_pythonProcess->state() != QProcess::NotRunning)
        {
            _pythonProcess->terminate();
            _pythonProcess->waitForFinished(1000);
        }
        delete _pythonProcess;
    }
}

void FuncEditorWindow::createContent()
{
    _editor = new QPlainTextEdit;
    _editor->setFont(Z::Gui::CodeEditorFont().get());
    _editor->setPlaceholderText(tr("Enter function code here..."));
    
    _log = new QPlainTextEdit;
    _log->setFont(Z::Gui::CodeEditorFont().get());
    _log->setReadOnly(true);
    _log->setPlaceholderText(tr("Execution log will appear here..."));
    
    setContent(Ori::Gui::splitterV(_editor, _log, 200, 100));
}

void FuncEditorWindow::createActions()
{
    #define A_ Ori::Gui::action

    _actnCheck = A_(tr("Check"), this, &FuncEditorWindow::checkFunction, ":/toolbar/check", Qt::Key_F9);
        
    #undef A_
}

void FuncEditorWindow::createMenuBar()
{
    _windowMenu = Ori::Gui::menu(tr("Function"), this, { _actnCheck });
}

void FuncEditorWindow::createToolBar()
{
    populateToolbar({ Ori::Gui::textToolButton(_actnCheck) });
}

void FuncEditorWindow::checkFunction()
{
    // Clear the log
    _log->clear();
    _log->appendPlainText(tr("Running Python code..."));
    
    // Get the code from the editor
    QString code = _editor->toPlainText();
    if (code.trimmed().isEmpty())
    {
        _log->appendPlainText(tr("Error: No code to execute."));
        return;
    }
    
    // Create a temporary file for the Python script
    cleanupTempFiles();
    
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    if (!tempFile.open())
    {
        _log->appendPlainText(tr("Error: Could not create temporary file."));
        return;
    }
    
    _tempScriptPath = tempFile.fileName();
    
    // Write the code to the temporary file
    QTextStream out(&tempFile);
    out << code;
    tempFile.close();
    
    // Get the Python interpreter path
    QString pythonPath = getPythonInterpreterPath();
    if (pythonPath.isEmpty())
    {
        _log->appendPlainText(tr("Error: Python interpreter not found."));
        return;
    }
    
    // Create a new process if needed
    if (!_pythonProcess)
    {
        _pythonProcess = new QProcess(this);
        connect(_pythonProcess, &QProcess::readyReadStandardOutput, this, &FuncEditorWindow::handlePythonOutput);
        connect(_pythonProcess, &QProcess::readyReadStandardError, this, &FuncEditorWindow::handlePythonError);
        connect(_pythonProcess, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, &FuncEditorWindow::handlePythonFinished);
    }
    
    // Run the Python script
    _pythonProcess->start(pythonPath, QStringList() << _tempScriptPath);
}

void FuncEditorWindow::handlePythonOutput()
{
    if (_pythonProcess)
    {
        QByteArray output = _pythonProcess->readAllStandardOutput();
        _log->appendPlainText(QString::fromUtf8(output));
    }
}

void FuncEditorWindow::handlePythonError()
{
    if (_pythonProcess)
    {
        QByteArray error = _pythonProcess->readAllStandardError();
        _log->appendPlainText(QString::fromUtf8(error));
    }
}

void FuncEditorWindow::handlePythonFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit)
    {
        _log->appendPlainText(tr("Error: Python process crashed."));
    }
    else if (exitCode != 0)
    {
        _log->appendPlainText(tr("Error: Python process exited with code %1.").arg(exitCode));
    }
    else
    {
        _log->appendPlainText(tr("Python code executed successfully."));
    }
}

QString FuncEditorWindow::getPythonInterpreterPath() const
{
    return "C:/Program Files/Python313/python.exe";
}

void FuncEditorWindow::cleanupTempFiles()
{
    if (!_tempScriptPath.isEmpty())
    {
        QFile::remove(_tempScriptPath);
        _tempScriptPath.clear();
    }
}

bool FuncEditorWindow::storableRead(const QJsonObject& root, Z::Report*)
{
    _editor->setPlainText(root["code"].toString());
    return true;
}

bool FuncEditorWindow::storableWrite(QJsonObject& root, Z::Report*)
{
    root["code"] = _editor->toPlainText();
    return true;
}
