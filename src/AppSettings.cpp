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

    s.beginGroup("Export");
    LOAD_DEF(exportAsCsv, Bool, false);
    LOAD_DEF(exportColumnHeaders, Bool, false);
    LOAD_DEF(exportSystemLocale, Bool, false);
    LOAD_DEF(exportTransposed, Bool, false);
    LOAD_DEF(exportNumberPrecision, Int, 6);
    LOAD_DEF(exportHideCursor, Bool, false);

    s.beginGroup("Format");
    LOAD_DEF(numberPrecisionData, Int, 6);

    s.beginGroup("Units");
    defaultUnitBeamRadius = Z::Units::findByAlias(s.settings()->value("defaultUnitBeamRadius").toString(), Z::Units::mkm());
    defaultUnitFrontRadius = Z::Units::findByAlias(s.settings()->value("defaultUnitFrontRadius").toString(), Z::Units::m());
    defaultUnitAngle = Z::Units::findByAlias(s.settings()->value("defaultUnitAngle").toString(), Z::Units::deg());

    s.beginGroup("Calc");
    LOAD_DEF(calcTablesMediumEnds, Bool, false);
    LOAD_DEF(calcTablesEmptySpaces, Bool, false);
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

    s.beginGroup("Export");
    SAVE(exportAsCsv);
    SAVE(exportColumnHeaders);
    SAVE(exportSystemLocale);
    SAVE(exportTransposed);
    SAVE(exportNumberPrecision);
    SAVE(exportHideCursor);

    s.beginGroup("Format");
    SAVE(numberPrecisionData);

    s.beginGroup("Units");
    s.settings()->setValue("defaultUnitBeamRadius", defaultUnitBeamRadius->alias());
    s.settings()->setValue("defaultUnitFrontRadius", defaultUnitFrontRadius->alias());
    s.settings()->setValue("defaultUnitAngle", defaultUnitAngle->alias());

    s.beginGroup("Calc");
    SAVE(calcTablesMediumEnds);
    SAVE(calcTablesEmptySpaces);
}

bool AppSettings::edit(class QWidget *parent)
{
    int old_numberPrecisionData = numberPrecisionData;

    bool result = Z::Dlg::editAppSettings(parent);
    if (result)
    {
        notify(&IAppSettingsListener::settingsChanged);

        if (old_numberPrecisionData != numberPrecisionData)
            notify(&IAppSettingsListener::optionChanged, AppSettingsOptions::numberPrecisionData);
    }
    return result;
}

QSize AppSettings::toolbarIconSize() const
{
    return smallToolbarImages ?
        QSize(toolbarIconSizeSmall, toolbarIconSizeSmall) :
        QSize(toolbarIconSizeBig, toolbarIconSizeBig);
}
