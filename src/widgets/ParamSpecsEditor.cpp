#include "ParamSpecsEditor.h"

#include "../app/PersistentState.h"
#include "../core/Schema.h"
#include "../widgets/ParamEditor.h"
#include "../widgets/ParamsEditor.h"

#include "helpers/OriDialogs.h"

#include <QTimer>

ParamSpecsEditor::ParamSpecsEditor(Element *element, ParamsEditor *editorParams, bool customParams, QObject *parent)
    : QObject{parent}, _element(element), _editorParams(editorParams), _customParams(customParams)
{
}

ParamSpecsEditor::~ParamSpecsEditor()
{
    qDeleteAll(_newParams);
}

void ParamSpecsEditor::collect()
{
    auto schema = dynamic_cast<Schema*>(_element->owner()); 

    for (auto p : std::as_const(_newParams))
    {
        _element->addParam(p);
        if (schema)
            schema->events().raise(SchemaEvents::ElemParamCreated, p, "ParamSpecsEditor");
    }
    for (auto p : std::as_const(_removedParams))
    {
        auto link = schema ? schema->paramLinks()->byTarget(p) : nullptr;
        if (link)
            schema->paramLinks()->removeOne(link);
        if (schema)
            schema->events().raise(SchemaEvents::ElemParamDeleting, p, "ParamSpecsEditor");
        _element->removeParam(p, true);
        if (schema)
            schema->events().raise(SchemaEvents::ElemParamDeleted, p, "ParamSpecsEditor");
    }
    for (auto p : std::as_const(_editedParams))
    {
        if (schema)
            schema->events().raise(SchemaEvents::ElemParamEdited, p, "ParamSpecsEditor");
    }

    _editorParams->applyValues();
        
    RecentData::PendingSave _;
    foreach (auto p, _newParams + _redimedParams)
        RecentData::setUnit("custom_param_unit", p->value().unit());

    _newParams.clear();
}

void ParamSpecsEditor::reject()
{
    for (auto it = _backupParams.cbegin(); it != _backupParams.cend(); it++)
        it.key()->copyFrom(&it.value());
}

Z::Parameters ParamSpecsEditor::existedParams() const
{
    Z::Parameters res;
    for (auto p : _element->params())
        if (!_removedParams.contains(p))
            res << p;
    return res + _newParams;
}

void ParamSpecsEditor::addParam(Z::Parameter *param)
{
    auto unit = RecentData::getUnit("custom_param_unit", param->dim());
    param->setValue(Z::Value(0, unit));
    if (_customParams)
        param->setOption(Z::ParamOption::Custom);
    _newParams << param;
    auto paramEditor = _editorParams->addEditor(param);
    QTimer::singleShot(100, this, [paramEditor](){ paramEditor->focus(); });
}

void ParamSpecsEditor::createParamDlg()
{
    ParamSpecEditor ed(nullptr, {
        .recentKeyPrefix = "custom_param",
        .existedParams = existedParams(),
        .allowNameEditor = true,
        .allowSavePreset = _customParams,
    });
    if (!ed.exec(tr("Create Parameter")))
        return;
    auto param = new Z::Parameter(ed.dim(), ed.alias(), ed.label(), ed.name(), ed.descr());
    addParam(param);
    if (ed.needSavePreset())
        emit onSavePreset(param);
}

void ParamSpecsEditor::editParamDlg()
{
    auto paramEditor = _editorParams->selectedEditor();
    if (!paramEditor)
        return;
    auto param = paramEditor->parameter();
    if (_customParams && !param->hasOption(Z::ParamOption::Custom))
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

void ParamSpecsEditor::removeParamDlg()
{
    auto editor = _editorParams->selectedEditor();
    if (!editor)
        return;
    auto param = editor->parameter();
    if (_customParams && !param->hasOption(Z::ParamOption::Custom))
        return;
    if (!Ori::Dlg::yes(tr("Remove parameter <b>%1</b> ?").arg(param->alias())))
        return;

    _editorParams->removeEditor(param);
    
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
