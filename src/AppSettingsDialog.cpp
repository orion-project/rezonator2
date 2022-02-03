#include "AppSettings.h"

#include "AppSettingsDialog.h"
#include "widgets/UnitWidgets.h"

#include "helpers/OriLayouts.h"
#include "widgets/OriOptionsGroup.h"

#include <QBoxLayout>
#include <QDebug>
#include <QFormLayout>
#include <QLabel>
#include <QIcon>
#include <QSpinBox>

using namespace Ori::Layouts;

namespace Z {
namespace Dlg {

bool editAppSettings(QWidget *parent)
{
    AppSettingsDialog dialog(parent);
    return dialog.exec() == QDialog::Accepted;
}

} // namespace Dlgs
} // namespace Z

namespace  {
class NumberPrecisionSpinBox : public QSpinBox
{
public:
    NumberPrecisionSpinBox() : QSpinBox()
    {
        setRange(1, 16);
        setFixedWidth(QFontMetrics(font()).boundingRect("00").width() * 4);
    }
};
}

//------------------------------------------------------------------------------
//                              ConfigDialog
//------------------------------------------------------------------------------

AppSettingsDialog::AppSettingsDialog(QWidget* parent) : Ori::Dlg::BasicConfigDialog(parent)
{
    pageListIconSize = QSize(48, 48);

    setObjectName("AppSettingsDialog");
    setTitleAndIcon(tr("Application Settings"), ":/window_icons/options");

    createPages({
                    createGeneralPage(),
                    createViewPage(),
                    createLayoutPage(),
                    createUnitsPage(),
                    createExportPage(),
                    createCalcPage(),
                });
}

QWidget* AppSettingsDialog::createGeneralPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Behavior"), ":/config_pages/general");

    _groupOptions = new Ori::Widgets::OptionsGroupV2(tr("Options"), {
        {"editNewElem", tr("Edit just created element")},
        {"elemAutoLabel", tr("Generate labels for created elements")},
        {"elemAutoLabelPasted", tr("Generate labels for pasted elements")},
        {"pumpAutoLabel", tr("Generate labels for new pumps")},
        {"showStartWindow", tr("Show start window when application runs")},
        {"showProtocolAtStart", tr("Show protocol window after application started")},
        {"showCustomElemLibrary", tr("Show custom elements in Elements Catalog")},
        {"showPythonMatrices", tr("Show Python code for matrices in info windows")},
        {"skipFuncWindowsLoading", tr("Don't load function windows when opening schema")},
    });

    page->add({_groupOptions, page->stretch()});
    return page;
}

QWidget* AppSettingsDialog::createViewPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Interface"), ":/config_pages/view");

    // group box "Options"
    _groupView = new Ori::Widgets::OptionsGroupV2(tr("Options"), {
        {"smallToolbarImages", tr("Use small toolbar images")},
        {"showBackground", tr("Show background image")},
        {"useNativeMenuBar", tr("Use native menu bar (restart required)")},
        {"useSystemDialogs", tr("Use system open/save file dialogs")},
    });

    _numberPrecisionData = new NumberPrecisionSpinBox;
    auto groupFormat = new QGroupBox(tr("Number format"));
    LayoutH({
        new QLabel(tr("Number precision for result formatting")), _numberPrecisionData
    }).useFor(groupFormat);

    page->add({_groupView, groupFormat, page->stretch()});
    return page;
}

QWidget* AppSettingsDialog::createLayoutPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Layout"), ":/config_pages/layout");

    // group box "Options"
    _groupLayoutExport = new Ori::Widgets::OptionsGroupV2(tr("Image export options"), {
        {"layoutExportTransparent", tr("Use transparent background")},
    });

    page->add({_groupLayoutExport, page->stretch()});
    return page;
}

QWidget* AppSettingsDialog::createUnitsPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Units"), ":/config_pages/units");

    _defaultUnitBeamRadius = new UnitComboBox(Z::Dims::linear());
    _defaultUnitFrontRadius = new UnitComboBox(Z::Dims::linear());
    _defaultUnitAngle = new UnitComboBox(Z::Dims::angular());

    auto unitsLayout = new QFormLayout;
    unitsLayout->setHorizontalSpacing(12);
    unitsLayout->addRow(tr("Beam radus"), _defaultUnitBeamRadius);
    unitsLayout->addRow(tr("Wavefront radius"), _defaultUnitFrontRadius);
    unitsLayout->addRow(tr("Beam angle"), _defaultUnitAngle);

    auto infoLabel = new QLabel(tr(
        "These units used for the presentation of function results "
        "when a function calculates the first time and no custom units selected yet."));
    infoLabel->setWordWrap(true);
    auto infoIcon = new QLabel;
    infoIcon->setPixmap(QIcon(":/toolbar/info").pixmap(24, 24));
    infoIcon->setFixedWidth(24);

    auto defUnitsGroup = new QGroupBox(tr("Default units"));
    LayoutV({
        unitsLayout,
        Space(12),
        LayoutH({
            LayoutV({infoIcon, Stretch()}),
            infoLabel
        })
    }).useFor(defUnitsGroup);

    page->add({defUnitsGroup, page->stretch()});
    return page;
}

QWidget* AppSettingsDialog::createExportPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Export"), ":/toolbar/save");

    _groupExportData = new Ori::Widgets::OptionsGroupV2(tr("Graph data export options"), {
        {"exportAsCsv", tr("Use CSV format (otherwise, use plain text format)")},
        {"exportSystemLocale", tr("Use system locale (otherwise, use C locale)")},
        {"exportTransposed", tr("Transposed mode (write values in row instead of column)")},
    });

    _exportNumberPrecision = new NumberPrecisionSpinBox;
    _groupExportData->addControls({
        LayoutH({new QLabel(tr("Number precision")), _exportNumberPrecision, Stretch()}).boxLayout(),
    });

    _groupExportPlot = new Ori::Widgets::OptionsGroupV2(tr("Plot image export options"), {
        {"exportHideCursor", tr("Hide cursor lines when save, copy, or print image")}
    });

    page->add({_groupExportData, _groupExportPlot, page->stretch()});
    return page;
}

QWidget* AppSettingsDialog::createCalcPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Calcs"), ":/toolbar/options");

    _groupCalcOpts = new Ori::Widgets::OptionsGroupV2(tr("Options"), {
        {"calcTablesMediumEnds", tr("Calculate at medium ends in table functions")},
        {"calcTablesEmptySpaces", tr("Calculate in empty spaces in table functions")},
    });

    page->add({_groupCalcOpts, page->stretch()});
    return page;
}

void AppSettingsDialog::populate()
{
    AppSettings &settings = AppSettings::instance();

    // options
    _groupOptions->setOption("editNewElem", settings.editNewElem);
    _groupOptions->setOption("elemAutoLabel", settings.elemAutoLabel);
    _groupOptions->setOption("elemAutoLabelPasted", settings.elemAutoLabelPasted);
    _groupOptions->setOption("pumpAutoLabel", settings.pumpAutoLabel);
    _groupOptions->setOption("showStartWindow", settings.showStartWindow);
    _groupOptions->setOption("showProtocolAtStart", settings.showProtocolAtStart);
    _groupOptions->setOption("showCustomElemLibrary", settings.showCustomElemLibrary);
    _groupOptions->setOption("showPythonMatrices", settings.showPythonMatrices);
    _groupOptions->setOption("skipFuncWindowsLoading", settings.skipFuncWindowsLoading);

    // view
    _groupView->setOption("smallToolbarImages", settings.smallToolbarImages);
    _groupView->setOption("showBackground", settings.showBackground);
    _groupView->setOption("useNativeMenuBar", settings.useNativeMenuBar);
    _groupView->setOption("useSystemDialogs", settings.useSystemDialogs);
    _numberPrecisionData->setValue(settings.numberPrecisionData);

    // layout
    _groupLayoutExport->setOption("layoutExportTransparent", settings.layoutExportTransparent);

    // units
    _defaultUnitBeamRadius->setSelectedUnit(settings.defaultUnitBeamRadius);
    _defaultUnitFrontRadius->setSelectedUnit(settings.defaultUnitFrontRadius);
    _defaultUnitAngle->setSelectedUnit(settings.defaultUnitAngle);

    // export
    _groupExportData->setOption("exportAsCsv", settings.exportAsCsv);
    _groupExportData->setOption("exportSystemLocale", settings.exportSystemLocale);
    _groupExportData->setOption("exportTransposed", settings.exportTransposed);
    _exportNumberPrecision->setValue(settings.exportNumberPrecision);
    _groupExportPlot->setOption("exportHideCursor", settings.exportHideCursor);

    // calc
    _groupCalcOpts->setOption("calcTablesMediumEnds", settings.calcTablesMediumEnds);
    _groupCalcOpts->setOption("calcTablesEmptySpaces", settings.calcTablesEmptySpaces);
}

bool AppSettingsDialog::collect()
{
    AppSettings &settings = AppSettings::instance();

    // options
    settings.editNewElem = _groupOptions->option("editNewElem");
    settings.elemAutoLabel = _groupOptions->option("elemAutoLabel");
    settings.elemAutoLabelPasted = _groupOptions->option("elemAutoLabelPasted");
    settings.pumpAutoLabel = _groupOptions->option("pumpAutoLabel");
    settings.showStartWindow = _groupOptions->option("showStartWindow");
    settings.showProtocolAtStart = _groupOptions->option("showProtocolAtStart");
    settings.showCustomElemLibrary = _groupOptions->option("showCustomElemLibrary");
    settings.showPythonMatrices = _groupOptions->option("showPythonMatrices");
    settings.skipFuncWindowsLoading = _groupOptions->option("skipFuncWindowsLoading");

    // view
    settings.smallToolbarImages = _groupView->option("smallToolbarImages");
    settings.showBackground = _groupView->option("showBackground");
    settings.useNativeMenuBar = _groupView->option("useNativeMenuBar");
    settings.useSystemDialogs = _groupView->option("useSystemDialogs");
    settings.numberPrecisionData = _numberPrecisionData->value();

    // layout
    settings.layoutExportTransparent = _groupLayoutExport->option("layoutExportTransparent");

    // units
    settings.defaultUnitBeamRadius = _defaultUnitBeamRadius->selectedUnit();
    settings.defaultUnitFrontRadius = _defaultUnitFrontRadius->selectedUnit();
    settings.defaultUnitAngle = _defaultUnitAngle->selectedUnit();

    // export
    settings.exportAsCsv = _groupExportData->option("exportAsCsv");
    settings.exportSystemLocale = _groupExportData->option("exportSystemLocale");
    settings.exportTransposed = _groupExportData->option("exportTransposed");
    settings.exportNumberPrecision = _exportNumberPrecision->value();
    settings.exportHideCursor = _groupExportPlot->option("exportHideCursor");

    // calc
    settings.calcTablesMediumEnds = _groupCalcOpts->option("calcTablesMediumEnds");
    settings.calcTablesEmptySpaces = _groupCalcOpts->option("calcTablesEmptySpaces");

    settings.save();
    return true;
}
