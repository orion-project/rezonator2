#include "ElementPropsDialog.h"

#include "../app/Appearance.h"
#include "../app/PersistentState.h"
#include "../core/Element.h"
#include "../core/Schema.h"
#include "../widgets/ParamEditor.h"
#include "../widgets/ParamsEditor.h"
#include "../widgets/SchemaLayout.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriSvgView.h"

#include <QApplication>
#include <QCheckBox>
#include <QFormLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QToolButton>
#include <QTimer>
#include <QWidgetAction>

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
    _eventsLocker.reset(new ElementEventsLocker(_element, "ElementPropsDialog::collect"));
    _matrixLocker.reset(new ElementMatrixLocker(_element, "ElementPropsDialog::collect"));

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
    _tabs->setCornerWidget(_butParamsMenu);
    connect(_tabs, &QTabWidget::currentChanged, this, [this](int tabIndex){
        _butParamsMenu->setVisible(tabIndex == 0);
    });

    mainLayout()->addLayout(layoutCommon);
    mainLayout()->addSpacing(6);
    mainLayout()->addWidget(_tabs);
}

ElementPropsDialog::~ElementPropsDialog()
{
    // Explicitly delete editors before deleteing _newParams
    // Otherwise param editors will be deleted *after* them
    // and will crash on listener unsubscribing
    _editorParams->removeEditors();

    qDeleteAll(_newParams);

    resetParamPresets();    
    
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
    #define A_ Ori::Gui::action
    _actnCreateParam = A_(tr("Add Parameter..."), this, &ElementPropsDialog::createCustomParam, ":/toolbar/plus");
    _actnEditParam = A_(tr("Edit Parameter..."), this, &ElementPropsDialog::editCustomParam, ":/toolbar/wrench");
    _actnRemoveParam = A_(tr("Remove Parameter..."), this, &ElementPropsDialog::removeCustomParam, ":/toolbar/delete");
    _actnRestorePresets = A_(tr("Restore Default Presets..."), this, &ElementPropsDialog::restoreParamPresets, ":/toolbar/update");
    _actnParamsHelp = A_(tr("Help"), this, &ElementPropsDialog::showParamsHelp, ":/toolbar/help");
    #undef A_

    _menuParams = new QMenu(this);
    _menuParams->addAction(_actnEditParam);
    connect(_menuParams, &QMenu::aboutToShow, this, &ElementPropsDialog::updateParamsMenu);
    
    _butParamsMenu = new QToolButton;
    _butParamsMenu->setToolTip(tr("Add parameter"));
    _butParamsMenu->setIcon(QIcon(":/toolbar/menu_sm"));
    _butParamsMenu->setIconSize({14, 14});
    _butParamsMenu->setObjectName("elem_props_create_param_button");
    _butParamsMenu->setMenu(_menuParams);
    _butParamsMenu->setPopupMode(QToolButton::InstantPopup);

    auto schema = dynamic_cast<Schema*>(_element->owner());
    ParamsEditor::Options opts(_element->params());
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
    updatePageParams();
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
    
    auto schema = dynamic_cast<Schema*>(_element->owner()); 

    _element->setLabel(_editorLabel->text());
    _element->setTitle(_editorTitle->text());
    _element->setDisabled(_elemDisabled->isChecked());
    _element->layoutOptions.showLabel = _layoutShowLabel->isChecked();
    _element->layoutOptions.drawAlt = _layoutDrawAlt->isChecked();
    
    for (auto p : std::as_const(_newParams))
    {
        _element->addParam(p);
        if (schema)
            schema->events().raise(SchemaEvents::CustomParamCreated, p, "ElementPropsDialog");
    }
    for (auto p : std::as_const(_removedParams))
    {
        auto link = schema ? schema->paramLinks()->byTarget(p) : nullptr;
        if (link)
            schema->paramLinks()->removeOne(link);
        if (schema)
            schema->events().raise(SchemaEvents::CustomParamDeleting, p, "ElementPropsDialog");
        _element->removeParam(p, true);
        if (schema)
            schema->events().raise(SchemaEvents::CustomParamDeleted, p, "ElementPropsDialog");
    }
    for (auto p : std::as_const(_editedParams))
    {
        if (schema)
            schema->events().raise(SchemaEvents::CustomParamEdited, p, "ElementPropsDialog");
    }

    _editorParams->applyValues();
        
    RecentData::PendingSave _;
    foreach (auto p, _newParams + _redimedParams)
        RecentData::setUnit("custom_param_unit", p->value().unit());

    _newParams.clear();
    
    accept();
}

void ElementPropsDialog::reject()
{
    for (auto it = _backupParams.cbegin(); it != _backupParams.cend(); it++)
        it.key()->copyFrom(&it.value());
        
    RezonatorDialog::reject();
}

QString ElementPropsDialog::helpTopic() const
{
   return "matrix/" % _element->type() % ".html";
}

void ElementPropsDialog::updatePageParams()
{
    _pageParams->setCurrentIndex(_editorParams->editors().isEmpty() ? 0 : 1);
}

Z::Parameters ElementPropsDialog::existedParams() const
{
    Z::Parameters res;
    for (auto p : _element->params())
        if (!_removedParams.contains(p))
            res << p;
    return res + _newParams;
}

void ElementPropsDialog::addCustomParam(Z::Parameter *param)
{
    auto unit = RecentData::getUnit("custom_param_unit", param->dim());
    param->setValue(Z::Value(0, unit));
    param->setOption(Z::ParamOption::Custom);
    _newParams << param;
    auto paramEditor = _editorParams->addEditor(param);
    updatePageParams();
    QTimer::singleShot(100, this, [paramEditor](){ paramEditor->focus(); });
}

void ElementPropsDialog::createCustomParam()
{
    ParamSpecEditor ed(nullptr, {
        .recentKeyPrefix = "custom_param",
        .existedParams = existedParams(),
        .allowNameEditor = true,
        .allowSavePreset = true,
    });
    if (!ed.exec(tr("Create Parameter")))
        return;
    auto param = new Z::Parameter(ed.dim(), ed.alias(), ed.label(), ed.name(), ed.descr());
    addCustomParam(param);
    if (ed.needSavePreset())
    {
        ParamPresets::save(param);
        resetParamPresets();
    }
}

void ElementPropsDialog::editCustomParam()
{
    auto paramEditor = _editorParams->selectedEditor();
    if (!paramEditor)
        return;
    auto param = paramEditor->parameter();
    if (!param->hasOption(Z::ParamOption::Custom))
        return;

    ParamSpecEditor editor(param, {
        .existedParams = existedParams(),
        .allowNameEditor = true,
    });
    if (!editor.exec(tr("Edit Parameter")))
        return;
        
    auto index = _editorParams->editors().indexOf(paramEditor);
    auto alias = editor.alias();
    auto aliasEdited = alias != param->alias();
    auto label = editor.label();
    auto labelEdited = label != param->label();
    auto name = editor.name();
    auto nameEdited = name != param->name();
    auto descr = editor.descr();
    auto descrEdited = descr != param->description();
    auto dim = editor.dim();
    auto dimEdited = dim != param->dim();
    auto edited = aliasEdited || labelEdited || nameEdited || descrEdited || dimEdited;
        
    if (edited && !_newParams.contains(param))
    {
        _editedParams << param;

        if (!_backupParams.contains(param))
        {
            Z::Parameter backup;
            backup.copyFrom(param);
            _backupParams[param] = backup;
        }
    }

    if (aliasEdited)
        param->setAlias(alias);
    if (labelEdited)
        param->setLabel(label);
    if (nameEdited)
        param->setName(name);
    if (descrEdited)
        param->setDescription(descr);
    if (dimEdited)
    {
        // I's fine to apply param value here
        // because element events are locked during the whole dialog
        // and we have backups that will be restored on dlg rejection
        param->setDim(dim);
        auto unit = RecentData::getUnit("custom_param_unit", dim);
        auto value = unit->fromSi(param->value().toSi());
        // Need to set a value even for param driven by link
        // to have a proper target unit (recent for the dim) 
        // for the link when source and target dims mismatch
        param->setValue(Z::Value(value, unit));

        auto schema = dynamic_cast<Schema*>(_element->owner()); 
        auto link = schema ? schema->paramLinks()->byTarget(param) : nullptr;
        if (link) link->apply();
        
        if (!_redimedParams.contains(param))
            _redimedParams << param;
    }
    if (aliasEdited || dimEdited)
    {
        // ParamEditor is not designed for parameters changing on the fly
        _editorParams->removeEditor(param);
        paramEditor = _editorParams->addEditor(param, {}, index);
        QTimer::singleShot(100, this, [paramEditor](){ paramEditor->focus(); });
    }
}

void ElementPropsDialog::removeCustomParam()
{
    auto editor = _editorParams->selectedEditor();
    if (!editor)
        return;
    auto param = editor->parameter();
    if (!param->hasOption(Z::ParamOption::Custom))
        return;
    if (!Ori::Dlg::yes(tr("Remove parameter <b>%1</b> ?").arg(param->alias())))
        return;

    _editorParams->removeEditor(param);
    updatePageParams();
    
    bool newParamRemoved = false;
    for (int i = 0; i < _newParams.size(); i++)
        if (_newParams.at(i) == param)
        {
            newParamRemoved = true;
            delete _newParams.at(i);
            _newParams.removeAt(i);
            break;
        }
    if (!newParamRemoved) {
        _removedParams << param;
        _backupParams.remove(param);
    }
    _redimedParams.removeAll(param);
    _editedParams.removeAll(param);

    if (!_editorParams->editors().isEmpty())
        QTimer::singleShot(100, this, [this]{ _editorParams->editors().first()->focus(); });
}

void ElementPropsDialog::updateParamsMenu()
{
    auto editor = _editorParams->selectedEditor();
    bool isCustom = editor && editor->parameter()->hasOption(Z::ParamOption::Custom);
    _actnEditParam->setEnabled(isCustom);
    _actnEditParam->setVisible(isCustom);
    _actnRemoveParam->setEnabled(isCustom);
    _actnRemoveParam->setVisible(isCustom);
    
    if (!_paramPresets.isEmpty())
        return;

    _menuParams->addAction(_actnEditParam);
    _menuParams->addAction(_actnRemoveParam);
    _menuParams->addAction(_actnCreateParam);
    _menuParams->addSeparator();
    
    _paramPresets = ParamPresets::getAll();
    auto builtin = ParamPresets::getBuiltin();
    int builtinVisible = 0;
    int customVisible = 0;
    for (auto p : std::as_const(_paramPresets))
    {
        if (builtin.contains(p))
            builtinVisible++;
        else customVisible++;
        _menuParams->addAction(makePresetAction(p));
    }
    _actnRestorePresets->setEnabled(builtinVisible < builtin.size() || customVisible > 0);
    
    _menuParams->addSeparator();
    _menuParams->addAction(_actnRestorePresets);
    _menuParams->addAction(_actnParamsHelp);
}

QAction* ElementPropsDialog::makePresetAction(Z::Parameter *preset)
{
    QString title;
    auto label = preset->label();
    auto name = preset->name();
    if (!name.isEmpty() && name != label)
        title = QString("%1 (%2)").arg(name, label);
    else title = label;

    auto action = new QWidgetAction(_menuParams);
    auto button = new QToolButton;
    auto menu = new QMenu(button);
    
    auto actnApply = Ori::Gui::action(title, button, [this, preset]{
        _menuParams->close();
        addCustomParam(ParamPresets::makeParam(preset));
    });

    auto actnEdit = menu->addAction(QIcon(":/toolbar/wrench"), tr("Edit Preset..."), [this, preset]{
        ParamSpecEditor ed(preset, {
            .recentKeyPrefix = "custom_param",
            .allowNameEditor = true,
        });
        if (!ed.exec(tr("Edit Preset")))
            return;
        Z::Parameter newParam(ed.dim(), ed.alias(), ed.label(), ed.name(), ed.descr());
        newParam.setOption(Z::ParamOption::Custom);
        ParamPresets::update(preset, &newParam);
        resetParamPresets();
    });
    actnEdit->setEnabled(!ParamPresets::getBuiltin().contains(preset));

    menu->addAction(QIcon(":/toolbar/delete"), tr("Remove Preset..."), this, [this, preset, title]{
        if (!Ori::Dlg::yes(tr("Remove preset <b>%1</b> ?").arg(title)))
            return;
        ParamPresets::remove(preset);
        resetParamPresets();
    });
    
    button->setDefaultAction(actnApply);
    button->setMenu(menu);
    button->setPopupMode(QToolButton::MenuButtonPopup);
    button->setProperty("role", "preset-menu-item");

    action->setDefaultWidget(button);
    return action;
}

void ElementPropsDialog::resetParamPresets()
{
    auto builtin = ParamPresets::getBuiltin();
    for (auto p : std::as_const(_paramPresets))
        if (!builtin.contains(p))
            delete p;
    _paramPresets.clear();
    _menuParams->clear();
    // There must be something in the menu to make it "popupable"
    _menuParams->addAction(_actnEditParam);
}

void ElementPropsDialog::restoreParamPresets()
{
    if (!Ori::Dlg::yes(tr("Restore parameter presets to their defaults?\n\nAll custom presets will be deleted!")))
        return;
    ParamPresets::restore();
    resetParamPresets();
}

void ElementPropsDialog::showParamsHelp()
{

}
