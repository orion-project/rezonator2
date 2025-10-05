#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include "../core/Units.h"

#include "core/OriTemplates.h"

#include <QDate>
#include <QMap>
#include <QObject>
#include <QPen>
#include <QSize>

QT_BEGIN_NAMESPACE
class QFileSystemWatcher;
QT_END_NAMESPACE

enum class AppSettingsOption { NumberPrecisionData, DefaultPenFormat };
enum class UpdateCheckInterval { None, Daily, Weekly, Monthly };

class IAppSettingsListener
{
public:
    IAppSettingsListener();
    virtual ~IAppSettingsListener();

    virtual void settingsChanged() {}
    virtual void optionChanged(AppSettingsOption option) { Q_UNUSED(option) }
};

class AppSettings : public QObject, public Ori::Notifier<IAppSettingsListener>
{
public:
    enum PenKind { PenGraphT, PenGraphS, PenCursor, PenStabBound, PenElemBound };

    AppSettings();

    static AppSettings& instance();

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
    bool useOnlineHelp;          ///< Navigate to online help instead of opening Assistant

    bool layoutExportTransparent; ///< Use transparent background in exported images of layout.

    bool exportAsCsv; ///< Write data in CSV format (otherwise, tab-separated text).
    bool exportColumnHeaders; ///< Write column headers in the first line.
    bool exportSystemLocale; ///< Use system locale (otherwise, use C-locale).
    bool exportTransposed; ///< Write columns as rows.
    int exportNumberPrecision; ///< Number precision for export.
    bool exportHideCursor; ///< Hide cursor lines when exporting plot image.

    int numberPrecisionData; ///< Number precision for value formatting (graph data, etc).
    bool showImagUnitAsJ;
    bool showImagUnitAtEnd;
    
    UpdateCheckInterval updateCheckInterval = UpdateCheckInterval::Weekly;
    QDate updateLastCheckDate;
    int updateCheckDelayMs = 300;

    Z::Unit defaultUnitFrontRadius = Z::Units::none();
    Z::Unit defaultUnitBeamRadius = Z::Units::none();
    Z::Unit defaultUnitAngle = Z::Units::none();

    bool isDevMode = false;

    void load();
    void save();
    void saveUpdateChecked();

    enum { PageGeneral, PageGeneral2, PageView, PageLayout, PageUnits, PageExport, PageCalc };
    bool edit(Ori::Optional<int> currentPageId = Ori::Optional<int>());

    QSize toolbarIconSize() const;

    QPen pen(PenKind kind) const { return _pens[kind]; }
    void setPen(PenKind kind, const QPen& pen);

    QStringList loadMruItems() const;
    void saveMruItems(const QStringList& items);

private:
    friend struct AppSettingsNotifier;

    int toolbarIconSizeSmall;
    int toolbarIconSizeBig;

    QFileSystemWatcher* _watcher = nullptr;
    bool _timerStarted = false;
    bool _selfSaved = false;
    QMap<PenKind, QPen> _pens;

    void onFileChanged();
    void onReloadTimeout();
};

#endif // APP_SETTINGS_H
