#ifndef MULTI_BEAM_CAUSTIC_WINDOW_H
#define MULTI_BEAM_CAUSTIC_WINDOW_H

#include "MulticausticWindow.h"
#include "../funcs/MultibeamCausticFunction.h"

class MultibeamCausticWindow final : public MulticausticWindow
{
    Q_OBJECT

public:
    explicit MultibeamCausticWindow(Schema*);

    MultibeamCausticFunction* function() const { return dynamic_cast<MultibeamCausticFunction*>(_function); }

protected:
    // Implementation of PlotFuncWindow
    void calculate() override;
    QString getDefaultTitle() const override;
    QString getDefaultTitleY() const override;
};

#endif // MULTI_BEAM_CAUSTIC_WINDOW_H
