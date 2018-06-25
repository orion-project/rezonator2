#include "ElementsCatalogDialog.h"
#include "core/ElementsCatalog.h"
#include "widgets/ElementImagesProvider.h"
#include "widgets/ElementTypesListView.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriSvgView.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QIcon>
#include <QSplitter>
#include <QTabWidget>

namespace Z {
namespace Dlgs {

// TODO: ElementsCatalog should be QWidget, not QDialog.
// In CatalogMode_View mode it should be displayed as tool window (Qt::Tool),
// as this type of window is not overlapped by main window on MacOS.
// In CatalogMode_Selector it should be displayed via Ori::Dialog or like that.

bool selectElementType(QString& type)
{
    ElementsCatalogDialog catalog(ElementsCatalogDialog::CatalogMode_Selector);
    if (catalog.exec() == QDialog::Accepted)
    {
        type = catalog.selected();
        return true;
    }
    return false;
}

Element* createElement()
{
    QString type;
    if (selectElementType(type))
        return ElementsCatalog::instance().create(type);
    return nullptr;
}

void showElementsCatalog()
{
    auto catalog = new ElementsCatalogDialog(ElementsCatalogDialog::CatalogMode_View);
    // NOTE: on MacOS popup window can by overlapped by main window
    // It seems be normal for MacOS, e.g. file info window is opened by Finder
    // and can be overlapped by Finder main window so we can't activate it again.
    catalog->show();
}

} // namespace Dlgs
} // namespace Z

//-----------------------------------------------------------------------------
//                            ElementsCatalogDialog
//-----------------------------------------------------------------------------

namespace {
int __savedTabIndex = 0;
}

ElementsCatalogDialog::ElementsCatalogDialog(CatalogMode mode, QWidget *parent)
    : RezonatorDialog(mode == CatalogMode_View ? OmitButtonsPanel : DontDeleteOnClose, parent)
{
    setTitleAndIcon(tr("Elements Catalog"), ":/window_icons/catalog");
    setObjectName("ElementsCatalogDialog");

    // category tabs
    tabs = new QTabWidget;
    for (auto category : ElementsCatalog::instance().categories())
        tabs->addTab(new QWidget, category);
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(categorySelected(int)));
    mainLayout()->addWidget(tabs);

    // preview
    drawing = new Ori::Widgets::SvgView(QString());

    // element list
    elements = new ElementTypesListView;
    connect(elements, SIGNAL(elementSelected(QString)), this, SLOT(loadDrawing(QString)));
    if (mode == CatalogMode_Selector)
    {
        connect(elements, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
        connect(elements, SIGNAL(enterPressed()), this, SLOT(accept()));
    }

    // page
    layoutPage = new QVBoxLayout;
    layoutPage->addWidget(Ori::Gui::splitterH(elements, drawing));
    layoutPage->setMargin(mainLayout()->spacing()/2+1);

    // initial view
    tabs->setCurrentIndex(__savedTabIndex);
    categorySelected(__savedTabIndex);
    elements->setFocus();
}

ElementsCatalogDialog::~ElementsCatalogDialog()
{
    __savedTabIndex = tabs->currentIndex();
}

void ElementsCatalogDialog::categorySelected(int index)
{
    if (index < 0) return;
    tabs->widget(index)->setLayout(layoutPage);
    auto category = ElementsCatalog::instance().categories().at(index);
    elements->populate(ElementsCatalog::instance().elements(category));
    elements->setFocus();
}

QString ElementsCatalogDialog::selected() const
{
    return elements->selected();
}

void ElementsCatalogDialog::loadDrawing(const QString& elemType)
{
    drawing->load(ElementImagesProvider::instance().drawingPath(elemType));
}
