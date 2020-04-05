#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include "core/Units.h"

#include "core/OriTemplates.h"

#include <QSize>

class IAppSettingsListener
{
public:
    IAppSettingsListener();
    virtual ~IAppSettingsListener();

    virtual void settingsChanged() {}
};

class AppSettings :
        public Ori::Singleton<AppSettings>,
        public Ori::Notifier<IAppSettingsListener>
{
public:
    bool editNewElem;         ///< Open element properties dialog after element has been created.
    bool smallToolbarImages;  ///< Use small toolbar images (16x16 instead of 24x24).
    bool showBackground;      ///< Show background image in main window.
    bool useSystemDialogs;    ///< Use native open/save file dialogs.
    bool elemAutoLabel;       ///< Automatically generate labels for new elements.
    bool elemAutoLabelPasted; ///< Automatically generate labels for pasted elements.
    bool pumpAutoLabel;       ///< Automatically generate labels for new pumps.
    bool useNativeMenuBar;    ///< Use menu bar specfic to Ubuntu Unity or MacOS (on sceern's top).
    bool showProtocolAtStart; ///< Open protocol window just after application started.
    double plotSafeMarginsPercentX; ///< Extra space around graphs when plot limits are fitted to graphs.
                                    ///< Set as fraction of limits range: `(max - min) * plotSafeMargins/100`.
    double plotSafeMarginsPercentY; ///< Extra space around graphs when plot limits are fitted to graphs.
                                    ///< Set as fraction of limits range: `(max - min) * plotSafeMargins/100`.
    double plotZoomStepPercentX; ///< Step of zooming for commands Zoom-in / Zoom-out.
                                 ///< Set as fraction of limits range: `(max - min) * plotZoomStep/100`.
    double plotZoomStepPercentY; ///< Step of zooming for commands Zoom-in / Zoom-out.
                                 ///< Set as fraction of limits range: `(max - min) * plotZoomStep/100`.
    int plotNumberPrecision;  ///< Number of decimal digits used to format axes labels.
    QString defaultTripType;  ///< New schemas should be created having this trip-type.
    bool showStartWindow;     ///< Show StartWindow after app started instead of opening ProjectWindow.
    int mruSchemaCount;          ///< Max items count in recently opened schemas list.
    double adjusterIncrement;    ///< Increment value used for new adjusters.
    double adjusterMultiplier;   ///< Multiplier value used for new adjusters.
    bool showCustomElemLibrary;  ///< Load Custom Element Library into Elements Catalog.
    bool showPythonMatrices;     ///< Show Python code for matrices in info function windows.
    bool skipFuncWindowsLoading; ///< Don't load function windows when opening schema.

    bool layoutExportTransparent; ///< Use transparent background in exported images of layout.

    bool exportGraphDataAsCsv; ///< Write data in CSV format (otherwise, tab-separated text).
    bool exportColumnHeaders; ///< Write column headers in the first line.
    bool useSystemDecimalSeparator; ///< Use system decimal separator (otherwise, use point).

    Z::Unit defaultUnitFrontRadius = Z::Units::none();
    Z::Unit defaultUnitBeamRadius = Z::Units::none();
    Z::Unit defaultUnitAngle = Z::Units::none();

    bool isDevMode = false;

    void load();
    void save();
    bool edit(class QWidget *parent);

    QSize toolbarIconSize() const;

private:
    AppSettings() {}

    friend class Ori::Singleton<AppSettings>;

    int toolbarIconSizeSmall;
    int toolbarIconSizeBig;
};

#endif // APP_SETTINGS_H
