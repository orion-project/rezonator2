#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include "core/OriTemplates.h"

class SettingsListener
{
public:
    SettingsListener();
    ~SettingsListener();

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
    double plotSafeMargins;   ///< Extra space around graphs when plot limits are fitted to graphs.
                              ///< Set as fraction of limits range: `(max - min) * plotSafeMargins`.
    double plotZoomStep;      ///< Step of zooming for commands Zoom-in / Zoom-out.
                              ///< Set as fraction of limits range: `(max - min) * plotZoomStep`.

    void load();
    void save();
    bool edit(class QWidget *parent);

private:
    Settings() {}

    friend class Singleton<Settings>;
};

#endif // APP_SETTINGS_H
