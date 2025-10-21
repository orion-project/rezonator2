#include "ElementPropsDialog.h"

#include "../app/Appearance.h"
#include "../app/PersistentState.h"
#include "../core/Element.h"
#include "../core/Schema.h"
#include "../widgets/ParamEditor.h"
#include "../widgets/ParamsEditor.h"
#include "../widgets/SchemaLayout.h"

#include "widgets/OriSvgView.h"
#include "helpers/OriLayouts.h"

#include <QApplication>
#include <QCheckBox>
#include <QFormLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTimer>

//------------------------------------------------------------------------------
//                             ElementPropsDialog
//------------------------------------------------------------------------------

namespace {
int __savedTabIndex = 0;
}

bool ElementPropsDialog::editElement(Element *elem, QWidget *parent)
{
    auto dlg = new ElementPropsDialog(elem, parent);
    return dlg && dlg->exec() == QDialog::Accepted;
}

ElementPropsDialog::ElementPropsDialog(Element *elem, QWidget* parent) : RezonatorDialog(UseHelpButton, parent)
{
    _element = elem;

    setTitleAndIcon(_element->typeName(), ":/window_icons/element");
    setObjectName("ElementPropsDialog");

    // common props
    _editorLabel = new QLineEdit;
    _editorLabel->setFont(Z::Gui::ValueFont().get());
    _editorTitle = new QLineEdit;
    _editorTitle->setFont(Z::Gui::ValueFont().get());

    auto layoutCommon = new QFormLayout;
    layoutCommon->addRow(tr("Label:"), _editorLabel);
    layoutCommon->addRow(tr("Title:"), _editorTitle);

    // parameters tab-set
    _tabs = new QTabWidget;
    _tabs->addTab(initPageParams(), tr("Parameters"));
    _tabs->addTab(initPageOptions(), tr("Options"));
    _tabs->addTab(initPageOutline(), tr("Outline"));
    _tabs->setCornerWidget(_butCreateParam);
    connect(_tabs, &QTabWidget::currentChanged, this, [this](int tabIndex){
        _butCreateParam->setVisible(tabIndex == 0);
    });

    mainLayout()->addLayout(layoutCommon);
    mainLayout()->addSpacing(6);
    mainLayout()->addWidget(_tabs);
}

ElementPropsDialog::~ElementPropsDialog()
{
    qDeleteAll(_newParams);
    __savedTabIndex = _tabs->currentIndex();
}

void ElementPropsDialog::showEvent(QShowEvent* event)
{
    RezonatorDialog::showEvent(event);

    _tabs->setCurrentIndex(__savedTabIndex);
    
    if (_tabs->currentIndex() == 0)
        _editorParams->focus();
    else
        _editorLabel->setFocus();

    populate();
}

QWidget* ElementPropsDialog::initPageParams()
{
    auto menuCreateParam = new QMenu(this);
    // TODO: populate presets
    menuCreateParam->addSeparator();
    auto actnCreateParam = menuCreateParam->addAction(tr("Create New..."));
    connect(actnCreateParam, &QAction::triggered, this, &ElementPropsDialog::createCustomParam);

    _butCreateParam = new QPushButton;
    _butCreateParam->setToolTip(tr("Add parameter"));
    _butCreateParam->setIcon(QIcon(":/toolbar/plus"));
    _butCreateParam->setIconSize({14, 14});
    _butCreateParam->setObjectName("elem_props_create_param_button");
    _butCreateParam->setMenu(menuCreateParam);

    auto schema = dynamic_cast<Schema*>(_element->owner());
    ParamsEditor::Options opts(&_element->params());
    opts.filter.reset(new Z::ParameterFilter({new Z::ParameterFilterVisible()}));
    opts.globalParams = schema ? schema->globalParams() : nullptr;
    opts.paramLinks = schema ? schema->paramLinks() : nullptr;
    opts.useExpression = true;
    _editorParams = new ParamsEditor(opts);
    
    auto labelEmpty = new QLabel(tr("Element has no editable parameters"));
    labelEmpty->setMargin(6);
    labelEmpty->setAlignment(Qt::AlignCenter);
    
    _pageParams = new QStackedWidget;
    _pageParams->addWidget(labelEmpty);
    _pageParams->addWidget(_editorParams);
    
    return _pageParams;
}

QWidget* ElementPropsDialog::initPageOptions()
{
    _elemDisabled = new QCheckBox(tr("Ignore in calculations (disable element)"));
    _layoutShowLabel = new QCheckBox(tr("Show element label on layout"));

    _layoutDrawAlt = new QCheckBox();
    auto opts = ElementLayoutFactory::getOptions(_element);
    if (opts && opts->hasAltVersion())
        _layoutDrawAlt->setText(tr(opts->altVersionOptionTitle(), "LayoutOptions"));
    else
        _layoutDrawAlt->setVisible(false);

    return Ori::Layouts::LayoutV({
        _elemDisabled,
        _layoutShowLabel,
        _layoutDrawAlt,
        Ori::Layouts::Stretch()
    }).makeWidget();
}

QWidget* ElementPropsDialog::initPageOutline()
{
    auto outline = new Ori::Widgets::SvgView;
    outline->load(Z::Utils::elemDrawingPath(_element->type()));
    return Ori::Layouts::LayoutV({outline}).setMargin(3).makeWidget();
}

void ElementPropsDialog::populate()
{
    _editorLabel->setText(_element->label());
    _editorTitle->setText(_element->title());
    _elemDisabled->setChecked(_element->disabled());
    _layoutShowLabel->setChecked(_element->layoutOptions.showLabel);
    _layoutDrawAlt->setChecked(_element->layoutOptions.drawAlt);
    _editorParams->populateValues();
    _pageParams->setCurrentIndex(_editorParams->editors().isEmpty() ? 0 : 1);
}

void ElementPropsDialog::collect()
{
    auto res = _editorParams->verify();
    if (!res.isEmpty())
    {
        _tabs->setCurrentIndex(0);
        // TODO:NEXT-VER use class WidgetResult to be able to focus invalid param editor
        QMessageBox::warning(this, qApp->applicationName(), res);
        return;
    }

    ElementEventsLocker eventsLocker(_element, "ElementPropsDialog::collect");
    ElementMatrixLocker matrixLocker(_element, "ElementPropsDialog::collect");

    _element->setLabel(_editorLabel->text());
    _element->setTitle(_editorTitle->text());
    _element->setDisabled(_elemDisabled->isChecked());
    _element->layoutOptions.showLabel = _layoutShowLabel->isChecked();
    _element->layoutOptions.drawAlt = _layoutDrawAlt->isChecked();
    
    for (auto p : std::as_const(_newParams))
        _element->addParam(p);

    _editorParams->applyValues();
        
    RecentData::PendingSave _;
    for (auto p : std::as_const(_newParams)) {
        auto key = ("custom_param_" + p->alias() + "_unit").toLatin1();
        RecentData::setUnit(key.constData(), p->value().unit());
    }

    _newParams.clear();
    
    accept();
    close();
}

QString ElementPropsDialog::helpTopic() const
{
   return "matrix/" % _element->type() % ".html";
}

void ElementPropsDialog::createCustomParam()
{
    ParamSpecEditor editor(nullptr, {
        .recentKeyPrefix = "custom_param",
        .existedParams = _element->params() + _newParams
    });
    if (!editor.exec(tr("Create Parameter")))
        return;

    auto dim = editor.dim();
    auto alias = editor.alias();
    auto label = alias;
    auto name = editor.descr();
    // Here we use descr in place of name because users will hardly write 
    // a long description similiar to what built-in parameters have.
    // Instead they will likely write a short info, e.g. "Aperture diameter"
    // which actually is the Name in Z::Parameter terms
    auto param = new Z::Parameter(dim, alias, label, name);
    auto unitKey = ("custom_param_" + alias + "_unit").toLatin1();
    auto unit = RecentData::getUnit(unitKey.constData(), dim);
    param->setValue(Z::Value(0, unit));
    param->setOption(Z::ParamOption::Custom);
    _newParams << param;
    _pageParams->setCurrentIndex(1);
    auto paramEditor = _editorParams->addEditor(param);
    QTimer::singleShot(100, this, [paramEditor](){ paramEditor->focus(); });
}
