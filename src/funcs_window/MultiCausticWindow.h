#ifndef MULTI_CAUSTIC_WINDOW_H
#define MULTI_CAUSTIC_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../funcs/MultiCausticFunction.h"

class MultiCausticWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit MultiCausticWindow(Schema*);

    MultiCausticFunction* function() const { return (MultiCausticFunction*)_function; }

protected:
    // Implementation of PlotFuncWindow
    bool configureInternal(QWidget* parent) override;
};

#endif // MULTI_CAUSTIC_WINDOW_H
