#include "SchemaParamsWindow.h"
#include "widgets/SchemaParamsTable.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriDialogs.h"

#include <QAction>
#include <QToolButton>

SchemaParamsWindow* SchemaParamsWindow::_instance = nullptr;

SchemaParamsWindow* SchemaParamsWindow::create(Schema* owner)
{
    if (!_instance)
        _instance = new SchemaParamsWindow(owner);
    return _instance;
}

SchemaParamsWindow::SchemaParamsWindow(Schema *owner) : SchemaMdiChild(owner)
{
    setWindowTitle(tr("Parameters", "Window title"));
    setWindowIcon(QIcon(":/window_icons/parameter"));

    _table = new SchemaParamsTable(owner);

    setContent(_table);

    createActions();
    createMenuBar();
    createToolBar();

    connect(_table, SIGNAL(doubleClicked(Z::Parameter*)), this, SLOT(actionParamSet()));
    _table->setContextMenu(_contextMenu);
}

SchemaParamsWindow::~SchemaParamsWindow()
{
    _instance = nullptr;
}

void SchemaParamsWindow::createActions()
{
    #define A_ Ori::Gui::action

    _actnParamAdd = A_(tr("&Create..."), this, SLOT(actionParamAdd()), ":/toolbar/param_add", Qt::CTRL | Qt::Key_Insert);
    _actnParamDelete = A_(tr("&Delete"), this, SLOT(actionParamDelete()), ":/toolbar/param_delete", Qt::CTRL | Qt::Key_Delete);
    _actnParamSet = A_(tr("&Set..."), this, SLOT(actionParamSet()), ":/toolbar/param_set", Qt::Key_Enter);

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

void SchemaParamsWindow::actionParamAdd()
{
    QString alias = Ori::Dlg::inputText(tr("Parameter name"), QString());
    if (alias.isEmpty()) return;

    // TODO set dimension
    // TODO check alias uniqueness
    // TODO check alias should be valid identifier (as C++ variable name)

    auto param = new Z::Parameter(Z::Dims::none(), alias);
    param->setOwner(schema());
    param->setValue(Z::Value(0, Z::Units::none()));
    schema()->params()->append(param);
    _table->parameterCreated(param);
}

void SchemaParamsWindow::actionParamDelete()
{
    // TODO
}

void SchemaParamsWindow::actionParamSet()
{
    // TOOD
}
