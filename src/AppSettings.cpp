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

IAppSettingsListener::IAppSettingsListener()
{
    AppSettings::instance().registerListener(this);
}

IAppSettingsListener::~IAppSettingsListener()
{
    AppSettings::instance().unregisterListener(this);
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

void AppSettings::load()
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
    LOAD_DEF(elemAutoLabelPasted, Bool, true);
    LOAD_DEF(pumpAutoLabel, Bool, true);
    LOAD(defaultTripType, String);
    LOAD_DEF(showStartWindow, Bool, true);
    LOAD_DEF(mruSchemaCount, Int, 16);
    LOAD_DEF(adjusterIncrement, Double, 1.0);
    LOAD_DEF(adjusterMultiplier, Double, 1.1);
    LOAD_DEF(showCustomElemLibrary, Bool, true);
    LOAD_DEF(showPythonMatrices, Bool, false);
    LOAD_DEF(skipFuncWindowsLoading, Bool, false);

    s.beginGroup("Debug");
    LOAD_DEF(showProtocolAtStart, Bool, false);

    s.beginGroup("Plot");
    LOAD_DEF(plotSafeMarginsPercentX, Double, 1);
    LOAD_DEF(plotSafeMarginsPercentY, Double, 5);
    LOAD_DEF(plotZoomStepPercentX, Double, 1);
    LOAD_DEF(plotZoomStepPercentY, Double, 1);
    LOAD_DEF(plotNumberPrecision, Int, 10);

    s.beginGroup("Layout");
    LOAD_DEF(layoutExportTransparent, Bool, false);

    s.beginGroup("Units");
    defaultUnitBeamRadius = Z::Units::findByAlias(s.settings()->value("defaultUnitBeamRadius").toString(), Z::Units::mkm());
    defaultUnitFrontRadius = Z::Units::findByAlias(s.settings()->value("defaultUnitFrontRadius").toString(), Z::Units::m());
    defaultUnitAngle = Z::Units::findByAlias(s.settings()->value("defaultUnitAngle").toString(), Z::Units::deg());
}

void AppSettings::save()
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
    SAVE(elemAutoLabelPasted);
    SAVE(pumpAutoLabel);
    SAVE(defaultTripType);
    SAVE(showStartWindow);
    SAVE(mruSchemaCount);
    SAVE(adjusterIncrement);
    SAVE(adjusterMultiplier);
    SAVE(showCustomElemLibrary);
    SAVE(showPythonMatrices);
    SAVE(skipFuncWindowsLoading);

    s.beginGroup("Debug");
    SAVE(showProtocolAtStart);

    s.beginGroup("Plot");
    SAVE(plotSafeMarginsPercentX);
    SAVE(plotSafeMarginsPercentY);
    SAVE(plotZoomStepPercentX);
    SAVE(plotZoomStepPercentY);
    SAVE(plotNumberPrecision);

    s.beginGroup("Layout");
    SAVE(layoutExportTransparent);

    s.beginGroup("Units");
    s.settings()->setValue("defaultUnitBeamRadius", defaultUnitBeamRadius->alias());
    s.settings()->setValue("defaultUnitFrontRadius", defaultUnitFrontRadius->alias());
    s.settings()->setValue("defaultUnitAngle", defaultUnitAngle->alias());
}

bool AppSettings::edit(class QWidget *parent)
{
    bool result = Z::Dlg::editAppSettings(parent);
    if (result)
        notify(&IAppSettingsListener::settingsChanged);
    return result;
}

QSize AppSettings::toolbarIconSize() const
{
    return smallToolbarImages ?
        QSize(toolbarIconSizeSmall, toolbarIconSizeSmall) :
        QSize(toolbarIconSizeBig, toolbarIconSizeBig);
}
