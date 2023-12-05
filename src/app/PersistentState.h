#ifndef CUSTOM_PREFS_H
#define CUSTOM_PREFS_H

#include "../core/Units.h"

#include <QJsonObject>
#include <QSize>

namespace PersistentState {

QJsonObject load(const QString& spec);
void save(const QString& spec, const QJsonObject& root);

void saveWindowSize(QJsonObject& root, QWidget* wnd);
void loadWindowSize(const QJsonObject& root, QWidget* wnd, int defaultW, int defaultH);

} // namespace PersistentState

class CustomPrefs
{
public:
    CustomPrefs() = delete;

    static void load(const QString& appConfigFile);

    static void setRecentDim(const QString& key, Z::Dim dim);
    static Z::Dim recentDim(const QString& key);
    static void setRecentUnit(const QString& key, Z::Unit unit);
    static Z::Unit recentUnit(const QString& key, Z::Dim dim);
    static void setRecentDir(const QString& key, const QString& dirOrFile);
    static QString recentDir(const QString& key, const QString& defaultDir = QString());
    static void setRecentStr(const QString& key, const QString& value);
    static QString recentStr(const QString& key, const QString& defaultStr = QString());
    static void setRecentObj(const QString& key, const QJsonObject& obj);
    static QJsonObject recentObj(const QString& key);
    static QSize recentSize(const QString& key, const QSize &defaultSize = QSize());
    static void setRecentSize(const QString& key, const QSize& size);
};

#endif // CUSTOM_PREFS_H
