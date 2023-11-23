#ifndef MULTI_BEAM_CAUSTIC_WINDOW_H
#define MULTI_BEAM_CAUSTIC_WINDOW_H

#include "../funcs/MulticausticWindow.h"
#include "../math/MultibeamCausticFunction.h"

class MultibeamCausticWindow final : public MulticausticWindow
{
    Q_OBJECT

public:
    explicit MultibeamCausticWindow(Schema*);

    MultibeamCausticFunction* function() const { return dynamic_cast<MultibeamCausticFunction*>(_function); }

    void pumpCreated(Schema*, PumpParams*) override { update(); }
    void pumpChanged(Schema*, PumpParams* p) override;
    void pumpCustomized(Schema*, PumpParams* p) override;
    void pumpDeleting(Schema*, PumpParams*) override { update(); }
    void pumpDeleted(Schema*, PumpParams*) override { update(); }

protected:
    // Implementation of PlotFuncWindow
    void calculate() override;
    void getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values) override;
    void prepareSpecPoints() override;
    void afterGraphFormatted(FunctionGraph*) override;

private:
    PumpParams* _lastSelectedPump = nullptr;
    bool _skipRecoloring = false;

    Z::WorkPlane workPlane() const;
};

#endif // MULTI_BEAM_CAUSTIC_WINDOW_H
