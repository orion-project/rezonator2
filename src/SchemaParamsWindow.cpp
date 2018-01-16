#include "SchemaParamsWindow.h"

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
}

SchemaParamsWindow::~SchemaParamsWindow()
{
    _instance = nullptr;
}
