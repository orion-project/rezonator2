#ifndef CAUSTIC_WINDOW_H
#define CAUSTIC_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../funcs_meat/CausticFunction.h"

class CausticWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit CausticWindow(Schema*);

    void schemaParamsChanged(Schema*) override;

    CausticFunction* function() const { return (CausticFunction*)_function; }

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    bool configureInternal(QWidget* parent) override;
};

#endif // CAUSTIC_WINDOW_H
