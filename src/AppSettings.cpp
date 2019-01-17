#include "AppSettings.h"
#include "tools/OriSettings.h"

#ifndef DLG_APP_CONFIG
#define DLG_APP_CONFIG
namespace Z {
namespace Dlg {
    bool editAppSettings(class QWidget *parent); // ConfigDialog.cpp
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
    LOAD_DEF(toolbarIconSizeSmall, Int, 16);
    LOAD_DEF(toolbarIconSizeBig, Int, 24);

    s.beginGroup("Options");
    LOAD_DEF(editNewElem, Bool, true);
    LOAD_DEF(elemAutoLabel, Bool, true);
    LOAD(defaultTripType, String);
    LOAD_DEF(showStartWindow, Bool, true);

    s.beginGroup("Debug");
    LOAD_DEF(showProtocolAtStart, Bool, false);

    s.beginGroup("Plot");
    LOAD_DEF(plotSafeMarginsPercentX, Double, 1);
    LOAD_DEF(plotSafeMarginsPercentY, Double, 5);
    LOAD_DEF(plotZoomStepPercentX, Double, 1);
    LOAD_DEF(plotZoomStepPercentY, Double, 1);
    LOAD_DEF(plotNumberPrecision, Int, 10);
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
    SAVE(defaultTripType);
    SAVE(showStartWindow);

    s.beginGroup("Debug");
    SAVE(showProtocolAtStart);

    s.beginGroup("Plot");
    SAVE(plotSafeMarginsPercentX);
    SAVE(plotSafeMarginsPercentY);
    SAVE(plotZoomStepPercentX);
    SAVE(plotZoomStepPercentY);
    SAVE(plotNumberPrecision);
}

bool Settings::edit(class QWidget *parent)
{
    bool result = Z::Dlg::editAppSettings(parent);
    if (result)
        notify(&SettingsListener::settingsChanged);
    return result;
}

QSize Settings::toolbarIconSize() const
{
    return smallToolbarImages ?
        QSize(toolbarIconSizeSmall, toolbarIconSizeSmall) :
        QSize(toolbarIconSizeBig, toolbarIconSizeBig);
}
