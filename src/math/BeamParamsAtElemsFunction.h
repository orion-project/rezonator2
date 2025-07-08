#ifndef BEAM_PARAMS_AT_ELEMS_FUNCTION_H
#define BEAM_PARAMS_AT_ELEMS_FUNCTION_H

#include "TableFunction.h"

class BeamParamsAtElemsFunction : public TableFunction
{
public:
    FUNC_ALIAS("BeamParamsAtElems")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Beam Data"))
    FUNC_ICON(":/toolbar/func_beamdata")
    FUNC_HELP("func_beamdata.html")

    BeamParamsAtElemsFunction(Schema *schema);

protected:
    QVector<Z::PointTS> calculatePumpBeforeSchema(Element *elem) override;
    QVector<Z::PointTS> calculateSinglePass(Element *elem, RoundTripCalculator* calc, double ior) override;
    QVector<Z::PointTS> calculateResonator(Element *elem, RoundTripCalculator* calc, double ior) override;
    
private:
    Z::PointTS calcApertureRatio(const Z::PointTS &beamRadius, Element *elem) const;
};

#endif // BEAM_PARAMS_AT_ELEMS_FUNCTION_H
