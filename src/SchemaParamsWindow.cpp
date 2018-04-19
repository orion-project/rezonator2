#include "SchemaParamsWindow.h"

#include "CustomPrefs.h"
#include "WindowsManager.h"
#include "widgets/Appearance.h"
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

    _actnParamAdd = A_(tr("&Create..."), this, SLOT(createParameter()), ":/toolbar/param_add", Qt::CTRL | Qt::Key_Insert);
    _actnParamDelete = A_(tr("&Delete"), this, SLOT(deleteParameter()), ":/toolbar/param_delete", Qt::CTRL | Qt::Key_Delete);
    _actnParamSet = A_(tr("&Set..."), this, SLOT(setParameterValue()), ":/toolbar/param_set", Qt::Key_Enter);

    #undef A_
}

void SchemaParamsWindow::createMenuBar()
{
    _windowMenu = Ori::Gui::menu(tr("&Parameter"), this,
        { _actnParamAdd, _actnParamSet, 0, _actnParamDelete });

    _contextMenu = Ori::Gui::menu(this,
        { _actnParamSet, 0, _actnParamDelete });
}

void SchemaParamsWindow::createToolBar()
{
    populateToolbar({ Ori::Gui::textToolButton(_actnParamAdd),
        Ori::Gui::textToolButton(_actnParamSet), 0, _actnParamDelete });
}

void SchemaParamsWindow::createParameter()
{
    auto aliasEditor = new QLineEdit;
    Z::Gui::setValueFont(aliasEditor);

    auto dimEditor = new DimComboBox;
    dimEditor->setSelectedDim(CustomPrefs::recentDim("global_param_dim"));

    QWidget editor;
    auto layout = new QFormLayout(&editor);
    layout->setMargin(0);
    layout->addRow(new QLabel(tr("Name")), aliasEditor);
    layout->addRow(new QLabel(tr("Dim")), dimEditor);

    auto verifyFunc = [&](){
        auto alias = aliasEditor->text().trimmed();
        if (alias.isEmpty())
            return tr("Parameter name can't be empty");
        if (schema()->customParams()->byAlias(alias))
            return tr("Parameter '%1' already exists").arg(alias);
        if (!Z::FormulaUtils::isValidVariableName(alias))
            return tr("Parameter name '%1' is invalid").arg(alias);
        return QString();
    };

    if (Ori::Dlg::Dialog(&editor)
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
        auto unit = CustomPrefs::recentUnit("global_param_unit", dim);
        param->setValue(Z::Value(0, unit));
        schema()->customParams()->append(param);

        CustomPrefs::setRecentDim("global_param_dim", dim);

        schema()->events().raise(SchemaEvents::CustomParamCreated, param);

        _isSettingValueForNewParam = true;
        QTimer::singleShot(100, [&](){ setParameterValue(); });
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
            tr("Can't delete paremeter '%1' because there are global parameters depending on it:<br><br>%2")
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
                tr("Can't delete paremeter '%1' because there are element parameters depending on it:<br><br>%2")
                    .arg(deletingParam->alias(), dependentParams.join("<br>")));
    }

    if (Ori::Dlg::ok(tr("Confirm deletion of parameter '%1'").arg(deletingParam->alias())))
    {
        schema()->events().raise(SchemaEvents::CustomParamDeleting, deletingParam);
        schema()->formulas()->free(deletingParam);
        schema()->customParams()->removeOne(deletingParam);
        schema()->events().raise(SchemaEvents::CustomParamDeleted, deletingParam);
    }
}

void SchemaParamsWindow::setParameterValue()
{
    auto param = _table->selected();
    if (!param) return;

    auto globalParams = schema()->globalParams();
    ParamEditorEx editor(param, schema()->formulas(), &globalParams);
    bool ok = Ori::Dlg::Dialog(&editor)
                .withTitle(tr("Set value"))
                .withIconPath(":/window_icons/parameter")
                .withContentToButtonsSpacingFactor(2)
                .connectOkToContentApply()
                .exec();
    if (ok)
        schema()->events().raise(SchemaEvents::CustomParamChanged, param);

    if (_isSettingValueForNewParam)
    {
        _isSettingValueForNewParam = false;
        CustomPrefs::setRecentUnit("global_param_unit", param->value().unit());
    }
}
