#include "AppSettings.h"
#include "tools/OriSettings.h"

#ifndef DLG_APP_CONFIG
#define DLG_APP_CONFIG
namespace Z {
namespace Dlg {
    bool appConfig(class QWidget *parent); // ConfigDialog.cpp
}}
#endif


//------------------------------------------------------------------------------
//                              SettingsListener
//------------------------------------------------------------------------------

SettingsListener::SettingsListener()
{
    Settings::instance().registerListener(this);
}

SettingsListener::~SettingsListener()
{
    Settings::instance().unregisterListener(this);
}


//------------------------------------------------------------------------------
//                               Settings
//------------------------------------------------------------------------------

#define LOAD(option, type)\
    option = s.settings()->value(QStringLiteral(#option)).to ## type();

#define LOAD_DEF(option, type, default_value)\
    option = s.settings()->value(QStringLiteral(#option), default_value).to ## type();

#define SAVE(option)\
    s.settings()->setValue(QStringLiteral(#option), option);

Settings::Settings()
{
}

void Settings::load()
{
    Ori::Settings s;

    s.beginGroup("View");
    LOAD_DEF(smallToolbarImages, Bool, false);
    LOAD_DEF(showBackground, Bool, true);
    LOAD_DEF(useNativeMenuBar, Bool, true);
    LOAD_DEF(useSystemDialogs, Bool, true);

    s.beginGroup("Options");
    LOAD_DEF(editNewElem, Bool, true);
    LOAD_DEF(elemAutoLabel, Bool, true);

    s.beginGroup("States");
    LOAD(schemaFileDlgOpenPath, String);
    LOAD(schemaFileDlgOpenFilter, String);
    LOAD(schemaFileDlgSavePath, String);
    LOAD(schemaFileDlgSaveFilter, String);

    s.beginGroup("Debug");
    LOAD_DEF(showProtocolAtStart, Bool, false);
}

void Settings::save()
{
    Ori::Settings s;

    s.beginGroup("View");
    SAVE(smallToolbarImages)
    SAVE(showBackground)
    SAVE(useNativeMenuBar)
    SAVE(useSystemDialogs)

    s.beginGroup("Options");
    SAVE(editNewElem)
    SAVE(elemAutoLabel)

    s.beginGroup("States");
    SAVE(schemaFileDlgOpenPath)
    SAVE(schemaFileDlgOpenFilter)
    SAVE(schemaFileDlgSavePath)
    SAVE(schemaFileDlgSaveFilter)

    s.beginGroup("Debug");
    SAVE(showProtocolAtStart)
}

bool Settings::edit(class QWidget *parent)
{
    bool result = Z::Dlg::appConfig(parent);
    if (result)
        NOTIFY_LISTENERS(settingsChanged);
    return result;
}

const QString& Settings::schemaOpenPath() const
{
    return schemaFileDlgOpenPath.isEmpty()? schemaFileDlgSavePath: schemaFileDlgOpenPath;
}

const QString& Settings::schemaSavePath() const
{
    return schemaFileDlgSavePath.isEmpty()? schemaFileDlgOpenPath: schemaFileDlgSavePath;
}

