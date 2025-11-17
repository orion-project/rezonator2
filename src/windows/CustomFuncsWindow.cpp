#include "CustomFuncsWindow.h"

#include "../app/AppSettings.h"
#include "../app/Appearance.h"
#include "../app/CustomFuncsLib.h"
#include "../app/HelpSystem.h"
#include "../app/PersistentState.h"
#include "../core/Schema.h"
#include "../math/CustomFuncUtils.h"
#include "../widgets/Widgets.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriWindows.h"
#include "widgets/OriCodeEditor.h"
#include "widgets/OriPopupMessage.h"

#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QListWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QEvent>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QPointer>
#include <QPushButton>
#include <QSplitter>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

using namespace  Ori::Layouts;

#define STATE_KEY "custom_funcs"

namespace {

using CustomFuncsWindowPtr = QPointer<CustomFuncsWindow>;
Q_GLOBAL_STATIC(CustomFuncsWindowPtr, __instance);

bool hasFuncDef(const QString &code, const QString &funcName)
{
    static QHash<QString, QRegularExpression> funcDefs = {
        { CustomFuncUtils::funcNameCalculate(), QRegularExpression(QString("\\bdef\\s+%1\\s*\\(").arg(CustomFuncUtils::funcNameCalculate())) },
        { CustomFuncUtils::funcNameColumns(), QRegularExpression(QString("\\bdef\\s+%1\\s*\\(").arg(CustomFuncUtils::funcNameColumns())) },
        { CustomFuncUtils::funcNameFigure(), QRegularExpression(QString("\\bdef\\s+%1\\s*\\(").arg(CustomFuncUtils::funcNameFigure())) },
    };
    if (!funcDefs.contains(funcName))
        return false;
    return code.contains(funcDefs[funcName]);
}

QIcon getFileIcon(const QString &code)
{
    if (hasFuncDef(code, CustomFuncUtils::funcNameColumns()))
        return QIcon(":/toolbar/func_table");
    if (hasFuncDef(code, CustomFuncUtils::funcNameFigure()))
        return QIcon(":/toolbar/func_plot");
    return QIcon(":/toolbar/func_script");
}

} // namespace

void CustomFuncsWindow::showWindow(Schema *target)
{
    if (!(*__instance))
        *__instance = new CustomFuncsWindow(target);
    
    (*__instance)->show();
    (*__instance)->raise();
    (*__instance)->activateWindow();
}

CustomFuncsWindow::CustomFuncsWindow(Schema *target) : QWidget(), _target(target)
{
    setWindowTitle(tr("Custom Functions Library"));
    setAttribute(Qt::WA_DeleteOnClose);
    
    createActions();
    createToolbar();
    
    _filterEdit = Z::Gui::makeFilterEdit(tr("Filter functions..."), this, SLOT(applyFilter()));
    
    _fileList = new QListWidget;
    _fileList->setAlternatingRowColors(true);
    connect(_fileList, &QListWidget::currentItemChanged, this, &CustomFuncsWindow::onFileSelected);
    
    _editor = Z::Gui::makeCodeEditor();
    
    auto bannerText = new QLabel;
    bannerText->setWordWrap(true);
    bannerText->setText(tr(
        "Use this edit mode for quick fixes and corrections only. "
        "For development, add the function to a schema, modify and test it there, "
        "then use <b>Save to Library</b> to update the functions in the library."));
    
    auto btnSave = new QPushButton;
    btnSave->addAction(_actnSave);
    btnSave->setIcon(QIcon(":/toolbar/save"));
    btnSave->setText(tr("Save"));
    btnSave->setFixedWidth(120);
    connect(btnSave, &QPushButton::clicked, this, &CustomFuncsWindow::actionSave);
    
    auto btnCancel = new QPushButton;
    btnCancel->addAction(_actnCancel);
    btnCancel->setIcon(QIcon(":/toolbar/stop"));
    btnCancel->setText(tr("Cancel"));
    btnCancel->setFixedWidth(120);
    connect(btnCancel, &QPushButton::clicked, this, &CustomFuncsWindow::actionCancel);
    
    _infoBanner = LayoutH({ bannerText, Space(24), btnSave, btnCancel }).makeWidget();
    
    auto filesPanel = LayoutV({_filterEdit, _fileList}).setMargins(3, 0, 0, 3).makeWidget();
    auto editorPanel = LayoutV({_editor, _infoBanner}).setMargins(0, 0, 3, 3).makeWidget();
    
    _splitter = Ori::Gui::splitterH(filesPanel, 1, editorPanel, 2);
    
    LayoutV({_toolbar, _splitter})
        .setMargin(0)
        .setSpacing(0)
        .useFor(this);

    _fadingWidgets << filesPanel << _toolbar;
    
    setEditMode(false);
    fillFileList();
    restoreState();
    
    Ori::Wnd::moveToScreenCenter(this);
}

CustomFuncsWindow::~CustomFuncsWindow()
{
    storeState();
}

void CustomFuncsWindow::restoreState()
{
    QJsonObject root = PersistentState::load(STATE_KEY);
    PersistentState::restoreWindowGeometry(root, this, {800, 600});
    if (root.contains("splitter")) {
        auto splitterState = QByteArray::fromBase64(root["splitter"].toString().toLatin1());
        _splitter->restoreState(splitterState);
    }
}

void CustomFuncsWindow::storeState()
{
    QJsonObject root = PersistentState::load(STATE_KEY);
    PersistentState::storeWindowGeometry(root, this);
    root["splitter"] = QString::fromLatin1(_splitter->saveState().toBase64());
    PersistentState::save(STATE_KEY, root);
}

void CustomFuncsWindow::createActions()
{
    #define A_ Ori::Gui::V0::action
    #define K_(s) QKeySequence(s)
    
    _actnApply = A_(tr("Apply to Schema"), this, SLOT(actionApply()), ":/toolbar/start", K_("Ctrl+Enter"));
    _actnDelete = A_(tr("Delete"), this, SLOT(actionDelete()), ":/toolbar/trash", K_("Ctrl+Del"));
    _actnEdit = A_(tr("Edit"), this, SLOT(actionEdit()), ":/toolbar/pencil", K_("F2"));
    _actnSave = A_(tr("Save"), this, SLOT(actionSave()), ":/toolbar/save", K_("Ctrl+S"));
    _actnCancel = A_(tr("Cancel"), this, SLOT(actionCancel()), ":/toolbar/stop", K_("Esc"));
    _actnExport = A_(tr("Export..."), this, SLOT(actionExport()), ":/toolbar/book_arrow_up", K_("Shift+Ctrl+S"));
    _actnImport = A_(tr("Import..."), this, SLOT(actionImport()), ":/toolbar/book_arrow_down", K_("Shift+Ctrl+O"));
    _actnHelp = A_(tr("Help"), this, SLOT(actionHelp()), ":/toolbar/help", QKeySequence::HelpContents);
    
    #undef K_
    #undef A_
}

void CustomFuncsWindow::createToolbar()
{
    _toolbar = Z::Gui::makeToolbar({
        Ori::Gui::textToolButton(_actnApply),
        nullptr,
        Ori::Gui::textToolButton(_actnExport),
        Ori::Gui::textToolButton(_actnImport),
        nullptr,
        Ori::Gui::textToolButton(_actnEdit),
        Ori::Gui::textToolButton(_actnDelete),
        nullptr,
        _actnHelp
    });
}

void CustomFuncsWindow::closeEvent(QCloseEvent *event)
{
    auto curFilePath = selectedPath();
    if (!curFilePath.isEmpty() && _editor->document()->isModified())
    {
        if (!saveModifiedCode(curFilePath))
        {
            // Keep window open if error or user canceled
            event->ignore();
            return;
        }
    }
    QWidget::closeEvent(event);
}

bool CustomFuncsWindow::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate)
        fillFileList();
    return QWidget::event(event);
}

void CustomFuncsWindow::setEditMode(bool on)
{
    _infoBanner->setVisible(on);
    
    for (auto w : std::as_const(_fadingWidgets)) {
        w->setEnabled(!on);
        if (on) {
            auto f = new QGraphicsOpacityEffect(w);
            f->setOpacity(0.4);
            w->setGraphicsEffect(f);
            w->setEnabled(false);
        } else {
            w->setGraphicsEffect(nullptr);
        }
    }
    
    auto actns = _toolbar->actions();
    for (auto a : std::as_const(actns))
        a->setEnabled(!on);
    _actnSave->setEnabled(on);
    _actnCancel->setEnabled(on);
        
    _editor->setReadOnly(!on);
    // TODO: CodeEditor should handle this by itself
    _editor->setStyleSheet(QString("QPlainTextEdit {background: %1}")
        .arg((on ? _editor->style().backColor : _editor->style().readonlyBackColor).name()));
    if (on)
        _editor->setFocus();
    else
        _editor->document()->setModified(false);
}

static QString itemPath(QListWidgetItem *item)
{
    return item ? item->data(Qt::UserRole).toString() : QString();
}

QString CustomFuncsWindow::selectedPath() const
{
    return itemPath(_fileList->currentItem());
}

void CustomFuncsWindow::fillFileList()
{
    QString oldCurPath = selectedPath();
    
    QHash<QString, QListWidgetItem*> listItems;
    for (int i = 0; i < _fileList->count(); i++) {
        auto item = _fileList->item(i);
        listItems[itemPath(item)] = item;
    }
    
    QStringList filePaths = CustomFuncsLib::getAllFiles();
    QSet<QString> existingPaths(filePaths.cbegin(), filePaths.cend());
    
    QList<QString> removedPaths;
    for (auto it = listItems.cbegin(); it != listItems.cend(); it++)
        if (!existingPaths.contains(it.key()))
            removedPaths << it.key();
    for (const QString &filePath : std::as_const(removedPaths))
        delete listItems.take(filePath);

    for (const QString &filePath : std::as_const(existingPaths))
    {
        auto res = CustomFuncsLib::loadCode(filePath);
        if (!res.ok()) {
            qWarning() << "Failed to load custom function file:" << filePath << res.error();
            continue;
        }

        QString code = res.result();
        if (code.isEmpty()) {
            qWarning() << "Custom function file is empty:" << filePath;
            continue;
        }

        QString displayName = CustomFuncsLib::getDisplayName(code);
        if (displayName.isEmpty()) {
            qWarning() << "File looks like a custom function but its name is empty:" << filePath;
            continue;
        }
        
        QListWidgetItem *item = listItems.value(filePath);
        if (!item) {
            item = new QListWidgetItem;
            listItems.insert(filePath, item);
            _fileList->addItem(item);
        }

        if (AppSettings::instance().isDevMode)
            displayName += "\n" + QFileInfo(filePath).fileName();
        item->setText(displayName);
        item->setIcon(getFileIcon(code));
        item->setData(Qt::UserRole, filePath);
    }
    _fileList->sortItems(Qt::AscendingOrder);
    
    auto curItem = listItems.value(_requiredSelectedPath.isEmpty() ? oldCurPath : _requiredSelectedPath);
    if (!curItem && _fileList->count() > 0)
        curItem = _fileList->item(0);
    if (curItem)
        _fileList->setCurrentItem(curItem);
    _requiredSelectedPath.clear();
    applyFilter();
}

void CustomFuncsWindow::showCurrentCode()
{
    auto filePath = selectedPath();
    if (filePath.isEmpty())
    {
        _editor->clear();
        return;
    }
    auto res = CustomFuncsLib::loadCode(filePath);
    _editor->setPlainText(res.ok() ? res.result(): res.error());
}

void CustomFuncsWindow::actionEdit()
{
    setEditMode(true);
}

bool CustomFuncsWindow::saveModifiedCode(const QString &filePath) const
{
    auto code = _editor->toPlainText();
    auto displayName = CustomFuncsLib::getDisplayName(code);
    auto res = Ori::Dlg::yesNo(tr(
        "Custom function <b>%1</b> has unsaved changes."
        "<br><br>Save changes before closing?").arg(displayName));
    if (!res.has_value()) // Cancel
        return false;
    if (res.value()) // Yes
    {
        auto savedPath = CustomFuncsLib::put(code, filePath);
        if (savedPath.isEmpty())
            return false;
    }
    return true;
}

void CustomFuncsWindow::actionCancel()
{
    auto curFilePath = selectedPath();
    if (!curFilePath.isEmpty() && _editor->document()->isModified())
        if (!saveModifiedCode(curFilePath))
        {
            // Keep edit mode if error or user canceled
            return;
        }

    showCurrentCode();
    setEditMode(false);
}

void CustomFuncsWindow::actionDelete()
{
    auto curFilePath = selectedPath();
    if (curFilePath.isEmpty())
        return;
    
    auto displayName = CustomFuncsLib::getDisplayName(_editor->code());
    if (!Ori::Dlg::yes(tr(
        "Delete custom function <b>%1</b>?<br><br>"
        "All your existing schemas will remain untouched.").arg(displayName)))
        return;
    
    CustomFuncsLib::remove(curFilePath);
    fillFileList();
}

void CustomFuncsWindow::actionApply()
{
    // TODO
}

void CustomFuncsWindow::actionSave()
{
    auto curFilePath = selectedPath();
    if (curFilePath.isEmpty()) return;

    auto code = _editor->toPlainText();
    auto savedPath = CustomFuncsLib::put(code, curFilePath);
    // Don't switch off the Edit mode is save failed
    if (savedPath.isEmpty()) return;

    _requiredSelectedPath = savedPath;
    setEditMode(false);
    fillFileList();
}

QString CustomFuncsWindow::fileFilters() const
{
    return tr("Python Files (*.py);;All Files (*.*)");
}

void CustomFuncsWindow::actionExport()
{
    auto item = _fileList->currentItem();
    if (!item)
        return;
    
    auto code = _editor->toPlainText();
    auto displayName = CustomFuncsLib::getDisplayName(code);

    auto root = PersistentState::load(STATE_KEY);
    auto recentDir = root["recent_dir"].toString();
    
    QString suggestedName(displayName);
    suggestedName.replace(QRegularExpression("[^a-zA-Z0-9_\\-]"), "_");
    suggestedName = recentDir + '/' + suggestedName + ".py";
    
    auto exportPath = QFileDialog::getSaveFileName(this, tr("Export Custom Function"), suggestedName, fileFilters());
    if (exportPath.isEmpty())
        return;

    root["recent_dir"] = QFileInfo(exportPath).dir().absolutePath();
    PersistentState::save(STATE_KEY, root);

    if (CustomFuncsLib::saveCode(_editor->toPlainText(), exportPath))
        showInfoPopup(tr("Custom function <b>%1</b> exported successfully").arg(displayName));
}

void CustomFuncsWindow::actionImport()
{
    auto root = PersistentState::load(STATE_KEY);
    auto recentDir = root["recent_dir"].toString();

    auto importPath = QFileDialog::getOpenFileName(this, tr("Import Custom Function"),  recentDir, fileFilters());
    if (importPath.isEmpty())
        return;

    root["recent_dir"] = QFileInfo(importPath).dir().absolutePath();
    PersistentState::save(STATE_KEY, root);
    
    auto res = CustomFuncsLib::loadCode(importPath);
    if (!res.ok())
    {
        Ori::Dlg::error(tr("Failed to open file:\n%1\n\nError: %2").arg(importPath, res.error()));
        return;
    }
    
    auto code = res.result();
    if (code.isEmpty())
    {
        Ori::Dlg::error(tr("The file is empty"));
        return;
    }
    
    auto displayName = CustomFuncsLib::getDisplayName(code);
    
    if (!hasFuncDef(code, CustomFuncUtils::funcNameCalculate()))
    {
        Ori::Dlg::error(tr(
            "The importing file does not contain a <b>%1</b> function.<br><br>"
            "Custom functions require a <code>def %1():</code> statement declaring "
            "a function that serves as the entry point for calculations."
            ).arg(CustomFuncUtils::funcNameCalculate()));
        return;
    }
    
    auto savedPath = CustomFuncsLib::put(code);
    if (!savedPath.isEmpty())
    {
        showInfoPopup(tr("Custom function <b>%1</b> imported successfully").arg(displayName));
        _requiredSelectedPath = savedPath;
        fillFileList();
    }
}

void CustomFuncsWindow::actionHelp()
{
    Z::HelpSystem::instance()->showTopic("custom_func_lib");
}

void CustomFuncsWindow::onFileSelected(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);
    
    if (current)
        showCurrentCode();
    else
        _editor->clear();
}

void CustomFuncsWindow::applyFilter()
{
    QListWidgetItem *firstVisibleItem = nullptr;
    auto curItem = _fileList->currentItem();
    bool curItemHidden = false;

    QStringList rawTerms = _filterEdit->text().split(' ', Qt::SkipEmptyParts);
    QStringList searchTerms;
    
    for (const auto& term : std::as_const(rawTerms))
        if (term.size() >= 2)
            searchTerms << term;
    
    for (int i = 0; i < _fileList->count(); ++i)
    {
        auto item = _fileList->item(i);
        if (searchTerms.isEmpty())
        {
            item->setHidden(false);
            continue;
        }
        bool allTermsMatch = true;
        auto displayName = item->text();
        for (const QString& term : std::as_const(searchTerms))
        {
            bool hasMatch = displayName.contains(term, Qt::CaseInsensitive);
            if (!hasMatch)
            {
                allTermsMatch = false;
                break;
            }
        }
        bool isHidden = !allTermsMatch;
        item->setHidden(isHidden);
        if (isHidden)
        {
            if (item == curItem)
                curItemHidden = true;
        }
        else if (!firstVisibleItem)
            firstVisibleItem = item;
    }

    if (curItemHidden)
        _fileList->setCurrentItem(firstVisibleItem);
}

void CustomFuncsWindow::showInfoPopup(const QString &text)
{
    Ori::Gui::PopupMessage::show({
        .mode = Ori::Gui::PopupMessage::AFFIRM,
        .text = text,
    }, this);
}
