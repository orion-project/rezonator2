#include "ExamplesDialog.h"

#include "../app/AppSettings.h"
#include "../app/PersistentState.h"
#include "../core/CommonTypes.h"
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
#include <QSplitter>
#include <QTextBrowser>
#include <QTimer>

ExamplesDialog::ExamplesDialog(): QSplitter()
{
    _examplesDir = qApp->applicationDirPath() % "/examples";

    _fileList = new QListWidget;
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

    //QTimer::singleShot(0, this, [this]{ _fileList->setFocus(); });
}

QString ExamplesDialog::examplePath(const QString &fileName, const QString &ext) const
{
    QString path = _examplesDir + '/' + fileName;
    if (!ext.isEmpty())
    {
        int pos = path.lastIndexOf('.');
        if (pos > 0)
            path = path.left(pos+1) + ext;
    }
    return path;
}

QString ExamplesDialog::selectedFile() const
{
    auto selected = _fileList->currentItem();
    if (selected)
        return examplePath(selected->text());
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

    auto fileName = item->text();
    auto descr = _descrs.value(fileName);
    
    auto imageFile = examplePath(fileName, "svg");
    if (!QFile::exists(imageFile))
    {
        imageFile = examplePath(fileName, "png");
        if (!QFile::exists(imageFile))
            imageFile.clear();
    }
    if (!imageFile.isEmpty())
        descr += QString("\n\n![image](%1)").arg(imageFile);

    _preview->setMarkdown(descr);
    
    // setMarkdown() ignores default stylesheet
    // As a workaround, we clean some markdown's styles we don't like
    // and re-set the same content as html, then the default stylesheet gets applied
    // (but even then it's unable to change header font size)
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
    _fileList->clear();

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
        auto fileDate = QFileInfo(examplePath(fileName)).lastModified().toSecsSinceEpoch();
        qint64 cacheDate = 0;
        Info info;
        if (root.contains(fileName))
        {
            auto fileJson = root[fileName].toObject();
            cacheDate = fileJson["date"].toInt();
            info.descr = fileJson["text"].toString();
            info.tripType = fileJson["trip_type"].toString();
        }
        if (fileDate > cacheDate)
        {
            info = loadExampleInfo(fileName);
            root[fileName] = QJsonObject{
                { "date", fileDate },
                { "text", info.descr },
                { "trip_type", info.tripType }
            };
            qDebug() << "Example updated" << fileName;
            changed = true;
        }
        _descrs[fileName] = info.descr;

        QString icon(":/window_icons/schema");
        bool ttFound;
        auto tt = TripTypes::find(info.tripType, &ttFound);
        if (ttFound)
            icon = TripTypes::info(tt).windowIconPath();
        _fileList->addItem(new QListWidgetItem(QIcon(icon), fileName));
    }

    QStringList removedFiles;
    for (auto it = root.constBegin(); it != root.constEnd(); it++)
        if (!exampleFiles.contains(it.key()))
            removedFiles << it.key();
    if (!removedFiles.isEmpty())
    {
        changed = true;
        for (const auto &fileName : std::as_const(removedFiles))
        {
            qDebug() << "Example removed" << fileName;
            root.remove(fileName);
        }
    }
    
    if (changed)
        PersistentState::save("examples", root);
}

QJsonObject ExamplesDialog::loadExampleFile(const QString& fileName)
{
    QFile file(examplePath(fileName));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to load file" << fileName << file.errorString();
        return {};
    }
    auto bytes = file.readAll();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(bytes, &error);
    if (doc.isNull())
    {
        qWarning() << "Filed to parse file" << fileName << error.errorString();
        return {};
    }
    return doc.object();
}

ExamplesDialog::Info ExamplesDialog::loadExampleInfo(const QString& fileName)
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
    return {
        .descr = text,
        .tripType = root["trip_type"].toString()
    };
}

void ExamplesDialog::applyFilter()
{
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
        
        auto fileName = item->text();
        
        // Prepare plain text description if not cached
        if (!_plainTextDescrs.contains(fileName))
        {
            // Strip markdown formatting that is not visible in rendered view
            auto plainText = _descrs.value(fileName);
            plainText.replace(QRegularExpression("^\\s*#{1,6}\\s*", QRegularExpression::MultilineOption), ""); // Headers
            plainText.replace(QRegularExpression("^\\s*[-*+]\\s+", QRegularExpression::MultilineOption), ""); // Unordered lists
            plainText.replace(QRegularExpression("^\\s*\\d+\\.\\s+", QRegularExpression::MultilineOption), ""); // Ordered lists
            _plainTextDescrs[fileName] = plainText;
        }
        auto plainTextDescr = _plainTextDescrs.value(fileName);
        
        // Check if ALL search terms match (AND logic)
        bool allTermsMatch = true;
        for (const QString& term : std::as_const(searchTerms))
        {
            bool hasMatch = fileName.contains(term, Qt::CaseInsensitive) || 
                plainTextDescr.contains(term, Qt::CaseInsensitive);
            if (!hasMatch)
            {
                allTermsMatch = false;
                break;
            }
        }
        
        item->setHidden(!allTermsMatch);
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
            QFile file(examplePath(fileName));
            if (!file.open(QFile::WriteOnly | QFile::Text | QFile::ExistingOnly))
            {
                qWarning() << "Unable to open file for writing" << fileName << file.errorString();
                return;
            }
            file.write(QJsonDocument(root).toJson());
            file.close();
            qDebug() << "Saved" << fileName;
            loadExamples();
            for (int i = 0; i < _fileList->count(); i++)
                if (_fileList->item(i)->text() == fileName)
                {
                    _fileList->setCurrentRow(i);
                    showCurrentExample();
                    break;
                }
        }
    }
}
