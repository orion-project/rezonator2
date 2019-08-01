#include "ElementPropsDialog.h"

#include "Appearance.h"
#include "core/Element.h"
#include "core/Schema.h"
#include "widgets/ElementImagesProvider.h"
#include "widgets/ParamsEditor.h"

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
    case Z::ParamsEditor::None:
        dlg = new ElementPropsDialog_None(elem, parent);
        break;

    case Z::ParamsEditor::List:
        if (Z::Utils::defaultParamFilter()->count(elem->params()) == 0)
            dlg = new ElementPropsDialog_None(elem, parent);
        else
            dlg = new ElementPropsDialog_List(elem, parent);
        break;

    case Z::ParamsEditor::ABCD:
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
    // TODO:NEXT-VER _tabs->addTab(initPageOther(), tr("Other"));
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

/* TODO:NEXT-VER
QWidget* ElementPropsDialog::initPageOther()
{
    return Ori::Gui::widgetV({
        _disabled = new QCheckBox(tr("Ignore this element in calculations")), 0 });
} */

QWidget* ElementPropsDialog::initPageOutline()
{
    auto outline = new Ori::Widgets::SvgView;
    outline->load(ElementImagesProvider::instance().drawingPath(_element->type()));
    return Ori::Layouts::LayoutV({outline}).setMargin(3).makeWidget();
}

void ElementPropsDialog::populate()
{
    _editorLabel->setText(_element->label());
    _editorTitle->setText(_element->title());
    // TODO:NEXT-VER _disabled->setChecked(_element->disabled());

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

    ElementLocker locker(_element);

    _element->setLabel(_editorLabel->text());
    _element->setTitle(_editorTitle->text());
    // TODO:NEXT-VER _element->setDisabled(_disabled->isChecked());

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
    opts.globalParams = schema ? schema->customParams() : nullptr;
    opts.paramLinks = schema ? schema->paramLinks() : nullptr;

    _editors = new ParamsEditor(opts);

    setPageParams(_editors);

    _editors->focus();
}

void ElementPropsDialog_List::populateParams()
{
    _editors->populate();
}

void ElementPropsDialog_List::collectParams()
{
    _editors->apply();
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
