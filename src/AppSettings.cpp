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
    option = s.settings()->value(QStringLiteral(#option)).to ## type()

#define LOAD_DEF(option, type, default_value)\
    option = s.settings()->value(QStringLiteral(#option), default_value).to ## type()

#define SAVE(option)\
    s.settings()->setValue(QStringLiteral(#option), option)

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

    s.beginGroup("Debug");
    LOAD_DEF(showProtocolAtStart, Bool, false);

    s.beginGroup("Plot");
    LOAD_DEF(plotSafeMargins, Double, 0.01);
    LOAD_DEF(plotZoomStep, Double, 0.1);
}

void Settings::save()
{
    Ori::Settings s;

    s.beginGroup("View");
    SAVE(smallToolbarImages);
    SAVE(showBackground);
    SAVE(useNativeMenuBar);
    SAVE(useSystemDialogs);

    s.beginGroup("Options");
    SAVE(editNewElem);
    SAVE(elemAutoLabel);

    s.beginGroup("Debug");
    SAVE(showProtocolAtStart);

    s.beginGroup("Plot");
    SAVE(plotSafeMargins);
    SAVE(plotZoomStep);
}

bool Settings::edit(class QWidget *parent)
{
    bool result = Z::Dlg::appConfig(parent);
    if (result)
        NOTIFY_LISTENERS(settingsChanged);
    return result;
}

