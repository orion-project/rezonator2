#include "AppSettingsDialog.h"
#include "AppSettings.h"
#include "widgets/OriOptionsGroup.h"

#include <QBoxLayout>
#include <QFormLayout>
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
    setObjectName("AppSettingsDialog");
    setTitleAndIcon(tr("Preferences"), ":/window_icons/options");

    createPages({createGeneralPage(), createViewPage()});
}

QWidget* AppSettingsDialog::createGeneralPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Behaviour"), ":/config_pages/general");

    groupOptions = new Ori::Widgets::OptionsGroup(tr("Options"), {
        tr("Edit just created element"),
        tr("Automatically generate labels for new elements"),
        tr("Show start window after application run")
    });

    page->add({groupOptions});
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

    page->add({groupView});
    return page;
}

QWidget* AppSettingsDialog::createLayoutPage()
{
    QWidget *page = new QWidget;
    page->setWindowTitle(tr("Layout"));
    page->setWindowIcon(QIcon(":/config_pages/layout"));
    return page;
}

void AppSettingsDialog::populate()
{
    Settings &settings = Settings::instance();

    // options
    groupOptions->setOption(0, settings.editNewElem);
    groupOptions->setOption(1, settings.elemAutoLabel);
    groupOptions->setOption(2, settings.showStartWindow);

    // view
    groupView->setOption(0, settings.smallToolbarImages);
    groupView->setOption(1, settings.showBackground);
    groupView->setOption(2, settings.useNativeMenuBar);
    groupView->setOption(3, settings.useSystemDialogs);
}

bool AppSettingsDialog::collect()
{
    Settings &settings = Settings::instance();

    // options
    settings.editNewElem = groupOptions->option(0);
    settings.elemAutoLabel = groupOptions->option(1);
    settings.showStartWindow = groupOptions->option(2);

    // view
    settings.smallToolbarImages = groupView->option(0);
    settings.showBackground = groupView->option(1);
    settings.useNativeMenuBar = groupView->option(2);
    settings.useSystemDialogs = groupView->option(3);

    settings.save();

    return true;
}
