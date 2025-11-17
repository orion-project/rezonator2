#include "CustomFuncsLib.h"

#include "helpers/OriDialogs.h"
#include "tools/OriSettings.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QUuid>

namespace CustomFuncsLib {

QString funcsDir()
{
    Ori::Settings s;
    QFileInfo fi(s.settings()->fileName());
    return fi.absolutePath();
}

QString getDisplayName(const QString &code)
{
    // Find opening triple quotes
    bool isDoubleQuote = true;
    int docstringStart = code.indexOf(QStringLiteral("\"\"\""));
    if (docstringStart == -1)
    {
        docstringStart = code.indexOf(QStringLiteral("'''"));
        isDoubleQuote = false;
    }
    if (docstringStart == -1)
        return {};
    
    // Move past the opening triple quotes
    docstringStart += 3;
    
    // Find closing triple quotes
    QString closingQuotes = isDoubleQuote ? QStringLiteral("\"\"\"") : QStringLiteral("'''");
    int docstringEnd = code.indexOf(closingQuotes, docstringStart);
    if (docstringEnd == -1)
        return {};
    
    QString docstring = code.mid(docstringStart, docstringEnd - docstringStart);
    
    QStringList lines = docstring.split('\n');
    for (const QString &line : std::as_const(lines))
    {
        QString trimmedLine = line.trimmed();
        if (!trimmedLine.isEmpty())
            return trimmedLine;
    }
    
    return {};
}

QString customFileName(const QString &id)
{
    return QString("rezonator.cf_%1.py").arg(id);
}

QString findExistingFile(const QString &displayName)
{
    if (displayName.isEmpty())
        return QString();
        
    QStringList filePaths = getAllFiles();
    for (const QString &path : std::as_const(filePaths))
    {
        auto res = loadCode(path);
        if (!res.ok())
            continue;
        QString code = res.result();
        QString existingName = getDisplayName(code);
        if (existingName == displayName)
            return path;
    }
    return {};
}

QStringList getAllFiles()
{
    QDir dir(funcsDir());
    QFileInfoList fileInfos = dir.entryInfoList({ customFileName("*") }, QDir::Files);
    QStringList filePaths;
    for (const QFileInfo &fileInfo : std::as_const(fileInfos))
        filePaths << fileInfo.absoluteFilePath();
    return filePaths;
}

bool saveCode(const QString &code, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        Ori::Dlg::error(QObject::tr(
            "Failed to save custom function to:\n%1\n\nError: %2"
        ).arg(filePath, file.errorString()));
        return false;
    }
    
    QTextStream stream(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#else
    stream.setEncoding(QStringConverter::Utf8);
#endif
    stream << code;
    return true;
}

Ori::Result<QString> loadCode(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to load custom function:" << filePath << file.errorString();
        return Ori::Result<QString>::fail(file.errorString());
    }
    
    QTextStream stream(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#else
    stream.setEncoding(QStringConverter::Utf8);
#endif
    
    QString code = stream.readAll().trimmed();
    return Ori::Result<QString>::ok(code);
}

QString makeNewPath()
{
    QString uuid = QUuid::createUuid().toString(QUuid::Id128);
    return funcsDir() % '/' % customFileName(uuid);
}

QString put(const QString &code, const QString &targetPath)
{
    QString displayName = getDisplayName(code);
    if (displayName.isEmpty())
    {
        Ori::Dlg::error(QObject::tr(
            "Failed to extract function name from function code. "
            "Make sure the code has a docstring with a name on the first line."));
        return {};
    }
    
    QString savePath, removePath;
    QString existingPath = findExistingFile(displayName);
    if (existingPath.isEmpty())
    {
        savePath = targetPath.isEmpty() ? makeNewPath() : targetPath;
    }
    else if (existingPath == targetPath)
    {
        savePath = targetPath;
    }
    else if (Ori::Dlg::yes(QObject::tr(
            "A function with the name <b>%1</b> already exists. "
            "Do you want to replace it?").arg(displayName)))
    {
        savePath = existingPath;
        removePath = targetPath;
    }
    
    if (!savePath.isEmpty() && saveCode(code, savePath))
    {
        if (!removePath.isEmpty())
            remove(removePath);
    
        return savePath;
    }

    return {};
}

bool remove(const QString &filePath)
{
    QFile file(filePath);
    if (!file.remove())
    {
        Ori::Dlg::error(QObject::tr(
            "Failed to delete custom function:\n%1\n\nError: %2"
        ).arg(filePath, file.errorString()));
        return false;
    }
    return true;
}

} // namespace CustomFuncsLib
