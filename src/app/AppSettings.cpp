#include "AppSettings.h"

#include "tools/OriSettings.h"

#include <QDebug>
#include <QFileSystemWatcher>
#include <QPen>
#include <QTimer>

namespace Z {
namespace Dlg {
    bool editAppSettings(Ori::Optional<int> currentrPageId); // AppSettingDialog.cpp
}}

//------------------------------------------------------------------------------
//                             IAppSettingsListener
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
//                              AppSettings
//------------------------------------------------------------------------------

#define LOAD(option, type)\
    option = s.settings()->value(QStringLiteral(#option)).to ## type()

#define LOAD_DEF(option, type, default_value)\
    option = s.settings()->value(QStringLiteral(#option), default_value).to ## type()

#define SAVE(option)\
    s.settings()->setValue(QStringLiteral(#option), option)

Q_GLOBAL_STATIC(AppSettings, __instance);

AppSettings& AppSettings::instance()
{
    return *__instance;
}

AppSettings::AppSettings() : QObject()
{
    load();
}

void AppSettings::load()
{
    Ori::Settings s;

    if (!_watcher)
    {
        _watcher = new QFileSystemWatcher({ s.settings()->fileName() }, this);
        _watcher->connect(_watcher, &QFileSystemWatcher::fileChanged, this, &AppSettings::onFileChanged);
    }

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
    LOAD_DEF(useOnlineHelp, Bool, false);

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
    LOAD_DEF(showImagUnitAsJ, Bool, false);
    LOAD_DEF(showImagUnitAtEnd, Bool, true);

    s.beginGroup("Units");
    defaultUnitBeamRadius = Z::Units::findByAlias(s.settings()->value("defaultUnitBeamRadius").toString(), Z::Units::mkm());
    defaultUnitFrontRadius = Z::Units::findByAlias(s.settings()->value("defaultUnitFrontRadius").toString(), Z::Units::m());
    defaultUnitAngle = Z::Units::findByAlias(s.settings()->value("defaultUnitAngle").toString(), Z::Units::deg());
}

void AppSettings::save()
{
    _selfSaved = true;

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
    SAVE(useOnlineHelp);

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
    SAVE(showImagUnitAsJ);
    SAVE(showImagUnitAtEnd);

    s.beginGroup("Units");
    s.settings()->setValue("defaultUnitBeamRadius", defaultUnitBeamRadius->alias());
    s.settings()->setValue("defaultUnitFrontRadius", defaultUnitFrontRadius->alias());
    s.settings()->setValue("defaultUnitAngle", defaultUnitAngle->alias());
}

bool AppSettings::edit(Ori::Optional<int> currentPageId)
{
    int old_numberPrecisionData = numberPrecisionData;

    bool result = Z::Dlg::editAppSettings(currentPageId);
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

QPen AppSettings::elemBoundMarkersPen() const
{
    return QPen(Qt::magenta, 1, Qt::DashLine);
}

QPen AppSettings::stabBoundMarkerPen() const
{
    return QPen(Qt::magenta, 1, Qt::DashLine);
}

QPen AppSettings::cursorPen() const
{
    return QPen(QColor::fromRgb(80, 80, 255));
}

QPen AppSettings::graphPenT() const
{
    return QPen(Qt::darkGreen);
}

QPen AppSettings::graphPenS() const
{
    return QPen(Qt::red);
}

void AppSettings::setElemBoundMarkersPen(const QPen& pen)
{

}

void AppSettings::setStabBoundMarkerPen(const QPen& pen)
{

}

void AppSettings::setCursorPen(const QPen& pen)
{

}

void AppSettings::setGraphPenT(const QPen& pen)
{

}

void AppSettings::setGraphPenS(const QPen& pen)
{

}

QStringList AppSettings::loadMruItems() const
{
    Ori::Settings s;
    s.beginGroup("States");
    return s.settings()->value("mru").toStringList();
}

void AppSettings::saveMruItems(const QStringList& items)
{
    _selfSaved = true;
    Ori::Settings s;
    s.beginGroup("States");
    s.settings()->setValue("mru", items);
}

void AppSettings::onFileChanged()
{
    if (_timerStarted)
        return;
    _timerStarted = true;
    // There could be several changed signals from the same save operation
    QTimer::singleShot(500, this, &AppSettings::onReloadTimeout);
}

void AppSettings::onReloadTimeout()
{
    _timerStarted = false;
    if (_selfSaved)
    {
        _selfSaved = false;
        return;
    }

    int old_numberPrecisionData = numberPrecisionData;

    load();

    notify(&IAppSettingsListener::settingsChanged);
    if (old_numberPrecisionData != numberPrecisionData)
        notify(&IAppSettingsListener::optionChanged, AppSettingsOptions::numberPrecisionData);
}
