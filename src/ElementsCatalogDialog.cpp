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

bool selectElementType(QString& type)
{
    ElementsCatalogDialog catalog(ElementsCatalogDialog::CatalogMode_Selector, qApp->activeWindow());
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
    auto catalog = new ElementsCatalogDialog(ElementsCatalogDialog::CatalogMode_View, qApp->activeWindow());
    catalog->show();
}

} // namespace Dlgs
} // namespace Z

///////////////////////////////////////////////////////////////////////////////
//                            ElementsCatalogDialog
////////////////////////////////////////////////////////////////////////////////

ElementsCatalogDialog::ElementsCatalogDialog(CatalogMode mode, QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Elements Catalog"));
    setWindowIcon(QIcon(":/window_icons/catalog"));
    if (mode == CatalogMode_View)
        setAttribute(Qt::WA_DeleteOnClose);

    ///////// category tabs
    tabs = new QTabWidget;
    for (auto category : ElementsCatalog::instance().categories())
        tabs->addTab(new QWidget, category);
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(categorySelected(int)));

    ///////// main layout
    QVBoxLayout *layoutMain = new QVBoxLayout;
    layoutMain->addWidget(tabs);
    setLayout(layoutMain);

    ///////// buttons
    if (mode == CatalogMode_Selector)
    {
        QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
        qApp->connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
        qApp->connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
        layoutMain->addWidget(buttons);
    }

    ///////// preview
    drawing = new Ori::Widgets::SvgView(QString());

    ///////// element list
    elements = new ElementTypesListView;
    connect(elements, SIGNAL(elementSelected(QString)), this, SLOT(loadDrawing(QString)));
    connect(elements, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
    connect(elements, SIGNAL(enterPressed()), this, SLOT(accept()));

    ///////// page
    layoutPage = new QVBoxLayout;
    layoutPage->addWidget(Ori::Gui::splitterH(elements, drawing));
    layoutPage->setMargin(layoutMain->spacing()/2+1);

    ///////// initial view
    categorySelected(0);
    elements->setFocus();
    resize(600, 400); // TODO store and restore size
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
