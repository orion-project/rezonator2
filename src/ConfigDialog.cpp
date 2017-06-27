#include "ConfigDialog.h"
#include "AppSettings.h"
#include "widgets/OriOptionsGroup.h"

#include <QBoxLayout>
#include <QFormLayout>
#include <QIcon>
#include <QSpinBox>

////////////////////////////////////////////////////////////////////////////////

namespace Z {
namespace Dlg {

bool appConfig(QWidget *parent)
{
    ConfigDialog dialog(parent);
    return dialog.exec() == QDialog::Accepted;
}

} // namespace Dlgs
} // namespace Z

////////////////////////////////////////////////////////////////////////////////
//                              ConfigDialog
////////////////////////////////////////////////////////////////////////////////

int ConfigDialog::_savedTabIndex = 0;

ConfigDialog::ConfigDialog(QWidget* parent) : Ori::Dlg::BasicConfigDialog(parent)
{
    setWindowTitle(tr("Preferences"));
    setWindowIcon(QIcon(":/window_icons/options"));

    createPages({createGeneralPage(), createViewPage()});

    setCurrentPageIndex(_savedTabIndex);
}

ConfigDialog::~ConfigDialog()
{
    _savedTabIndex = currentPageIndex();
}

QWidget* ConfigDialog::createGeneralPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Behaviour"), ":/config_pages/general");

    groupOptions = new Ori::Widgets::OptionsGroup(tr("Options"), {
        tr("Edit just now created element"),
        tr("Automatically generate labels for new elements"),
        tr("Use system open/save file dialogs"),
        tr("Do not store function windows in schema file"),
        tr("Do not load function windows from schema file")
    });

    page->add({groupOptions});
    return page;
}

QWidget* ConfigDialog::createViewPage()
{
    auto page = new Ori::Dlg::BasicConfigPage(tr("Interface"), ":/config_pages/view");

    ///////// group box "Options"
    groupView = new Ori::Widgets::OptionsGroup(tr("Options"), {
        tr("Use small toolbar images"),
        tr("Show background image")
    });

    ///////// group box "Lists length"
//    sedMruLength = new QSpinBox;
//    sedMruLength->setRange(0, 99);

//    auto layoutLists = new QFormLayout;
//    layoutLists->addRow(tr("Recent files list length:"), sedMruLength);

//    auto groupLists = new QGroupBox(tr("Lists length"));
//    groupLists->setLayout(layoutLists);

    page->add({groupView});
    return page;
}

QWidget* ConfigDialog::createLayoutPage()
{
    QWidget *page = new QWidget;
    page->setWindowTitle(tr("Layout"));
    page->setWindowIcon(QIcon(":/config_pages/layout"));
    return page;
}

void ConfigDialog::populate()
{
    Settings &settings = Settings::instance();

    //// options
    groupOptions->setOption(0, settings.editNewElem);
    groupOptions->setOption(1, settings.elemAutoLabel);
    groupOptions->setOption(2, settings.useSystemDialogs);
    groupOptions->setOption(3, settings.dontSaveWindows);
    groupOptions->setOption(4, settings.ignoreSavedWindows);
    //sedMruLength->setValue(settings.maxRecentFiles);

    //// view
    groupView->setOption(0, settings.smallToolbarImages);
    groupView->setOption(1, settings.showBackground);
}

void ConfigDialog::collect()
{
    Settings &settings = Settings::instance();

    //// options
    settings.editNewElem = groupOptions->option(0);
    settings.elemAutoLabel = groupOptions->option(1);
    settings.useSystemDialogs = groupOptions->option(2);
    settings.dontSaveWindows = groupOptions->option(3);
    settings.ignoreSavedWindows = groupOptions->option(4);
    //settings.maxRecentFiles = sedMruLength->value();

    //// view
    settings.smallToolbarImages = groupView->option(0);
    settings.showBackground = groupView->option(1);

    settings.save();
}
