#include "AppSettings.h"

#include "AppSettingsDialog.h"
#include "widgets/UnitWidgets.h"

#include "helpers/OriLayouts.h"
#include "widgets/OriOptionsGroup.h"

#include <QBoxLayout>
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

    page->add({_groupView, page->stretch()});
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

    // view
    _groupView->setOption(0, settings.smallToolbarImages);
    _groupView->setOption(1, settings.showBackground);
    _groupView->setOption(2, settings.useNativeMenuBar);
    _groupView->setOption(3, settings.useSystemDialogs);

    // layout
    _groupLayoutExport->setOption(0, settings.layoutExportTransparent);

    // units
    _defaultUnitBeamRadius->setSelectedUnit(settings.defaultUnitBeamRadius);
    _defaultUnitFrontRadius->setSelectedUnit(settings.defaultUnitFrontRadius);
    _defaultUnitAngle->setSelectedUnit(settings.defaultUnitAngle);
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

    // view
    settings.smallToolbarImages = _groupView->option(0);
    settings.showBackground = _groupView->option(1);
    settings.useNativeMenuBar = _groupView->option(2);
    settings.useSystemDialogs = _groupView->option(3);

    // layout
    settings.layoutExportTransparent = _groupView->option(1);

    // units
    settings.defaultUnitBeamRadius = _defaultUnitBeamRadius->selectedUnit();
    settings.defaultUnitFrontRadius = _defaultUnitFrontRadius->selectedUnit();
    settings.defaultUnitAngle = _defaultUnitAngle->selectedUnit();

    settings.save();
    return true;
}
