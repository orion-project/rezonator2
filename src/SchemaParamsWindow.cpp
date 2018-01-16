#include "SchemaParamsWindow.h"
#include "widgets/SchemaParamsTable.h"

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
}

SchemaParamsWindow::~SchemaParamsWindow()
{
    _instance = nullptr;
}
