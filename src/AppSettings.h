#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include "core/OriTemplates.h"
#include <QSize>

class SettingsListener
{
public:
    SettingsListener();
    virtual ~SettingsListener();

    virtual void settingsChanged() {}
};

class Settings :
        public Singleton<Settings>,
        public Notifier<SettingsListener>
{
public:
    bool editNewElem;         ///< Open element properties dialog after element has been created.
    bool smallToolbarImages;  ///< Use small toolbar images (16x16 instead of 24x24).
    bool showBackground;      ///< Show background image in main window.
    bool useSystemDialogs;    ///< Use native open/save file dialogs.
    bool elemAutoLabel;       ///< Automatically generate labels for new elements.
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

    void load();
    void save();
    bool edit(class QWidget *parent);

    QSize toolbarIconSize() const;

private:
    Settings() {}

    friend class Singleton<Settings>;

    int toolbarIconSizeSmall;
    int toolbarIconSizeBig;
};

#endif // APP_SETTINGS_H
