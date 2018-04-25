#ifndef CUSTOM_PREFS_H
#define CUSTOM_PREFS_H

#include "core/Units.h"

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
};

#endif // CUSTOM_PREFS_H
