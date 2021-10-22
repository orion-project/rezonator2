#ifndef NOTE_WINDOW_H
#define NOTE_WINDOW_H

#include "SchemaWindows.h"

QT_BEGIN_NAMESPACE
class QActionGroup;
class QComboBox;
class QFontComboBox;
class QTextCharFormat;
class QTextEdit;
class QToolButton;
QT_END_NAMESPACE

class Schema;

class NoteWindow : public SchemaMdiChild,
                   public IEditableWindow,
                   public IPrintableWindow
{
    Q_OBJECT

public:
    static NoteWindow* create(Schema*);

    ~NoteWindow() override;

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return { _windowMenu }; }

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

    // inherits from IPrintableWindow
    void sendToPrinter() override;
    void printPreview() override;

protected:
    explicit NoteWindow(Schema *owner);

    void closeEvent(QCloseEvent *e) override;

private slots:
    void textBold();
    void textItalic();
    void textUnderline();
    void textStrikeout();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textColor();
    void textAlign(QAction *a);
    void cursorPositionChanged();
    void currentCharFormatChanged(const QTextCharFormat &format);
    void insertTable();
    void markModified(bool ok);
    void indent();
    void unindent();

private:
    static NoteWindow* _instance;

    QTextEdit *_editor;
    QMenu *_windowMenu, *_alignMenu;
    QFontComboBox *_comboFont;
    QComboBox *_comboSize;
    QActionGroup *_actionsAlignment;
    QToolButton *_alignButton;
    QAction *_actionBold, *_actionUnderline, *_actionItalic, *_actionStrikeout,
        *_actionTextColor, *_actionAlignLeft, *_actionAlignCenter, *_actionAlignRight,
        *_actionAlignJustify, *_actionUndo, *_actionRedo, *_actionCut, *_actionCopy, *_actionPaste,
        *_actionInsertTable, *_actionIndent, *_actionUnindent;
    bool _schemaChanged = false;

    void createActions();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();

    void mergeFormat(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);
    void modifyIndentation(int amount);
};

#endif // NOTE_WINDOW_H

