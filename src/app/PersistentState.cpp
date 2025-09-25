#include "PersistentState.h"

#include "helpers/OriWindows.h"
#include "tools/OriSettings.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QWidget>

//--------------------------------------------------------------------------------
//                               PersistentState
//--------------------------------------------------------------------------------

namespace PersistentState {

QString stateFileName(const char* id)
{
    Ori::Settings s;
    return s.settings()->fileName().section('.', 0, -2) % '.' % id % QLatin1String(".json");
}

bool createStateDir(const QString &stateFileName)
{
    QDir dir = QFileInfo(stateFileName).dir();
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            qWarning() << "Failed to create settings directory" << dir.path();
            return false;
        }
    }
    return true;
}

QJsonObject load(const char* id)
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

void save(const char *id, const QJsonObject& root)
{
    auto fileName = stateFileName(id);
    if (!createStateDir(fileName))
        return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << id << "failed to save state" << fileName << file.errorString();
        return;
    }
    QTextStream(&file) << QJsonDocument(root).toJson();
}

void storeWindowGeometry(const char *id, QWidget* w)
{
    QJsonObject state;
    storeWindowGeometry(state, w);
    save(id, state);
}

void restoreWindowGeometry(const char *id, QWidget* w, const QSize& defSize)
{
    restoreWindowGeometry(load(id), w, defSize);
}

void storeWindowGeometry(QJsonObject& s, QWidget* w)
{
    auto g = w->geometry();
    s[QLatin1String("window")] = QJsonObject({
        { QStringLiteral("left"), g.left() },
        { QStringLiteral("top"), g.top() },
        { QStringLiteral("width"), g.width() },
        { QStringLiteral("height"), g.height() },
        { QStringLiteral("maximized"), w->isMaximized() },
    });
}

void restoreWindowGeometry(const QJsonObject& s, QWidget* w, const QSize& defSize)
{
    auto o = s[QLatin1String("window")].toObject();
    Ori::Wnd::setGeometry(w, QRect(
        o[QLatin1String("left")].toInt(),
        o[QLatin1String("top")].toInt(),
        o[QLatin1String("width")].toInt(),
        o[QLatin1String("height")].toInt()),
        o[QLatin1String("maximized")].toBool(), defSize);
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
        load();
        auto watcher = new QFileSystemWatcher({fileName}, this);
        connect(watcher, &QFileSystemWatcher::fileChanged, this, &Storage::onFileChanged);
    }

    void load()
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
        needSave = false;
        selfSaved = true;

        if (!PersistentState::createStateDir(fileName))
            return;
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
            qWarning() << "Unable to save recent data" << fileName << file.errorString();
            return;
        }
        QTextStream(&file) << QJsonDocument(data).toJson();
    }

    void onFileChanged()
    {
        if (timerStarted)
            return;
        timerStarted = true;
        // There could be several changed signals from the same save operation
        QTimer::singleShot(200, this, &Storage::onReloadTimeout);
    }

    void onReloadTimeout()
    {
        timerStarted = false;
        if (selfSaved)
        {
            selfSaved = false;
            return;
        }
        load();
    }

    QJsonObject data;
    QString fileName;
    int pendingCount = 0;
    bool needSave = false;
    bool selfSaved = false;
    bool timerStarted = false;
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
            { QStringLiteral("width"), size.width() },
            { QStringLiteral("height"), size.height() }
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
