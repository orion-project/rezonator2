#ifndef CUSTOM_PREFS_H
#define CUSTOM_PREFS_H

#include "core/Units.h"

class CustomPrefs
{
public:
    CustomPrefs() = delete;

    static void load();

    static void setRecentDim(const QString& key, Z::Dim dim);
    static Z::Dim recentDim(const QString& key);
    static void setRecentUnit(const QString& key, Z::Unit unit);
    static Z::Unit recentUnit(const QString& key, Z::Dim dim);
};

#endif // CUSTOM_PREFS_H
