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
    //int maxRecentFiles;       ///< Recent files list length.

    // states
    QString schemaFileDlgOpenPath, schemaFileDlgOpenFilter;
    QString schemaFileDlgSavePath, schemaFileDlgSaveFilter;

    void load();
    void save();
    bool edit(class QWidget *parent);

    const QString& schemaOpenPath() const;
    const QString& schemaSavePath() const;
    QString* schemaOpenFilter() { return &schemaFileDlgOpenFilter; }
    QString* schemaSaveFilter() { return &schemaFileDlgSaveFilter; }

private:
    Settings();

    friend class Singleton<Settings>;
};

#endif // APP_SETTINGS_H
