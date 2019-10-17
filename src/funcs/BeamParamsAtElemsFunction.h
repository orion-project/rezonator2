#ifndef BEAMPARAMS_AT_ELEMS_FUNCTION_H
#define BEAMPARAMS_AT_ELEMS_FUNCTION_H

#include "TableFunction.h"

class BeamParamsAtElemsFunction : public TableFunction
{
public:
    FUNC_ALIAS("BeamParamsAtElems")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Beam Parameters at Elements"))

    BeamParamsAtElemsFunction(Schema *schema);
};

#endif // BEAMPARAMS_AT_ELEMS_FUNCTION_H
