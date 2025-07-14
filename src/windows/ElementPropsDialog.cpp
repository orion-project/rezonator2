#include "ElementPropsDialog.h"

#include "../app/Appearance.h"
#include "../core/Element.h"
#include "../core/Schema.h"
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
#include <QMessageBox>
#include <QTabWidget>

//------------------------------------------------------------------------------
//                             ElementPropsDialog
//------------------------------------------------------------------------------

namespace {
int __savedTabIndex = 0;
}

bool ElementPropsDialog::editElement(Element *elem, QWidget *parent)
{
    ElementPropsDialog *dlg = nullptr;
    switch (elem->paramsEditorKind())
    {
    case Z::ParamsEditorKind::None:
        dlg = new ElementPropsDialog_None(elem, parent);
        break;

    case Z::ParamsEditorKind::List:
        if (Z::Utils::defaultParamFilter()->count(elem->params()) == 0)
            dlg = new ElementPropsDialog_None(elem, parent);
        else
            dlg = new ElementPropsDialog_List(elem, parent);
        break;

    case Z::ParamsEditorKind::ABCD:
        dlg = new ElementPropsDialog_Abcd(elem, parent);
        break;
    }
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
    _tabs->addTab(initPageOptions(), tr("Options"));
    _tabs->addTab(initPageOutline(), tr("Outline"));

    mainLayout()->addLayout(layoutCommon);
    mainLayout()->addSpacing(6);
    mainLayout()->addWidget(_tabs);

    _editorLabel->setFocus();
}

ElementPropsDialog::~ElementPropsDialog()
{
    __savedTabIndex = _tabs->currentIndex();
}

void ElementPropsDialog::showEvent(QShowEvent* event)
{
    RezonatorDialog::showEvent(event);

    _tabs->setCurrentIndex(__savedTabIndex);

    populate();
}

void ElementPropsDialog::setPageParams(QWidget* pageParams)
{
    _tabs->insertTab(0, pageParams, tr("Parameters"));
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

    populateParams();
}

void ElementPropsDialog::collect()
{
    auto res = verifyParams();
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

    collectParams();
    accept();
    close();
}

QString ElementPropsDialog::helpTopic() const
{
   return "matrix/" % _element->type() % ".html";
}

//------------------------------------------------------------------------------

ElementPropsDialog_None::ElementPropsDialog_None(Element *elem, QWidget *parent) : ElementPropsDialog(elem, parent)
{
    auto label = new QLabel(tr("Element has no editable parameters"));
    label->setMargin(6);
    label->setAlignment(Qt::AlignCenter);

    setPageParams(label);
}

//------------------------------------------------------------------------------

ElementPropsDialog_List::ElementPropsDialog_List(Element *elem, QWidget *parent) : ElementPropsDialog(elem, parent)
{
    auto schema = dynamic_cast<Schema*>(elem->owner());

    ParamsEditor::Options opts(&elem->params());
    opts.filter.reset(new Z::ParameterFilter({new Z::ParameterFilterVisible()}));
    opts.globalParams = schema ? schema->globalParams() : nullptr;
    opts.paramLinks = schema ? schema->paramLinks() : nullptr;
    opts.useExpression = true;

    _editors = new ParamsEditor(opts);

    setPageParams(_editors);

    _editors->focus();
}

void ElementPropsDialog_List::populateParams()
{
    _editors->populateValues();
}

void ElementPropsDialog_List::collectParams()
{
    _editors->applyValues();
}

QString ElementPropsDialog_List::verifyParams() const
{
    return _editors->verify();
}

//------------------------------------------------------------------------------

ElementPropsDialog_Abcd::ElementPropsDialog_Abcd(Element *elem, QWidget *parent) : ElementPropsDialog(elem, parent)
{
    setPageParams(Ori::Layouts::LayoutV({
        _editorMt = new ParamsEditorAbcd(QString("T"), elem->params().mid(0, 4)),
        _editorMs = new ParamsEditorAbcd(QString("S"), elem->params().mid(4, 4))
    }).makeWidget());

    _editorMt->focus();
}

void ElementPropsDialog_Abcd::populateParams()
{
    _editorMt->populate();
    _editorMs->populate();
}

void ElementPropsDialog_Abcd::collectParams()
{
    _editorMt->apply();
    _editorMs->apply();
}
