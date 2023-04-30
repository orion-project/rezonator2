#include "CustomPrefs.h"

#include "tools/OriSettings.h"

#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QWidget>

//--------------------------------------------------------------------------------
//                               CustomDataHelpers
//--------------------------------------------------------------------------------

namespace CustomDataHelpers {

QString dataFileName(const QString& spec)
{
    Ori::Settings s;
    return s.settings()->fileName().section('.', 0, -2) % '.' % spec % QStringLiteral(".json");
}

QJsonObject loadCustomData(const QString& spec)
{
    QJsonObject root;
    auto fileName = dataFileName(spec);
    QFile file(fileName);
    if (file.exists())
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            qWarning() << spec << "failed to load state" << fileName << file.errorString();
        else
        {
            QJsonParseError error;
            root = QJsonDocument::fromJson(file.readAll(), &error).object();
            if (error.error != QJsonParseError::NoError)
                qWarning() << spec << "failed to load state" << error.errorString();
        }
    }
    return root;
}

void saveCustomData(const QJsonObject& root, const QString& spec)
{
    auto fileName = dataFileName(spec);
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << spec << "failed to save state" << fileName << file.errorString();
        return;
    }
    QTextStream(&file) << QJsonDocument(root).toJson();
}

void storeWindowSize(QJsonObject& root, QWidget* wnd)
{
    root["window_width"] = wnd->width();
    root["window_height"] = wnd->height();
}

void restoreWindowSize(const QJsonObject& root, QWidget* wnd, int defaultW, int defaultH)
{
    int w = root["window_width"].toInt();
    int h = root["window_height"].toInt();
    if (w == 0 || h == 0)
    {
        w = defaultW;
        h = defaultH;
    }
    wnd->resize(w, h);
}

} // namespace CustomDataHelpers

//--------------------------------------------------------------------------------
//                                   CustomData
//--------------------------------------------------------------------------------

namespace CustomData {

static QJsonObject __customData;
static QString __storagePath;


void load(const QString& storagePath)
{
    __storagePath = storagePath;

    QFile file(__storagePath);
    if (file.exists())
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qWarning() << "Unable to load custom prefs" << __storagePath << file.errorString();
            return;
        }
        QJsonParseError error;
        __customData = QJsonDocument::fromJson(file.readAll(), &error).object();
        if (error.error != QJsonParseError::NoError)
            qWarning() << "Unable to load custom prefs" << error.errorString();
    }
}

void save()
{
    QFile file(__storagePath);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << "Unable to save custom prefs" << __storagePath << file.errorString();
        return;
    }
    QTextStream(&file) << QJsonDocument(__customData).toJson();
}

/// Returns key of max counter from counter object
QString maxCounterKey(const QString& counterObjKey)
{
    auto counterObj = __customData[counterObjKey].toObject();
    int maxCounter = 0;
    QString maxCounterKey;
    for (auto& counterKey: counterObj.keys())
    {
        int counter = counterObj[counterKey].toInt();
        if (counter > maxCounter)
        {
            maxCounter = counter;
            maxCounterKey = counterKey;
        }
    }
    return maxCounterKey;
}

/// Increase counter with specific key in counter object
void increaseCounterKey(const QString& counterObjKey, const QString& counterKey)
{
    auto counterObj = __customData[counterObjKey].toObject();
    counterObj[counterKey] = counterObj[counterKey].toInt() + 1;
    __customData[counterObjKey] = counterObj;
}

} // namespace CustomData

using namespace CustomData;

//------------------------------------------------------------------------------
//                                CustomPrefs
//------------------------------------------------------------------------------

void CustomPrefs::load(const QString &appConfigFile)
{
    ::load(appConfigFile.section('.', 0, -2) + ".prefs.json");
}

void CustomPrefs::setRecentDim(const QString& key, Z::Dim dim)
{
    __customData[key] = dim->alias();
    save();
}

Z::Dim CustomPrefs::recentDim(const QString& key)
{
    return Z::Dims::findByAliasOrNone(__customData[key].toString());
}

void CustomPrefs::setRecentUnit(const QString& key, Z::Unit unit)
{
    auto unitByDim = __customData[key].toObject();
    unitByDim[Z::Units::guessDim(unit)->alias()] = unit->alias();
    __customData[key] = unitByDim;
    save();
}

Z::Unit CustomPrefs::recentUnit(const QString& key, Z::Dim dim)
{
    auto unitByDim = __customData[key].toObject();
    return dim->unitByAliasOrSi(unitByDim[dim->alias()].toString());
}

void CustomPrefs::setRecentDir(const QString& key, const QString& dirOrFile)
{
    QString dir;
    // Existing file
    if (QFile::exists(dirOrFile))
        dir = QFileInfo(dirOrFile).absolutePath();
    // Existing dir
    else if (QDir(dirOrFile).exists())
        dir = QDir(dirOrFile).absolutePath();
    // Non-existing (a new) file
    else {
        auto dir1 = QFileInfo(dirOrFile).absoluteDir();
        if (dir1.exists()) dir = dir1.absolutePath();
    }
    if (!dir.isEmpty())
        setRecentStr(key, dir);
}

QString CustomPrefs::recentDir(const QString& key, const QString& defaultDir)
{
    QString dir = recentStr(key, defaultDir);
    return (dir.isEmpty() || !QDir(dir).exists()) ? QDir::currentPath() : dir;
}

void CustomPrefs::setRecentStr(const QString& key, const QString& value)
{
    if (__customData[key].toString() != value)
    {
        __customData[key] = value;
        save();
    }
}

QString CustomPrefs::recentStr(const QString& key, const QString& defaultStr)
{
    return __customData[key].toString(defaultStr);
}

void CustomPrefs::setRecentObj(const QString& key, const QJsonObject& obj)
{
    if (!__customData.contains(key) || __customData[key].toObject() != obj)
    {
        __customData[key] = obj;
        save();
    }
}

QJsonObject CustomPrefs::recentObj(const QString &key)
{
    return __customData[key].toObject();
}

QSize CustomPrefs::recentSize(const QString& key, const QSize& defaultSize)
{
    if (!__customData.contains(key)) return defaultSize;
    auto json = __customData[key].toObject();
    return QSize(json["width"].toInt(), json["height"].toInt());
}

void CustomPrefs::setRecentSize(const QString& key, const QSize& size)
{
    if (recentSize(key) != size)
    {
        __customData[key] = QJsonObject({
            { "width", size.width() },
            { "height", size.height() }
        });
        save();
    }
}
