#include "CustomPrefs.h"

#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

namespace CustomData {

QJsonObject __customData;
QString __storagePath;

void load(const QString& storagePath)
{
    __storagePath = storagePath;

    QFile file(__storagePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Unable to load custom prefs" << __storagePath << file.errorString();
        return;
    }
    __customData = QJsonDocument::fromJson(file.readAll()).object();
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
    for (auto counterKey: counterObj.keys())
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
    if (QFile::exists(dirOrFile))
        dir = QFileInfo(dirOrFile).absolutePath();
    else if (QDir(dirOrFile).exists())
        dir = QDir(dirOrFile).absolutePath();
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
