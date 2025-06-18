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

    QVector<TableFunction::ColumnDef> columns() const override;
    int columnCount() const override;
    QString columnTitle(int colIndex) const override;

protected:
    QVector<Z::PointTS> calculatePumpBeforeSchema() override;
    QVector<Z::PointTS> calculateSinglePass(RoundTripCalculator* calc, double ior) const override;
    QVector<Z::PointTS> calculateResonator(RoundTripCalculator* calc, double ior) const override;
};

#endif // BEAM_PARAMS_AT_ELEMS_FUNCTION_H
