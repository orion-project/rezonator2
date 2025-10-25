#include "ExamplesDialog.h"

#include "../app/AppSettings.h"
#include "../app/PersistentState.h"
#include "../io/CommonUtils.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDir>
#include <QJsonDocument>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QSplitter>

#define LOG_ID "ExamplesDialog"

ExamplesDialog::ExamplesDialog(): QSplitter()
{
    _examplesDir = qApp->applicationDirPath() % "/examples";

    _fileList = new QListWidget;
#ifdef Q_OS_WIN
    // Default icon size looks OK on Ubuntu and MacOS but it is too small on Windows
    _fileList->setIconSize(QSize(24, 24));
#endif
    connect(_fileList, &QListWidget::itemDoubleClicked, this, &ExamplesDialog::accepted);
    connect(_fileList, &QListWidget::currentItemChanged, this, &ExamplesDialog::showCurrentExample);

    _preview = new QTextBrowser;
    _preview->document()->setDefaultStyleSheet(R"(
        html { font-size: 10pt }
        body { margin-left: 5px; }
        p { margin-top: 10px; }
    )");
    
    _filterEdit = new QLineEdit;
    _filterEdit->setPlaceholderText(qApp->tr("Filter examples..."));
    _filterEdit->setClearButtonEnabled(true);
    connect(_filterEdit, &QLineEdit::textChanged, this, &ExamplesDialog::applyFilter);
    
    auto leftPanel = new QWidget;
    auto leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setMargin(0);
    
    leftLayout->addWidget(_filterEdit);
    leftLayout->addWidget(_fileList);
    if (AppSettings::instance().isDevMode)
    {
        auto button = new QPushButton("Edit");
        connect(button, &QPushButton::clicked, this, &ExamplesDialog::editExampleDescr);
        leftLayout->addWidget(button);
    }
    
    addWidget(leftPanel);
    addWidget(_preview);
    setStretchFactor(0, 1);
    setStretchFactor(1, 2);
    setSizes({250, 500});
    
    loadExamples();
}

QString ExamplesDialog::selectedFile() const
{
    auto selected = _fileList->currentItem();
    if (selected)
        return _examplesDir + '/' % selected->text();
    return {};
}

QString ExamplesDialog::exec()
{
    ExamplesDialog dlg;
    if (Ori::Dlg::Dialog(&dlg, false)
        .withTitle(qApp->tr("Open Example Schema"))
        .withStretchedContent()
        .withInitialSize(RecentData::getSize("open_example_dlg_size", {750, 500}))
        .withSizeSaver([](QSize sz){ RecentData::setSize("open_example_dlg_size", sz); })
        .withOkSignal(SIGNAL(accepted()))
        .exec())
    {
        return dlg.selectedFile();
    }
    return {};
}

void ExamplesDialog::showCurrentExample()
{
    auto item = _fileList->currentItem();
    if (!item) return;

    _preview->setMarkdown(_descrs.value(item->text()));
    // Workaround for applying stylesheet for markdown
    //_preview->setHtml(_preview->toHtml());
    
    // setMarkdown() ignores default stylesheet
    // As a workaround, we clean some markdown's styles we don't like
    // and re-set the same content as html, then the default stylesheet gets applied
    QString text = _preview->toHtml();
    static QVector<QPair<QRegularExpression, QString>> patterns = {
        // { QRegularExpression("\\<h1.+?\\>"), "<h1>" },
        // { QRegularExpression("\\<h2.+?\\>"), "<h2>" },
        // { QRegularExpression("\\<h3.+?\\>"), "<h3>" },
        { QRegularExpression("\\<p.+?\\>"), "<p>" },
    };
    for (const auto& p : patterns)
        text.replace(p.first, p.second);
    _preview->setHtml(text);
}

void ExamplesDialog::loadExamples()
{
    QStringList exampleFiles = QDir(_examplesDir).entryList(QDir::Files, QDir::Name);
#ifdef Q_OS_MAC
    if (exampleFiles.isEmpty())
    {
        // Look near the application bundle, it is for development mode
        QString examplesDir = qApp->applicationDirPath() % "/../../../examples";
        examplesDir = QDir(examplesDir).absolutePath();
        exampleFiles = QDir(examplesDir).entryList(QDir::Files, QDir::Name);
    }
#endif

    auto root = PersistentState::load("examples");
    bool changed = false;
    
    for (auto& fileName : exampleFiles)
    {
        if (!fileName.endsWith(Z::IO::Utils::suffix()))
            continue;
        _fileList->addItem(new QListWidgetItem(QIcon(":/window_icons/schema"), fileName));
        auto fileDate = QFileInfo(_examplesDir + '/' + fileName).lastModified().toMSecsSinceEpoch();
        qint64 cacheDate = 0;
        QString text;
        if (root.contains(fileName))
        {
            auto fileJson = root[fileName].toObject();
            cacheDate = fileJson["date"].toInt(0);
            text = fileJson["text"].toString();
        }
        if (fileDate > cacheDate)
        {
            text = loadExampleDescr(fileName);
            if (!text.isEmpty())
            {
                root[fileName] = QJsonObject{
                    { "date", fileDate },
                    { "text", text },
                };
                changed = true;
            }
        }
        _descrs[fileName] = text;
    }
    
    if (changed)
        PersistentState::save("examples", root);
}

QJsonObject ExamplesDialog::loadExampleFile(const QString& fileName)
{
    QFile file(_examplesDir + '/' + fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << LOG_ID << "Failed to load file" << fileName << file.errorString();
        return {};
    }
    auto bytes = file.readAll();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(bytes, &error);
    if (doc.isNull())
    {
        qWarning() << LOG_ID << "Filed to parse file" << fileName << error.errorString();
        return {};
    }
    return doc.object();
}

QString ExamplesDialog::loadExampleDescr(const QString& fileName)
{
    auto root = loadExampleFile(fileName);
    if (root.isEmpty()) return {};
    QString text;
    auto title = root["title"].toString();
    if (!title.isEmpty())
        text += "## " + title + "\n\n";
    auto descr = root["notes"].toString();
    if (!descr.isEmpty())
        text += descr;
    return text;
}

void ExamplesDialog::applyFilter()
{
    QString filterText = _filterEdit->text().trimmed();
    
    // Count alphabetic characters in filter text
    int alphaCount = 0;
    for (const QChar& ch : std::as_const(filterText))
        if (ch.isLetter())
            alphaCount++;
    
    for (int i = 0; i < _fileList->count(); ++i)
    {
        auto item = _fileList->item(i);
        if (filterText.isEmpty())
        {
            item->setHidden(false);
            continue;
        }
        if (alphaCount < 3)
        {
            item->setHidden(true);
            continue;
        }
        auto fileName = item->text();
        if (!_plainTextDescrs.contains(fileName))
        {
            // Strip markdown formatting that is not visible in rendered view
            auto plainText = _descrs.value(fileName);
            plainText.replace(QRegularExpression("^\\s*#{1,6}\\s*", QRegularExpression::MultilineOption), ""); // Headers
            plainText.replace(QRegularExpression("^\\s*[-*+]\\s+", QRegularExpression::MultilineOption), ""); // Unordered lists
            plainText.replace(QRegularExpression("^\\s*\\d+\\.\\s+", QRegularExpression::MultilineOption), ""); // Ordered lists
            _plainTextDescrs[fileName] = plainText;
        }
        bool matches = _plainTextDescrs.value(fileName).contains(filterText, Qt::CaseInsensitive);
        item->setHidden(!matches);
    }
}

void ExamplesDialog::editExampleDescr()
{
    auto item = _fileList->currentItem();
    if (!item) return;
    auto fileName = item->text();
    auto root = loadExampleFile(fileName);
    if (root.isEmpty()) return;
    auto title = root["title"].toString();
    auto descr = root["notes"].toString();
    auto titleEdit = new QLineEdit;
    auto descrEdit = new QPlainTextEdit;
    titleEdit->setText(title);
    descrEdit->setPlainText(descr);
    auto w = Ori::Layouts::LayoutV({
        "Title", titleEdit,
        "Description", descrEdit
    }).setMargin(0).makeWidgetAuto();
    if (Ori::Dlg::Dialog(w).exec())
    {
        bool changed = false;
        auto newTitle = titleEdit->text().trimmed();
        auto newDescr = descrEdit->toPlainText().trimmed();
        if (newTitle != title)
        {
            root["title"] = newTitle;
            changed = true;
        }
        if (newDescr != descr)
        {
            root["notes"] = newDescr;
            changed = true;
        }
        if (changed)
        {
            QJsonDocument doc(root);
            QFile file(_examplesDir + '/' + fileName);
            if (!file.open(QFile::WriteOnly | QFile::Text | QFile::ExistingOnly))
            {
                qWarning() << "Unable to open file for writing" << fileName << file.errorString();
                return;
            }
            QTextStream stream(&file);
            stream << doc.toJson();
            file.close();
            qDebug() << "Updated" << fileName;
            loadExamples();
            showCurrentExample();
        }
    }
}
