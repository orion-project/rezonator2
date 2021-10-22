#include "NoteWindow.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"

#include <QAction>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QMenu>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QTextEdit>
#include <QTextList>
#include <QTextTable>
#include <QToolButton>
#include <QSpinBox>

//------------------------------------------------------------------------------
//                                NoteWindow
//------------------------------------------------------------------------------

NoteWindow* NoteWindow::_instance = nullptr;

NoteWindow* NoteWindow::create(Schema* owner)
{
    if (!_instance)
        _instance = new NoteWindow(owner);
    return _instance;
}

NoteWindow::NoteWindow(Schema* owner) : SchemaMdiChild(owner)
{
    setTitleAndIcon(tr("Notes"), ":/toolbar/notepad");

    _schemaChanged = schema()->modified();

    _editor = new QTextEdit;
    setContent(_editor);

    _comboFont = new QFontComboBox;
    _comboFont->setMaxVisibleItems(16);
    connect(_comboFont, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));

    _comboSize = new QComboBox;
    _comboSize->setEditable(true);
    _comboSize->setMaxVisibleItems(24);
    QList<int> fontSizes = QFontDatabase::standardSizes();
    foreach(int size, fontSizes) _comboSize->addItem(QString::number(size));
    connect(_comboSize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
    _comboSize->setCurrentIndex(_comboSize->findText(QString::number(QApplication::font().pointSize())));

    createActions();
    createMenuBar();
    createToolBar();

    connect(_editor, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    connect(_editor, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(_editor, SIGNAL(copyAvailable(bool)), _actionCut, SLOT(setEnabled(bool)));
    connect(_editor, SIGNAL(copyAvailable(bool)), _actionCopy, SLOT(setEnabled(bool)));
    connect(_editor->document(), SIGNAL(modificationChanged(bool)), this, SLOT(markModified(bool)));
    connect(_editor->document(), SIGNAL(undoAvailable(bool)), _actionUndo, SLOT(setEnabled(bool)));
    connect(_editor->document(), SIGNAL(redoAvailable(bool)), _actionRedo, SLOT(setEnabled(bool)));

    // initial state
    auto f = _editor->font();
    f.setFamily("Arial");
    f.setPointSize(12);
    _editor->setFont(f);
    _actionCut->setEnabled(false);
    _actionCopy->setEnabled(false);
    _actionUndo->setEnabled(false);
    _actionRedo->setEnabled(false);
    fontChanged(_editor->font());
    colorChanged(_editor->textColor());
    alignmentChanged(_editor->alignment());
    _editor->setFocus();
}

NoteWindow::~NoteWindow()
{
    _instance = nullptr;
}

void NoteWindow::createActions()
{
    #define A_ Ori::Gui::action

    _actionUndo = A_(tr("Undo"), _editor, SLOT(undo()), ":/toolbar/undo");
    _actionRedo = A_(tr("Redo"), _editor, SLOT(redo()), ":/toolbar/redo");
    _actionCut = A_(tr("Cut"), _editor, SLOT(cut()), ":/toolbar/cut");
    _actionCopy = A_(tr("Copy"), _editor, SLOT(copy()), ":/toolbar/copy");
    _actionPaste = A_(tr("Paste"), _editor, SLOT(paste()), ":/toolbar/paste");

    _actionBold = A_(tr("Bold"), this, SLOT(textBold()), ":/toolbar/bold", QKeySequence::Bold);
    _actionItalic = A_(tr("Italic"), this, SLOT(textItalic()), ":/toolbar/italic", QKeySequence::Italic);
    _actionUnderline = A_(tr("Underline"), this, SLOT(textUnderline()), ":/toolbar/underline", QKeySequence::Underline);
    _actionStrikeout = A_(tr("Strikeout"), this, SLOT(textStrikeout()), ":/toolbar/strikeout");
    _actionBold->setCheckable(true);
    _actionItalic->setCheckable(true);
    _actionUnderline->setCheckable(true);
    _actionStrikeout->setCheckable(true);

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    _actionTextColor = new QAction(pix, tr("Color..."), this);
    connect(_actionTextColor, &QAction::triggered, this, &NoteWindow::textColor);

    _actionsAlignment = new QActionGroup(this);
    connect(_actionsAlignment, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));
    _actionAlignLeft = new QAction(QIcon(":/toolbar/align_left"), tr("Left"), _actionsAlignment);
    _actionAlignCenter = new QAction(QIcon(":/toolbar/align_center"), tr("Center"), _actionsAlignment);
    _actionAlignRight = new QAction(QIcon(":/toolbar/align_right"), tr("Right"), _actionsAlignment);
    _actionAlignJustify = new QAction(QIcon(":/toolbar/align_just"), tr("Justify"), _actionsAlignment);
    _actionAlignLeft->setCheckable(true);
    _actionAlignCenter->setCheckable(true);
    _actionAlignRight->setCheckable(true);
    _actionAlignJustify->setCheckable(true);

    _actionIndent = A_(tr("Indent"), this, SLOT(indent()), ":/toolbar/indent_inc");
    _actionUnindent = A_(tr("Unindent"), this, SLOT(unindent()), ":/toolbar/indent_dec");

    _actionInsertTable = A_(tr("Insert Table..."), this, SLOT(insertTable()));

    #undef A_
}

void NoteWindow::createMenuBar()
{
    _alignMenu = Ori::Gui::menu(tr("Alignment"), this, {
        _actionAlignLeft, _actionAlignCenter, _actionAlignRight, _actionAlignJustify
    });

    _windowMenu = Ori::Gui::menu(tr("Notes"), this, {
        _actionBold, _actionItalic, _actionUnderline, _actionStrikeout, nullptr,
        _actionIndent, _actionUnindent, nullptr,
        _actionTextColor,
        _alignMenu, nullptr,
        _actionInsertTable,
    });
}

static QWidget* makeEmptySeparator(int width = 6)
{
    auto w = new QWidget;
    w->setFixedWidth(width);
    return w;
}

void NoteWindow::createToolBar()
{
    _alignButton = new QToolButton;
    _alignButton->setPopupMode(QToolButton::InstantPopup);
    _alignButton->setMenu(_alignMenu);


    populateToolbar({
        _actionUndo, _actionRedo,
        makeEmptySeparator(),
        _comboFont, makeEmptySeparator(3), _comboSize,
        makeEmptySeparator(),
        _actionCut, _actionCopy, _actionPaste, nullptr,
        _actionBold, _actionItalic, _actionUnderline, _actionStrikeout, nullptr,
        _actionIndent, _actionUnindent, nullptr, _actionTextColor, _alignButton,
    });
}

void NoteWindow::closeEvent(QCloseEvent *e)
{
    // TODO:
}

void NoteWindow::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(_actionBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormat(fmt);
}

void NoteWindow::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(_actionUnderline->isChecked());
    mergeFormat(fmt);
}

void NoteWindow::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(_actionItalic->isChecked());
    mergeFormat(fmt);
}

void NoteWindow::textStrikeout()
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut(_actionStrikeout->isChecked());
    mergeFormat(fmt);
}

void NoteWindow::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormat(fmt);
}

void NoteWindow::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormat(fmt);
    }
}

void NoteWindow::textColor()
{
    QColor color = QColorDialog::getColor(_editor->textColor(), this);
    if (!color.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(color);
    mergeFormat(fmt);
    colorChanged(color);
}

void NoteWindow::mergeFormat(const QTextCharFormat &format)
{
    QTextCursor cursor = _editor->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    _editor->mergeCurrentCharFormat(format);
}

void NoteWindow::textAlign(QAction *a)
{
    if (a == _actionAlignLeft) {
        _editor->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
        _alignButton->setIcon(_actionAlignLeft->icon());
    } else if (a == _actionAlignCenter) {
        _editor->setAlignment(Qt::AlignHCenter);
        _alignButton->setIcon(_actionAlignCenter->icon());
    } else if (a == _actionAlignRight) {
        _editor->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
        _alignButton->setIcon(_actionAlignRight->icon());
    } else if (a == _actionAlignJustify) {
        _editor->setAlignment(Qt::AlignJustify);
        _alignButton->setIcon(_actionAlignJustify->icon());
    }
}

void NoteWindow::indent()
{
    modifyIndentation(1);
}

void NoteWindow::unindent()
{
    modifyIndentation(-1);
}

void NoteWindow::modifyIndentation(int amount)
{
    QTextCursor cursor = _editor->textCursor();
    cursor.beginEditBlock();
    if (cursor.currentList()) {
        QTextListFormat listFmt = cursor.currentList()->format();
        // See whether the line above is the list we want to move this item into,
        // or whether we need a new list.
        QTextCursor above(cursor);
        above.movePosition(QTextCursor::Up);
        if (above.currentList() && listFmt.indent() + amount == above.currentList()->format().indent()) {
            above.currentList()->add(cursor.block());
        } else {
            listFmt.setIndent(listFmt.indent() + amount);
            cursor.createList(listFmt);
        }
    } else {
        QTextBlockFormat blockFmt = cursor.blockFormat();
        blockFmt.setIndent(blockFmt.indent() + amount);
        cursor.setBlockFormat(blockFmt);
    }
    cursor.endEditBlock();
}


void NoteWindow::cursorPositionChanged()
{
    alignmentChanged(_editor->alignment());
}

void NoteWindow::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void NoteWindow::fontChanged(const QFont &f)
{
    _comboFont->setCurrentIndex(_comboFont->findText(QFontInfo(f).family()));
    _comboSize->setCurrentIndex(_comboSize->findText(QString::number(f.pointSize())));
    _actionBold->setChecked(f.bold());
    _actionItalic->setChecked(f.italic());
    _actionUnderline->setChecked(f.underline());
    _actionStrikeout->setChecked(f.strikeOut());
}

void NoteWindow::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    _actionTextColor->setIcon(pix);
}

void NoteWindow::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft) {
        _actionAlignLeft->setChecked(true);
        _alignButton->setIcon(_actionAlignLeft->icon());
    } else if (a & Qt::AlignHCenter) {
        _actionAlignCenter->setChecked(true);
        _alignButton->setIcon(_actionAlignCenter->icon());
    } else if (a & Qt::AlignRight) {
        _actionAlignRight->setChecked(true);
        _alignButton->setIcon(_actionAlignRight->icon());
    } else if (a & Qt::AlignJustify) {
        _actionAlignJustify->setChecked(true);
        _alignButton->setIcon(_actionAlignJustify->icon());
    }
}

void NoteWindow::insertTable()
{
    auto rows = Ori::Gui::spinBox(1, 1000, 4);
    auto cols = Ori::Gui::spinBox(1, 100, 4);
    auto w = new QWidget;
    auto layout = new QFormLayout(w);
    layout->addRow(tr("Row count"), rows);
    layout->addRow(tr("Col count"), cols);
    auto dlg = Ori::Dlg::Dialog(w, true)
            .withTitle(tr("Insert Table"))
            .withContentToButtonsSpacingFactor(2);
    if (dlg.exec())
    {
        QTextTableFormat fmt;
        fmt.setCellPadding(5);
        fmt.setCellSpacing(0);
        fmt.setBorderBrush(Qt::black);
        fmt.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
        fmt.setBorderCollapse(true);
        _editor->textCursor().insertTable(rows->value(), cols->value(), fmt);
    }
}

bool NoteWindow::canUndo() { return _actionUndo->isEnabled(); }
bool NoteWindow::canRedo() { return _actionRedo->isEnabled(); }
bool NoteWindow::canCut() { return _actionCut->isEnabled(); }
bool NoteWindow::canCopy() { return _actionCopy->isEnabled(); }
bool NoteWindow::canPaste() { return _editor->canPaste(); }
void NoteWindow::undo() { _editor->undo(); }
void NoteWindow::redo() { _editor->redo(); }
void NoteWindow::cut() { _editor->cut(); }
void NoteWindow::copy() { _editor->copy(); }
void NoteWindow::paste() { _editor->paste(); }
void NoteWindow::selectAll() { _editor->selectAll(); }

void NoteWindow::markModified(bool ok)
{
    qDebug() << "document modified" << ok;
}

void NoteWindow::sendToPrinter()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (_editor->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Schema Notes"));
    if (dlg->exec() == QDialog::Accepted)
        _editor->print(&printer);
    delete dlg;
}

void NoteWindow::printPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, _editor, &QTextEdit::print);
    preview.exec();
}
