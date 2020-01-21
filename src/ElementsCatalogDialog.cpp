#include "ElementsCatalogDialog.h"

#include "Appearance.h"
#include "CustomElemsManager.h"
#include "core/Schema.h"
#include "core/Elements.h"
#include "core/ElementsCatalog.h"
#include "funcs/FormatInfo.h"
#include "widgets/ElementImagesProvider.h"
#include "widgets/ElementTypesListView.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriSvgView.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QIcon>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTextBrowser>

static int __savedTabIndex = 0;

Element* ElementsCatalogDialog::chooseElementSample()
{
    ElementsCatalogDialog catalog;
    if (catalog.exec() != QDialog::Accepted)
        return nullptr;

    auto sample = catalog.selection();

    if (sample->hasOption(Element_CustomSample))
    {
        // Extract the element from the library instance
        // to prevent its deletion together with the lib
        catalog._library->deleteElement(sample, false, false);
    }

    return sample;
}

ElementsCatalogDialog::ElementsCatalogDialog(QWidget *parent): RezonatorDialog(DontDeleteOnClose, parent)
{
    setTitleAndIcon(tr("Elements Catalog"), ":/window_icons/catalog");
    setObjectName("ElementsCatalogDialog");

    // category tabs
    _categoryTabs = new QTabWidget;
    for (auto category : ElementsCatalog::instance().categories())
        _categoryTabs->addTab(new QWidget, category);

    auto res = CustomElemsManager::loadLibrary();
    if (res.ok())
    {
        if (not res.result()->isEmpty())
        {
            _library = res.result();
            _categoryTabs->addTab(new QWidget, tr("Custom"));
            _customElemsTab = _categoryTabs->count()-1;
            _previewHtml = new QTextBrowser;
            _previewHtml->setFont(Z::Gui::ValueFont().get());
            _previewHtml->document()->setDefaultStyleSheet(Z::Gui::reportStyleSheet());
        }
    }
    else
        Ori::Dlg::error(tr("There are messages while loading Custom Elements Library:\n\n%1").arg(res.error()));

    connect(_categoryTabs, &QTabWidget::currentChanged, this, &ElementsCatalogDialog::categorySelected);
    mainLayout()->addWidget(_categoryTabs);

    // preview
    _previewSvg = new Ori::Widgets::SvgView(QString());

    // element list
    _elementsList = new ElementTypesListView;
    connect(_elementsList, &ElementTypesListView::elementSelected, this, &ElementsCatalogDialog::updatePreview);
    connect(_elementsList, &ElementTypesListView::itemDoubleClicked, this, &ElementsCatalogDialog::accept);
    connect(_elementsList, &ElementTypesListView::enterPressed, this, &ElementsCatalogDialog::accept);

    _preview = new QStackedWidget;
    _preview->addWidget(_previewSvg);
    if (_previewHtml)
        _preview->addWidget(_previewHtml);

    // page
    _pageLayout = new QVBoxLayout;
    _pageLayout->addWidget(Ori::Gui::splitterH(_elementsList, _preview));
    _pageLayout->setMargin(mainLayout()->spacing()/2+1);

    // initial view
    int tabIndex = qMin(qMax(0, __savedTabIndex), _categoryTabs->count()-1);
    _categoryTabs->setCurrentIndex(tabIndex);
    categorySelected(tabIndex);
    _elementsList->setFocus();
}

ElementsCatalogDialog::~ElementsCatalogDialog()
{
    __savedTabIndex = _categoryTabs->currentIndex();

    if (_library)
        delete _library;
}

void ElementsCatalogDialog::categorySelected(int index)
{
    if (index < 0) return;
    _categoryTabs->widget(index)->setLayout(_pageLayout);
    if (index == _customElemsTab)
    {
        _elementsList->populate(_library->elements(), ElementTypesListView::DisplayNameKind::Title);
        _preview->setCurrentWidget(_previewHtml);
    }
    else
    {
        auto category = ElementsCatalog::instance().categories().at(index);
        _elementsList->populate(ElementsCatalog::instance().elements(category));
        _preview->setCurrentWidget(_previewSvg);
    }
    _elementsList->setFocus();
}

Element *ElementsCatalogDialog::selection() const
{
    return _elementsList->selected();
}

static QString makeCustomElemPreview(Element* elem)
{
    QString report;
    QTextStream stream(&report);
    stream << QStringLiteral("<p><b><span class=elem_title>")
           << elem->typeName()
           << QStringLiteral("</span></b><p>");

    Z::Format::FormatParam f;
    f.includeDriver = false;
    f.includeValue = true;

    for (const auto param : elem->params())
        stream << f.format(param) << QStringLiteral("<br/>");

    stream << "<center><img src='" << ElementImagesProvider::instance().drawingPath(elem->type()) << "'/></center>";

    return report;
}

void ElementsCatalogDialog::updatePreview(Element* elem)
{
    if (_categoryTabs->currentIndex() == _customElemsTab)
        _previewHtml->setHtml(makeCustomElemPreview(elem));
    else
        _previewSvg->load(ElementImagesProvider::instance().drawingPath(elem->type()));
}
