#ifndef CUSTOM_TABLE_WINDOW_H
#define CUSTOM_TABLE_WINDOW_H

#include "../funcs/TableFuncWindow.h"
#include "../math/CustomTableFunction.h"

class CustomTableWindow final : public TableFuncWindow
{
    Q_OBJECT

public:
    explicit CustomTableWindow(Schema*);
    
    CustomTableFunction* function() const { return dynamic_cast<CustomTableFunction*>(_function); }
};

#endif // CUSTOM_TABLE_WINDOW_H
