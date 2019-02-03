#include "AppSettingsDialog.h"
#include "AppSettings.h"
#include "widgets/OriOptionsGroup.h"

#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QIcon>
#include <QSpinBox>

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
    setTitleAndIcon(tr("Preferences"), ":/window_icons/options");

    createPages({
                    createGeneralPage(),
                    createViewPage(),
                    createLayoutPage()
                });
}

QWidget* AppSettingsDialog::createGeneralPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Behavior"), ":/config_pages/general");

    groupOptions = new Ori::Widgets::OptionsGroup(tr("Options"), {
        tr("Edit just created element"),
        tr("Generate labels for created elements"),
        tr("Generate labels for pasted elements"),
        tr("Show start window after application run")
    });

    page->add({groupOptions, page->stretch()});
    return page;
}

QWidget* AppSettingsDialog::createViewPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Interface"), ":/config_pages/view");

    // group box "Options"
    groupView = new Ori::Widgets::OptionsGroup(tr("Options"), {
        tr("Use small toolbar images"),
        tr("Show background image"),
        tr("Use native menu bar (restart required)"),
        tr("Use system open/save file dialogs")
    });

    page->add({groupView, page->stretch()});
    return page;
}

QWidget* AppSettingsDialog::createLayoutPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Layout"), ":/config_pages/layout");

    // group box "Options"
    groupLayoutExport = new Ori::Widgets::OptionsGroup(tr("Image export options"), {
        tr("Use transparent background"),
    });

    page->add({groupLayoutExport, page->stretch()});
    return page;
}

void AppSettingsDialog::populate()
{
    Settings &settings = Settings::instance();

    // options
    groupOptions->setOption(0, settings.editNewElem);
    groupOptions->setOption(1, settings.elemAutoLabel);
    groupOptions->setOption(2, settings.elemAutoLabelPasted);
    groupOptions->setOption(3, settings.showStartWindow);

    // view
    groupView->setOption(0, settings.smallToolbarImages);
    groupView->setOption(1, settings.showBackground);
    groupView->setOption(2, settings.useNativeMenuBar);
    groupView->setOption(3, settings.useSystemDialogs);

    // layout
    groupLayoutExport->setOption(0, settings.layoutExportTransparent);
}

bool AppSettingsDialog::collect()
{
    Settings &settings = Settings::instance();

    // options
    settings.editNewElem = groupOptions->option(0);
    settings.elemAutoLabel = groupOptions->option(1);
    settings.elemAutoLabelPasted = groupOptions->option(2);
    settings.showStartWindow = groupOptions->option(3);

    // view
    settings.smallToolbarImages = groupView->option(0);
    settings.showBackground = groupView->option(1);
    settings.useNativeMenuBar = groupView->option(2);
    settings.useSystemDialogs = groupView->option(3);

    // layout
    settings.save();
    settings.layoutExportTransparent = groupView->option(1);

    return true;
}
