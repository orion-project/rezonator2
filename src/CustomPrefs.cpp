#include "CustomPrefs.h"
#include "tools/OriSettings.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QTextStream>

namespace CustomData {

QJsonObject __customData;

QString storagePath()
{
    // TODO use local path in portabe mode, or user profile path otherwise
    // TODO change ext, not just append it
    return Ori::Settings::localIniPath() + ".json";
}

void load()
{
    auto path = storagePath();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Unable to load custom prefs" << path << file.errorString();
        return;
    }
    __customData = QJsonDocument::fromJson(file.readAll()).object();
}

void save()
{
    auto path = storagePath();
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << "Unable to save custom prefs" << path << file.errorString();
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

void CustomPrefs::load()
{
    ::load();
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

