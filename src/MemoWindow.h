#ifndef MEMO_WINDOW_H
#define MEMO_WINDOW_H

#include "SchemaWindows.h"

QT_BEGIN_NAMESPACE
class QActionGroup;
class QComboBox;
class QFontComboBox;
class QTextCharFormat;
class QToolButton;
QT_END_NAMESPACE

class Schema;
class MemoTextEdit;

class MemoWindow : public SchemaMdiChild,
                   public IEditableWindow,
                   public IPrintableWindow,
                   public ISchemaMemoEditor
{
    Q_OBJECT

public:
    static MemoWindow* create(Schema*);

    ~MemoWindow() override;

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

    // inherits from ISchemaMemoEditor
    void saveMemo() override;

protected:
    explicit MemoWindow(Schema *owner);

    void closeEvent(class QCloseEvent*) override;
    QSize sizeHint() const override;

private slots:
    void textBold();
    void textItalic();
    void textUnderline();
    void textStrikeout();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textColor();
    void backColor();
    void textAlign(QAction *a);
    void cursorPositionChanged();
    void currentCharFormatChanged(const QTextCharFormat &format);
    void insertTable();
    void markModified(bool m);
    void indent();
    void unindent();

private:
    static MemoWindow* _instance;

    MemoTextEdit *_editor;
    QMenu *_windowMenu, *_alignMenu;
    QFontComboBox *_comboFont;
    QComboBox *_comboSize;
    QActionGroup *_actionsAlignment;
    QToolButton *_alignButton;
    QAction *_actionBold, *_actionUnderline, *_actionItalic, *_actionStrikeout,
        *_actionTextColor, *_actionBackColor, *_actionAlignLeft, *_actionAlignCenter, *_actionAlignRight,
        *_actionAlignJustify, *_actionUndo, *_actionRedo, *_actionCut, *_actionCopy, *_actionPaste,
        *_actionInsertTable, *_actionIndent, *_actionUnindent;

    void createActions();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();

    void mergeFormat(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void textColorChanged(const QBrush &b);
    void backColorChanged(const QBrush &b);
    void alignmentChanged(Qt::Alignment a);
    void modifyIndentation(int amount);
};

#endif // MEMO_WINDOW_H

