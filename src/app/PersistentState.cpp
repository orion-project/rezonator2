#include "PersistentState.h"

#include "tools/OriSettings.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QWidget>

//--------------------------------------------------------------------------------
//                               PersistentState
//--------------------------------------------------------------------------------

namespace PersistentState {

QString stateFileName(const QString& id)
{
    Ori::Settings s;
    return s.settings()->fileName().section('.', 0, -2) % '.' % id % QLatin1String(".json");
}

QJsonObject load(const QString& id)
{
    QJsonObject root;
    auto fileName = stateFileName(id);
    QFile file(fileName);
    if (file.exists())
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            qWarning() << id << "failed to load state" << fileName << file.errorString();
        else
        {
            QJsonParseError error;
            root = QJsonDocument::fromJson(file.readAll(), &error).object();
            if (error.error != QJsonParseError::NoError)
                qWarning() << id << "failed to load state" << error.errorString();
        }
    }
    return root;
}

void save(const QString& id, const QJsonObject& root)
{
    auto fileName = stateFileName(id);
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << id << "failed to save state" << fileName << file.errorString();
        return;
    }
    QTextStream(&file) << QJsonDocument(root).toJson();
}

void saveWindowSize(QJsonObject& root, QWidget* wnd)
{
    root["window_width"] = wnd->width();
    root["window_height"] = wnd->height();
}

void loadWindowSize(const QJsonObject& root, QWidget* wnd, int defaultW, int defaultH)
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

} // namespace PersistentState

//--------------------------------------------------------------------------------
//                               RecentData
//--------------------------------------------------------------------------------

namespace RecentData {

class Storage : public QObject
{
public:
    Storage(const QString& fileName): QObject(), fileName(fileName)
    {
        QFile file(fileName);
        if (!file.exists())
            return;
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qWarning() << "Unable to load recent data" << fileName << file.errorString();
            return;
        }
        QJsonParseError error;
        data = QJsonDocument::fromJson(file.readAll(), &error).object();
        if (error.error != QJsonParseError::NoError)
            qWarning() << "Unable to load recent data" << fileName << error.errorString();
    }

    void save()
    {
        if (pendingCount > 0)
        {
            needSave = true;
            return;
        }

        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
            qWarning() << "Unable to save recent data" << fileName << file.errorString();
            return;
        }
        QTextStream(&file) << QJsonDocument(data).toJson();
        needSave = false;
    }

    QJsonObject data;
    QString fileName;
    int pendingCount = 0;
    bool needSave = false;
};

Q_GLOBAL_STATIC_WITH_ARGS(Storage, __storage, { PersistentState::stateFileName("prefs") });

PendingSave::PendingSave()
{
    __storage->pendingCount++;
}

PendingSave::~PendingSave()
{
    __storage->pendingCount--;
    if (__storage->pendingCount <= 0 && __storage->needSave)
        __storage->save();
}

QString getStr(const char *key, const QString& defaultStr)
{
    QString s = __storage->data[key].toString(defaultStr);
    return s;
}

void setStr(const char *key, const QString& value)
{
    if (__storage->data[QLatin1String(key)].toString() != value)
    {
        __storage->data[QLatin1String(key)] = value;
        __storage->save();
    }
}

QString getDir(const char *key, const QString& defaultDir)
{
    QString dir = getStr(key, defaultDir);
    return (dir.isEmpty() || !QDir(dir).exists()) ? QDir::currentPath() : dir;
}

void setDir(const char *key, const QString& dirOrFile)
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
        setStr(key, dir);
}

QSize getSize(const char *key, const QSize& defaultSize)
{
    if (!__storage->data.contains(QLatin1String(key)))
        return defaultSize;

    auto json = __storage->data[QLatin1String(key)].toObject();
    return {
        json[QLatin1String("width")].toInt(),
        json[QLatin1String("height")].toInt(),
    };
}

void setSize(const char *key, const QSize& size)
{
    if (getSize(key) != size)
    {
        __storage->data[key] = QJsonObject({
            { "width", size.width() },
            { "height", size.height() }
        });
        __storage->save();
    }
}

Z::Dim getDim(const char *key)
{
    return Z::Dims::findByAliasOrNone(__storage->data[QLatin1String(key)].toString());
}

void setDim(const char *key, Z::Dim dim)
{
    __storage->data[QLatin1String(key)] = dim->alias();
    __storage->save();
}

Z::Unit getUnit(const char *key, Z::Dim dim)
{
    auto unitByDim = __storage->data[QLatin1String(key)].toObject();
    return dim->unitByAliasOrSi(unitByDim[dim->alias()].toString());
}

void setUnit(const char *key, Z::Unit unit)
{
    auto unitByDim = __storage->data[QLatin1String(key)].toObject();
    unitByDim[Z::Units::guessDim(unit)->alias()] = unit->alias();
    __storage->data[QLatin1String(key)] = unitByDim;
    __storage->save();
}

void setObj(const char *key, const QJsonObject& obj)
{
    if (!__storage->data.contains(QLatin1String(key)) || __storage->data[QLatin1String(key)].toObject() != obj)
    {
        __storage->data[QLatin1String(key)] = obj;
        __storage->save();
    }
}

QJsonObject getObj(const char *key)
{
    return __storage->data[QLatin1String(key)].toObject();
}

} // namespace RecentData
