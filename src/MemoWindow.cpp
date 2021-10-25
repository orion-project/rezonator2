#include "MemoWindow.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"

#include <QAction>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QRegularExpression>
#include <QSpinBox>
#include <QTextEdit>
#include <QTextList>
#include <QTextTable>
#include <QToolButton>
#include <QUuid>

//------------------------------------------------------------------------------
//                                MemoTextEdit
//------------------------------------------------------------------------------

class MemoTextEdit : public QTextEdit
{
public:
    MemoTextEdit(SchemaMemo* memo) : QTextEdit(), _memo(memo)
    {
        if (!memo->images.isEmpty())
        {
            auto it = memo->images.constBegin();
            while (it != memo->images.constEnd())
            {
                document()->addResource(QTextDocument::ImageResource, it.key(), it.value());
                it++;
            }
        }
        if (!memo->text.isEmpty())
            setHtml(memo->text);
    }

    bool canInsertFromMimeData(const QMimeData* source) const override
    {
        return source->hasImage() || source->hasUrls() || QTextEdit::canInsertFromMimeData(source);
    }

    void insertFromMimeData(const QMimeData* source) override
    {
        if (source->hasImage())
        {
            auto id = QUuid::createUuid().toString(QUuid::Id128);
            auto img = qvariant_cast<QImage>(source->imageData());
            document()->addResource(QTextDocument::ImageResource, id, img);
            textCursor().insertImage(id);
            _memo->images[id] = img;
            return;
        }
        QTextEdit::insertFromMimeData(source);
    }

private:
    SchemaMemo *_memo;
};

//------------------------------------------------------------------------------
//                                MemoWindow
//------------------------------------------------------------------------------

static QWidget* makeEmptySeparator(int width = 6)
{
    auto w = new QWidget;
    w->setFixedWidth(width);
    return w;
}

static QPixmap makeColorIcon(QMap<QString, QPixmap>& icons, const QString& baseIcon, const QBrush &b)
{
    auto n = b.style() == Qt::NoBrush ? QStringLiteral("empty") : b.color().name();
    if (icons.contains(n))
        return icons[n];
    auto pixmap = QIcon(baseIcon).pixmap(24, 24);
    QPainter p(&pixmap);
    p.setPen(b.color());
    p.setBrush(b);
    p.drawRect(0, 18, 23, 5);
    icons[n] = pixmap;
    return pixmap;
}

static QPixmap makeTextColorIcon(const QBrush &b)
{
    static QMap<QString, QPixmap> icons;
    return makeColorIcon(icons, QStringLiteral(":/toolbar/text_color"), b);
}

static QPixmap makeBackColorIcon(const QBrush &b)
{
    static QMap<QString, QPixmap> icons;
    return makeColorIcon(icons, QStringLiteral(":/toolbar/back_color"), b);
}

MemoWindow* MemoWindow::_instance = nullptr;

MemoWindow* MemoWindow::create(Schema* owner)
{
    if (!_instance)
        _instance = new MemoWindow(owner);
    return _instance;
}

MemoWindow::MemoWindow(Schema* owner) : SchemaMdiChild(owner)
{
    setTitleAndIcon(tr("Memo"), ":/toolbar/notepad");

    if (!schema()->memo)
        schema()->memo = new SchemaMemo;
    schema()->memo->editor = this;

    _editor = new MemoTextEdit(schema()->memo);
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
    _editor->setFocus();

    fontChanged(_editor->font());
    alignmentChanged(_editor->alignment());

    QTextCursor cursor = _editor->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    auto fmt = cursor.charFormat();
    textColorChanged(fmt.foreground());
    backColorChanged(fmt.background());
}

MemoWindow::~MemoWindow()
{
    _instance = nullptr;
}

void MemoWindow::createActions()
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

    _actionTextColor = new QAction(makeTextColorIcon(Qt::black), tr("Text Color..."), this);
    connect(_actionTextColor, &QAction::triggered, this, &MemoWindow::textColor);
    _actionBackColor = new QAction(makeBackColorIcon(Qt::white), tr("Back Color..."), this);
    connect(_actionBackColor, &QAction::triggered, this, &MemoWindow::backColor);

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

    _actionExportPdf = A_(tr("Export as PDF..."), this, SLOT(exportPdf()));

    #undef A_
}

void MemoWindow::createMenuBar()
{
    _alignMenu = Ori::Gui::menu(tr("Alignment"), this, {
        _actionAlignLeft, _actionAlignCenter, _actionAlignRight, _actionAlignJustify
    });

    _windowMenu = Ori::Gui::menu(tr("Memo"), this, {
        _actionBold, _actionItalic, _actionUnderline, _actionStrikeout, nullptr,
        _actionIndent, _actionUnindent, nullptr,
        _actionTextColor, _actionBackColor,
        _alignMenu, nullptr,
        _actionInsertTable, nullptr,
        _actionExportPdf,
    });
}

void MemoWindow::createToolBar()
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
        _actionIndent, _actionUnindent, nullptr, _actionTextColor, _actionBackColor, _alignButton,
    });
}

void MemoWindow::closeEvent(class QCloseEvent* e)
{
    if (_editor->document()->isEmpty())
    {
        delete schema()->memo;
        schema()->memo = nullptr;
    }
    else
        saveMemo();

    SchemaMdiChild::closeEvent(e);
}

void MemoWindow::saveMemo()
{
    auto memo = schema()->memo;
    memo->text = _editor->toHtml();

    QStringList unusedImages;
    auto it = memo->images.constBegin();
    while (it != memo->images.constEnd())
    {
        QRegularExpression expr(QString("<img.*src\\s*=\\s*\"%1\".*\\/>").arg(it.key()));
        if (!expr.match(memo->text).hasMatch())
            unusedImages << it.key();
        it++;
    }
    foreach (const auto& id, unusedImages)
        memo->images.remove(id);

    _editor->document()->setModified(false);
}

void MemoWindow::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(_actionBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormat(fmt);
}

void MemoWindow::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(_actionUnderline->isChecked());
    mergeFormat(fmt);
}

void MemoWindow::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(_actionItalic->isChecked());
    mergeFormat(fmt);
}

void MemoWindow::textStrikeout()
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut(_actionStrikeout->isChecked());
    mergeFormat(fmt);
}

void MemoWindow::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormat(fmt);
}

void MemoWindow::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormat(fmt);
    }
}

void MemoWindow::textColor()
{
    QColor color = QColorDialog::getColor(_editor->textColor(), this);
    if (!color.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(color);
    mergeFormat(fmt);
    textColorChanged(color);
}

void MemoWindow::backColor()
{
    QColor color = QColorDialog::getColor(_editor->textBackgroundColor(), this);
    if (!color.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setBackground(color);
    mergeFormat(fmt);
    backColorChanged(color);
}

void MemoWindow::mergeFormat(const QTextCharFormat &format)
{
    QTextCursor cursor = _editor->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    _editor->mergeCurrentCharFormat(format);
}

void MemoWindow::textAlign(QAction *a)
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

void MemoWindow::indent()
{
    modifyIndentation(1);
}

void MemoWindow::unindent()
{
    modifyIndentation(-1);
}

void MemoWindow::modifyIndentation(int amount)
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


void MemoWindow::cursorPositionChanged()
{
    alignmentChanged(_editor->alignment());
}

void MemoWindow::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    textColorChanged(format.foreground());
    backColorChanged(format.background());
}

void MemoWindow::fontChanged(const QFont &f)
{
    _comboFont->setCurrentIndex(_comboFont->findText(QFontInfo(f).family()));
    _comboSize->setCurrentIndex(_comboSize->findText(QString::number(f.pointSize())));
    _actionBold->setChecked(f.bold());
    _actionItalic->setChecked(f.italic());
    _actionUnderline->setChecked(f.underline());
    _actionStrikeout->setChecked(f.strikeOut());
}

void MemoWindow::textColorChanged(const QBrush &b)
{
    _actionTextColor->setIcon(makeTextColorIcon(b));
}

void MemoWindow::backColorChanged(const QBrush &b)
{
    _actionBackColor->setIcon(makeBackColorIcon(b));
}

void MemoWindow::alignmentChanged(Qt::Alignment a)
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

void MemoWindow::insertTable()
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

bool MemoWindow::canUndo() { return _actionUndo->isEnabled(); }
bool MemoWindow::canRedo() { return _actionRedo->isEnabled(); }
bool MemoWindow::canCut() { return _actionCut->isEnabled(); }
bool MemoWindow::canCopy() { return _actionCopy->isEnabled(); }
bool MemoWindow::canPaste() { return _editor->canPaste(); }
void MemoWindow::undo() { _actionUndo->trigger(); }
void MemoWindow::redo() { _actionRedo->trigger(); }
void MemoWindow::cut() { _actionCut->trigger(); }
void MemoWindow::copy() { _actionCopy->trigger(); }
void MemoWindow::paste() { _actionPaste->trigger(); }
void MemoWindow::selectAll() { _editor->selectAll(); }

void MemoWindow::markModified(bool m)
{
    if (m)
        schema()->events().raise(SchemaEvents::Changed, "memo modified");
}

void MemoWindow::sendToPrinter()
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

void MemoWindow::printPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, _editor, &QTextEdit::print);
    preview.exec();
}

QSize MemoWindow::sizeHint() const
{
    auto sz = SchemaMdiChild::sizeHint();
    // make a bit wider to be sure that all toolbuttons are visible
    sz.setWidth(1.1 * sz.width());
    return sz;
}

void MemoWindow::exportPdf()
{
    QString fileName = Ori::Dlg::getSaveFileName(
        tr("Export memo as PDF"), tr("PDF documents (*.pdf);;All files (*.*)"), "pdf");
    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    printer.setPageSize(QPageSize(QPageSize::A4));
#else
    printer.setPaperSize(QPrinter::A4);
#endif
    printer.setOutputFileName(fileName);

    _editor->document()->print(&printer);
}
