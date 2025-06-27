#include "CustomTableWindow.h"

CustomTableWindow::CustomTableWindow(Schema* schema):
    TableFuncWindow(new CustomTableFunction(schema))
{
}
