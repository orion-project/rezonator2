#include "ElementsCatalogDialog.h"

#include "../app/Appearance.h"
#include "../app/AppSettings.h"
#include "../app/CustomElemsManager.h"
#include "../core/ElementsCatalog.h"
#include "../core/Schema.h"
#include "../math/FormatInfo.h"
#include "../widgets/ElementTypesListView.h"

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

std::optional<ElementsCatalogDialog::ElementSample> ElementsCatalogDialog::chooseElementSample()
{
    ElementsCatalogDialog dlg;
    if (dlg.exec() != QDialog::Accepted)
        return {};

    ElementsCatalogDialog::ElementSample sample;
    sample.elem = dlg.selection();

    if (sample.elem->hasOption(Element_CustomSample))
    {
        // Extract the sample from the library instance
        // to prevent its deletion together with the lib
        dlg._library->deleteElements({sample.elem}, Arg::RaiseEvents(false), Arg::FreeElem(false));
        sample.isCustom = true;
        sample.deleter = QSharedPointer<Element>(sample.elem);
    }

    return sample;
}

ElementsCatalogDialog::ElementsCatalogDialog(QWidget *parent): RezonatorDialog(DontDeleteOnClose | UseHelpButton, parent)
{
    setTitleAndIcon(tr("Elements Catalog"), ":/window_icons/catalog");
    setObjectName("ElementsCatalogDialog");

    // category tabs
    _categoryTabs = new QTabWidget;
    for (auto& category : ElementsCatalog::instance().categories())
        _categoryTabs->addTab(new QWidget, category);

    if (AppSettings::instance().showCustomElemLibrary)
    {
        auto res = CustomElemsManager::loadLibrary();
        if (res.ok())
        {
            if (res.result()->count() > 0)
            {
                _library = res.result();
                _categoryTabs->addTab(new QWidget, tr("Custom library"));
                _customElemsTab = _categoryTabs->count()-1;
                _previewHtml = new QTextBrowser;
                _previewHtml->setFont(Z::Gui::ValueFont().get());
                _previewHtml->document()->setDefaultStyleSheet(Z::Gui::reportStyleSheet());
            }
        }
        else
            Ori::Dlg::error(tr("There are messages while loading Custom Elements Library:\n\n%1").arg(res.error()));
    }

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
    auto m = mainLayout()->spacing()/2+1;
    _pageLayout->setContentsMargins(m, m, m, m);

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

void ElementsCatalogDialog::makeCustomElemPreview(Element* elem)
{
    QString report;
    QTextStream stream(&report);
    stream << QStringLiteral("<p><b><span class=elem_title>")
           << elem->typeName();

    QString customLabel = elem->label();
    if (!customLabel.isEmpty())
        stream << QStringLiteral(" (") << customLabel << ')';

    stream << QStringLiteral("</span></b><p>");

    Z::Format::FormatParam f;
    f.includeDriver = false;
    f.includeValue = true;

    for (const auto param : elem->params())
        stream << f.format(param) << QStringLiteral("<br/>");

    QString elemType = elem->type();
    if (!_customPreviews.contains(elemType))
    {
        // To have preview properly rendered on 4k displays
        // we need to load it as icon and then display automatically upscaled pixmap
        QString path = Z::Utils::elemDrawingPath(elemType);
        auto pxm = QIcon(path).pixmap(QImage(path).size());
        _previewHtml->document()->addResource(QTextDocument::ImageResource, elemType, pxm);
        _customPreviews.insert(elemType);
    }
    stream << "<center><img src='" << elemType << "'/></center>";

    _previewHtml->setHtml(report);
}

void ElementsCatalogDialog::updatePreview(Element* elem)
{
    if (_categoryTabs->currentIndex() == _customElemsTab)
        makeCustomElemPreview(elem);
    else
        _previewSvg->load(Z::Utils::elemDrawingPath(elem->type()));
}
