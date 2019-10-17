#ifndef BEAM_PARAMS_AT_ELEMS_FUNCTION_H
#define BEAM_PARAMS_AT_ELEMS_FUNCTION_H

#include "TableFunction.h"

class BeamParamsAtElemsFunction : public TableFunction
{
public:
    FUNC_ALIAS("BeamParamsAtElems")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Beam Data"))
    FUNC_ICON(":/toolbar/func_beamdata")

    BeamParamsAtElemsFunction(Schema *schema);

    QVector<ColumnDef> columns() const override;
};

#endif // BEAM_PARAMS_AT_ELEMS_FUNCTION_H
