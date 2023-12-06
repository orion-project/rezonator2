#ifndef CUSTOM_PREFS_H
#define CUSTOM_PREFS_H

#include "../core/Units.h"

#include <QMetaEnum>
#include <QJsonObject>
#include <QSize>

namespace PersistentState {

QJsonObject load(const char *id);
void save(const char *id, const QJsonObject& root);

void saveWindowSize(QJsonObject& root, QWidget* wnd);
void loadWindowSize(const QJsonObject& root, QWidget* wnd, int defaultW, int defaultH);

} // namespace PersistentState


namespace RecentData {

struct PendingSave
{
    PendingSave();
    ~PendingSave();
};

QString getStr(const char* key, const QString& defaultStr = QString());
void setStr(const char* key, const QString& value);

QString getDir(const char* key, const QString& defaultDir = QString());
void setDir(const char* key, const QString& dirOrFile);

QSize getSize(const char* key, const QSize &defaultSize = QSize());
void setSize(const char* key, const QSize& size);

Z::Dim getDim(const char* key);
void setDim(const char* key, Z::Dim dim);
Z::Unit getUnit(const char* key, Z::Dim dim);
void setUnit(const char* key, Z::Unit unit);

QJsonObject getObj(const char* key);
void setObj(const char* key, const QJsonObject& obj);

template <typename TEnum> TEnum getEnum(const char* key, TEnum defaultValue)
{
    QString str = getStr(key);
    bool ok;
    int res = QMetaEnum::fromType<TEnum>().keyToValue(str.toLatin1().data(), &ok);
    return ok ? TEnum(res) : defaultValue;
}

template <typename TEnum> void setEnum(const char* key, TEnum value)
{
    setStr(key, QMetaEnum::fromType<TEnum>().key(value));
}

} // namespace RecentData

#endif // CUSTOM_PREFS_H
