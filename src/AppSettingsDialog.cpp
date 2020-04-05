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
                });
}

QWidget* AppSettingsDialog::createGeneralPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Behavior"), ":/config_pages/general");

    _groupOptions = new Ori::Widgets::OptionsGroup(tr("Options"), {
        tr("Edit just created element"),
        tr("Generate labels for created elements"),
        tr("Generate labels for pasted elements"),
        tr("Generate labels for new pumps"),
        tr("Show start window when application runs"),
        tr("Show protocol window after application started"),
        tr("Show custom elements in Elements Catalog"),
        tr("Show Python code for matrices in info windows"),
        tr("Don't load function windows when opening schema"),
    });

    page->add({_groupOptions, page->stretch()});
    return page;
}

QWidget* AppSettingsDialog::createViewPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Interface"), ":/config_pages/view");

    // group box "Options"
    _groupView = new Ori::Widgets::OptionsGroup(tr("Options"), {
        tr("Use small toolbar images"),
        tr("Show background image"),
        tr("Use native menu bar (restart required)"),
        tr("Use system open/save file dialogs")
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
    _groupLayoutExport = new Ori::Widgets::OptionsGroup(tr("Image export options"), {
        tr("Use transparent background"),
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

    _groupExportData = new Ori::Widgets::OptionsGroup(tr("Graph data export options"), {
        tr("Use CSV format"),
        tr("Use system locale "),
        //tr("Write column headers"),
    });

    _exportNumberPrecision = new NumberPrecisionSpinBox;
    _groupExportData->addControls({
        LayoutH({new QLabel(tr("Number precision")), _exportNumberPrecision, Stretch()}).boxLayout(),
    });

    page->add({_groupExportData, page->stretch()});
    return page;
}

void AppSettingsDialog::populate()
{
    AppSettings &settings = AppSettings::instance();

    // options
    _groupOptions->setOption(0, settings.editNewElem);
    _groupOptions->setOption(1, settings.elemAutoLabel);
    _groupOptions->setOption(2, settings.elemAutoLabelPasted);
    _groupOptions->setOption(3, settings.pumpAutoLabel);
    _groupOptions->setOption(4, settings.showStartWindow);
    _groupOptions->setOption(5, settings.showProtocolAtStart);
    _groupOptions->setOption(6, settings.showCustomElemLibrary);
    _groupOptions->setOption(7, settings.showPythonMatrices);
    _groupOptions->setOption(8, settings.skipFuncWindowsLoading);

    // view
    _groupView->setOption(0, settings.smallToolbarImages);
    _groupView->setOption(1, settings.showBackground);
    _groupView->setOption(2, settings.useNativeMenuBar);
    _groupView->setOption(3, settings.useSystemDialogs);
    _numberPrecisionData->setValue(settings.numberPrecisionData);

    // layout
    _groupLayoutExport->setOption(0, settings.layoutExportTransparent);

    // units
    _defaultUnitBeamRadius->setSelectedUnit(settings.defaultUnitBeamRadius);
    _defaultUnitFrontRadius->setSelectedUnit(settings.defaultUnitFrontRadius);
    _defaultUnitAngle->setSelectedUnit(settings.defaultUnitAngle);

    // export
    _groupExportData->setOption(0, settings.exportAsCsv);
    _groupExportData->setOption(1, settings.exportSystemLocale);
    //_groupExportData->setOption(2, settings.exportColumnHeaders);
    _exportNumberPrecision->setValue(settings.exportNumberPrecision);
}

bool AppSettingsDialog::collect()
{
    AppSettings &settings = AppSettings::instance();

    // options
    settings.editNewElem = _groupOptions->option(0);
    settings.elemAutoLabel = _groupOptions->option(1);
    settings.elemAutoLabelPasted = _groupOptions->option(2);
    settings.pumpAutoLabel = _groupOptions->option(3);
    settings.showStartWindow = _groupOptions->option(4);
    settings.showProtocolAtStart = _groupOptions->option(5);
    settings.showCustomElemLibrary = _groupOptions->option(6);
    settings.showPythonMatrices = _groupOptions->option(7);
    settings.skipFuncWindowsLoading = _groupOptions->option(8);

    // view
    settings.smallToolbarImages = _groupView->option(0);
    settings.showBackground = _groupView->option(1);
    settings.useNativeMenuBar = _groupView->option(2);
    settings.useSystemDialogs = _groupView->option(3);
    settings.numberPrecisionData = _numberPrecisionData->value();

    // layout
    settings.layoutExportTransparent = _groupView->option(1);

    // units
    settings.defaultUnitBeamRadius = _defaultUnitBeamRadius->selectedUnit();
    settings.defaultUnitFrontRadius = _defaultUnitFrontRadius->selectedUnit();
    settings.defaultUnitAngle = _defaultUnitAngle->selectedUnit();

    // export
    settings.exportAsCsv = _groupExportData->option(0);
    settings.exportSystemLocale = _groupExportData->option(1);
    //settings.exportColumnHeaders = _groupExportData->option(2);
    settings.exportNumberPrecision = _exportNumberPrecision->value();

    settings.save();
    return true;
}
