#ifndef CAUSTIC_WINDOW_H
#define CAUSTIC_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../funcs_meat/CausticFunction.h"

class CausticWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit CausticWindow(Schema*);

    bool configure(QWidget* parent) override;

    void schemaParamsChanged(Schema*) override;

    CausticFunction* function() const { return (CausticFunction*)_function; }

protected:
    QWidget* makeOptionsPanel() override;
};

#endif // CAUSTIC_WINDOW_H
