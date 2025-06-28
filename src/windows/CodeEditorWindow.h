#ifndef CODE_EDITOR_WINDOW_H
#define CODE_EDITOR_WINDOW_H

#include "../windows/SchemaWindows.h"

class QPlainTextEdit;

namespace Ori::Widgets {
class CodeEditor;
}

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
    
    void clearLog();
    
protected:
    Ori::Widgets::CodeEditor* _editor;
    QString _customTitle;
    QString _defaultTitle;

    void logInfo(const QString &msg, bool scrollToEnd = true);
    void logError(const QString &msg, bool scrollToEnd = true);
    void logError(const QStringList &log, int errorLine);
    void logScrollToEnd();
    
    virtual void runCode();

    void updateWindowTitle();
    
private:
    QPlainTextEdit* _log;
    QAction *_actnRun, *_actnClearLog;
    QMenu* _windowMenu;
    bool _isChanging = false;

    void createActions();
    void createMenuBar();
    void createToolBar();
    
    void run();

    void markModified(bool m);
};

#endif // CODE_EDITOR_WINDOW_H
