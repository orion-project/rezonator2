#include "TableFuncWindow.h"

#include "../funcs/TableFunction.h"

static QMap<QString, int> __windowIndeces;

TableFuncWindow::TableFuncWindow(TableFunction* func) : SchemaMdiChild(func->schema()), _function(func)
{
    _windowIndex = __windowIndeces[function()->name()];
    __windowIndeces[function()->name()] = _windowIndex+1;

    setTitleAndIcon(displayWindowTitle(), function()->iconPath());
}

TableFuncWindow::~TableFuncWindow()
{
    delete _function;
}

QString TableFuncWindow::displayWindowTitle() const
{
    if (_windowIndex > 0)
        return QString("%1 (%2)").arg(function()->name()).arg(_windowIndex);
    return function()->name();
}

void TableFuncWindow::update()
{
    // TODO
}
