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

    void pumpCreated(Schema*, PumpParams*) override { update(); }
    void pumpChanged(Schema*, PumpParams* p) override;
    void pumpDeleting(Schema*, PumpParams*) override { update(); }
    void pumpDeleted(Schema*, PumpParams*) override { update(); }

protected:
    // Implementation of PlotFuncWindow
    void calculate() override;
};

#endif // MULTI_BEAM_CAUSTIC_WINDOW_H
