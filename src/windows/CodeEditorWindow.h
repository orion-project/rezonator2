#ifndef CODE_EDITOR_WINDOW_H
#define CODE_EDITOR_WINDOW_H

#include "../windows/SchemaWindows.h"

class QPlainTextEdit;

namespace Ori::Widgets {
class CodeEditor;
}

/**
    Generic window for code editors without code running functionality.
    Intended only as a base class for function code editors
    @sa CustomCodeWindow, @sa CustomTableCodeWindow, @sa CustomPlotCodeWindow
*/
class CodeEditorWindow: public SchemaMdiChild, public IEditableWindow
{
    Q_OBJECT

public:
    CodeEditorWindow(Schema*, const QString &title = QString());
    
    QString code() const;
    void setCode(const QString &code);

    // inherits from IEditableWindow
    SupportedCommands supportedCommands() override {
        return EditCmd_Undo | EditCmd_Redo | EditCmd_Cut | EditCmd_Copy | EditCmd_Paste | EditCmd_SelectAll; }
    bool canUndo() override;
    bool canRedo() override;
    bool canCut() override;
    bool canCopy() override;
    bool canPaste() override;
    void undo() override;
    void redo() override;
    void cut() override;
    void copy() override;
    void paste() override;
    void selectAll() override;

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return { _windowMenu }; }
    
    // inherits from SchemaListener
    void schemaSaved(Schema*) override;
    
    void clearLog();
    
protected:
    Ori::Widgets::CodeEditor* _editor;
    QString _customTitle;
    QString _defaultTitle;

    void logError(const QStringList &log, int errorLine);
    void logInfo(const QString &msg);
    
    virtual void runCode();

    void updateWindowTitle();
    
private:
    QPlainTextEdit* _log;
    QAction *_actnRun, *_actnClearLog, *_actnSaveCustom;
    QMenu* _windowMenu;
    bool _isChanging = false;

    void createActions();
    void createMenuBar();
    void createToolBar();
    
    void run();

    void markModified(bool m);
    void saveToLibrary();
};

namespace CodeUtils
{
/// Returns a full path to the code template file.
QString codeTemplateFile(const QString &templateName);

/// Returns a content of the code template file
/// or empty string if that does not exists.
QString loadCodeTemplate(const QString &templateName);
}

#endif // CODE_EDITOR_WINDOW_H
