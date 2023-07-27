#include "SchemaParamsWindow.h"

#include "AdjustmentWindow.h"
#include "Appearance.h"
#include "CustomPrefs.h"
#include "HelpSystem.h"
#include "WindowsManager.h"
#include "widgets/SchemaParamsTable.h"
#include "widgets/ParamEditorEx.h"
#include "widgets/UnitWidgets.h"

#include "helpers/OriWidgets.h"
#include "helpers/OriDialogs.h"

#include <QAction>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QToolButton>

namespace SchemaParamsWindowStorable
{
    SchemaWindow* createWindow(Schema* schema)
    {
        return SchemaParamsWindow::create(schema);
    }
} // namespace SchemaParamsWindowStorable

//------------------------------------------------------------------------------
//                             SchemaParamsWindow
//------------------------------------------------------------------------------

SchemaParamsWindow* SchemaParamsWindow::_instance = nullptr;

SchemaParamsWindow* SchemaParamsWindow::create(Schema* owner)
{
    if (!_instance)
        _instance = new SchemaParamsWindow(owner);
    return _instance;
}

SchemaParamsWindow::SchemaParamsWindow(Schema *owner) : SchemaMdiChild(owner)
{
    setTitleAndIcon(tr("Parameters"), ":/window_icons/parameter");

    _table = new SchemaParamsTable(owner);
    setContent(_table);

    createActions();
    createMenuBar();
    createToolBar();

    _table->setContextMenu(_contextMenu);
    connect(_table, SIGNAL(doubleClicked(Z::Parameter*)), this, SLOT(setParameterValue()));
    schema()->registerListener(_table);
}

SchemaParamsWindow::~SchemaParamsWindow()
{
    _instance = nullptr;

    schema()->unregisterListener(_table);
}

void SchemaParamsWindow::createActions()
{
    #define A_ Ori::Gui::action

    _actnParamAdd = A_(tr("Create..."), this, SLOT(createParameter()), ":/toolbar/param_add", Qt::CTRL | Qt::Key_Insert);
    _actnParamDelete = A_(tr("Delete..."), this, SLOT(deleteParameter()), ":/toolbar/param_delete", Qt::CTRL | Qt::Key_Delete);
    _actnParamSet = A_(tr("Set..."), this, SLOT(setParameterValue()), ":/toolbar/param_set");
    _actnParamDescr = A_(tr("Annotate..."), this, SLOT(annotateParameter()), ":/toolbar/param_annotate", Qt::CTRL | Qt::Key_Return);
    _actnParamAdjust = A_(tr("Adjust"), this, SLOT(adjustParameter()), ":/toolbar/adjust");

    #undef A_
}

void SchemaParamsWindow::createMenuBar()
{
    _windowMenu = Ori::Gui::menu(tr("Parameter"), this,
        { _actnParamAdd, nullptr, _actnParamSet, _actnParamDescr, nullptr, _actnParamAdjust, nullptr, _actnParamDelete });

    _contextMenu = Ori::Gui::menu(this,
        { _actnParamSet, _actnParamDescr, nullptr, _actnParamAdjust, nullptr, _actnParamDelete });
}

void SchemaParamsWindow::createToolBar()
{
    populateToolbar({ Ori::Gui::textToolButton(_actnParamAdd), nullptr,
        Ori::Gui::textToolButton(_actnParamSet), _actnParamDescr, nullptr, _actnParamDelete });
}

void SchemaParamsWindow::createParameter()
{
    auto recentDim = CustomPrefs::recentDim("global_param_dim");
    auto recentUnit = CustomPrefs::recentUnit("global_param_unit", recentDim);

    auto aliasEditor = new QLineEdit;
    aliasEditor->setFont(Z::Gui::ValueFont().get());

    auto unitEditor = new UnitComboBox;
    unitEditor->canSelectFixedUnit = true;
    unitEditor->setFixedWidth(QWIDGETSIZE_MAX);
    unitEditor->populate(recentDim);
    unitEditor->setSelectedUnit(recentUnit);

    QHash<Z::Dim, Z::Unit> recentUnits;

    auto dimEditor = new DimComboBox;
    dimEditor->setSelectedDim(recentDim);
    connect(dimEditor, &DimComboBox::dimChanged, [&](Z::Dim dim){
        recentUnits[recentDim] = unitEditor->selectedUnit();
        if (!recentUnits.contains(dim))
            recentUnits[dim] = CustomPrefs::recentUnit("global_param_unit", dim);
        unitEditor->populate(dim);
        unitEditor->setSelectedUnit(recentUnits[dim]);
        recentDim = dim;
    });

    QWidget editor;
    auto layout = new QFormLayout(&editor);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addRow(new QLabel(tr("Name")), aliasEditor);
    layout->addRow(new QLabel(tr("Dim")), dimEditor);
    layout->addRow(new QLabel(tr("Unit")), unitEditor);

    auto verifyFunc = [&](){
        auto alias = aliasEditor->text().trimmed();
        if (alias.isEmpty())
            return tr("Parameter name can't be empty");
        if (schema()->customParams()->byAlias(alias))
            return tr("Parameter <b>%1</b> already exists").arg(alias);
        if (!Z::FormulaUtils::isValidVariableName(alias))
            return tr("Parameter name <b>%1</b> is invalid").arg(alias);
        return QString();
    };

    if (Ori::Dlg::Dialog(&editor, false)
                .withTitle(tr("Create Parameter"))
                .withIconPath(":/window_icons/parameter")
                .withContentToButtonsSpacingFactor(3)
                .withVerification(verifyFunc)
                .exec())
    {
        auto dim = dimEditor->selectedDim();
        auto alias = aliasEditor->text().trimmed();
        auto label = alias;
        auto name = alias;
        auto param = new Z::Parameter(dim, alias, label, name);
        auto unit = unitEditor->selectedUnit();
        param->setValue(Z::Value(0, unit));
        schema()->customParams()->append(param);

        CustomPrefs::setRecentDim("global_param_dim", dim);
        CustomPrefs::setRecentUnit("global_param_unit", unit);

        schema()->events().raise(SchemaEvents::CustomParamCreated, param, "Params window: param created");

        _isSettingValueForNewParam = true;
        QTimer::singleShot(100, this, [&](){ setParameterValue(); });
    }
}

void SchemaParamsWindow::deleteParameter()
{
    auto deletingParam = _table->selected();
    if (!deletingParam) return;

    // Check if there are dependent parameters via formulas
    Z::Parameters dependentParams = schema()->formulas()->dependentParams(deletingParam);
    if (!dependentParams.isEmpty())
    {
        QStringList dependentAliases;
        for (Z::Parameter *param : dependentParams)
            dependentAliases << "<b>" % param->alias() % "</b>";
        return Ori::Dlg::info(
            tr("Can't delete paremeter <b>%1</b> because there are global parameters depending on it:<br><br>%2")
                .arg(deletingParam->alias(), dependentAliases.join(", ")));
    }

    // Check if there are dependent parameters via links
    Z::ParamLink *dependentLink = schema()->paramLinks()->bySource(deletingParam);
    if (dependentLink)
    {
        QStringList dependentParams;
        for (Element *elem : schema()->elements())
            for (Z::Parameter *param : elem->params())
                if (param == dependentLink->target())
                    dependentParams << tr("Element <b>%1</b>, parameter <b>%2</b>").arg(elem->label(), param->label());
        if (!dependentParams.isEmpty())
            return Ori::Dlg::info(
                tr("Can't delete global parameter <b>%1</b><br>because some elements' parameters depend on it:<br><br>%2")
                    .arg(deletingParam->alias(), dependentParams.join("<br>")));
    }

    if (Ori::Dlg::yes(tr("Delete parameter<b>%1</b>?").arg(deletingParam->alias())))
    {
        schema()->events().raise(SchemaEvents::CustomParamDeleting, deletingParam, "Params window: param deleting");
        schema()->formulas()->free(deletingParam);
        schema()->customParams()->removeOne(deletingParam);
        schema()->events().raise(SchemaEvents::CustomParamDeleted, deletingParam, "Params window: param deleted");
    }
}

void SchemaParamsWindow::setParameterValue()
{
    auto param = _table->selected();
    if (!param) return;

    auto globalParams = schema()->globalParams();
    ParamEditorEx editor(param, schema()->formulas(), &globalParams);
    bool ok = Ori::Dlg::Dialog(&editor, false)
                .withTitle(tr("Set Value"))
                .withIconPath(":/window_icons/parameter")
                .withOnHelp([]{
                    Z::HelpSystem::instance()->showTopic("params_window.html#params-window-value");
                })
                .withContentToButtonsSpacingFactor(2)
                .connectOkToContentApply()
                .exec();
    if (ok)
    {
        schema()->events().raise(SchemaEvents::CustomParamChanged, param, "Params window: param value set");
        schema()->events().raise(SchemaEvents::RecalRequred, "Params window: param value set");
    }

    if (_isSettingValueForNewParam)
    {
        _isSettingValueForNewParam = false;
        CustomPrefs::setRecentUnit("global_param_unit", param->value().unit());
    }
}

void SchemaParamsWindow::annotateParameter()
{
    auto param = _table->selected();
    if (!param) return;

    bool ok;
    QString descr = Ori::Dlg::inputText(tr("Annotation:"), param->description(), &ok);
    if (ok)
    {
        param->setDescription(descr);
        schema()->events().raise(SchemaEvents::CustomParamEdited, param, "Params window: param annotated");
    }
}

void SchemaParamsWindow::adjustParameter()
{
    auto param = _table->selected();
    if (!param) return;

    AdjustmentWindow::adjust(schema(), param);
}

void SchemaParamsWindow::shortcutEnterPressed()
{
    setParameterValue();
}
