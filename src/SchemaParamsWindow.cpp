#include "SchemaParamsWindow.h"

#include "CustomPrefs.h"
#include "WindowsManager.h"
#include "widgets/Appearance.h"
#include "widgets/SchemaParamsTable.h"
#include "widgets/FormulaEditor.h"
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
        if (schema()->params()->byAlias(alias))
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
        schema()->params()->append(param);

        CustomPrefs::setRecentDim("global_param_dim", dim);

        schema()->events().raise(SchemaEvents::CustomParamCreated, param);

        _isSettingValueForNewParam = true;
        QTimer::singleShot(100, [&](){ setParameterValue(); });
    }
}

void SchemaParamsWindow::deleteParameter()
{
    auto param = _table->selected();
    if (!param) return;

    // TODO search for schema()->paramLinks()->bySource(param) and remove links
    // TODO remove parameter

    schema()->events().raise(SchemaEvents::CustomParamDeleted, param);
}

void SchemaParamsWindow::setParameterValue()
{
    auto param = _table->selected();
    if (!param) return;

    FormulaEditor editor(param, schema()->formulas());
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
