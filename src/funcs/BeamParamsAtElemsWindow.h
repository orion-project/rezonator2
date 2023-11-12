#ifndef BEAM_PARAMS_AT_ELEMS_WINDOW_H
#define BEAM_PARAMS_AT_ELEMS_WINDOW_H

#include "../funcs/TableFuncWindow.h"
#include "../math/BeamParamsAtElemsFunction.h"

class BeamParamsAtElemsWindow final : public TableFuncWindow
{
    Q_OBJECT

public:
    explicit BeamParamsAtElemsWindow(Schema*);

    BeamParamsAtElemsFunction* function() const { return dynamic_cast<BeamParamsAtElemsFunction*>(_function); }
};

#endif // BEAM_PARAMS_AT_ELEMS_WINDOW_H
