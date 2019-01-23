#ifndef BEAMVSSTABWINDOW_H
#define BEAMVSSTABWINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../funcs/BeamOverStabFunction.h"

class BeamOverStabWindow : public PlotFuncWindowStorable
{
public:
    BeamOverStabWindow(Schema*);

    BeamOverStabFunction* function() const { return dynamic_cast<BeamOverStabFunction*>(_function); }

protected:
    // Implementation of PlotFuncWindow
    bool configureInternal() override;
};

#endif // BEAMVSSTABWINDOW_H
