#include "FunctionUtils.h"

#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../core/Schema.h"
#include "../core/Elements.h"

namespace FunctionUtils {

QString preparePumpCalculator(Schema* schema, PumpParams* useThisPump, Z::PairTS<std::shared_ptr<PumpCalculator>>& pumpCalc)
{
    auto pump = useThisPump ? useThisPump : schema->activePump();
    if (!pump)
    {
        return "There is no active pump in the schema. "
            "Use 'Pumps' window to create a new pump or activate one of the existing ones.";
    }

    if (!pumpCalc.T) pumpCalc.T = PumpCalculator::T();
    if (!pumpCalc.S) pumpCalc.S = PumpCalculator::S();

    const double lambda = schema->wavelength().value().toSi();
    if (!pumpCalc.T->init(pump, lambda) ||
        !pumpCalc.S->init(pump, lambda))
    {
        return "Unsupported pump mode";
    }

    return QString();
}

void prepareDynamicElements(Schema* schema, Element* stopElem, const Z::PairTS<std::shared_ptr<PumpCalculator>>& pumpCalc)
{
    const double lambda = schema->wavelength().value().toSi();

    for (int i = 0; i < schema->count(); i++)
    {
        auto elem = schema->element(i);
        if (elem == stopElem) break;
        auto dynamic = dynamic_cast<ElementDynamic*>(elem);
        if (!dynamic) continue;

        if (i == 0)
        {
            Z::Matrix unity;
            ElementDynamic::CalcParams p;
            p.Mt = &unity;
            p.Ms = &unity;
            p.pumpCalcT = pumpCalc.T.get();
            p.pumpCalcS = pumpCalc.S.get();
            p.schemaWavelenSI = lambda;
            p.prevElemIor = 1;
            dynamic->calcDynamicMatrix(p);
        }
        else
        {
            auto prevElem = schema->element(i - i);
            RoundTripCalculator calc(schema, prevElem);
            calc.calcRoundTrip();
            calc.multMatrix();
            ElementDynamic::CalcParams p;
            p.Mt = calc.pMt();
            p.Ms = calc.pMs();
            p.pumpCalcT = pumpCalc.T.get();
            p.pumpCalcS = pumpCalc.S.get();
            auto medium = dynamic_cast<ElemMediumRange*>(prevElem);
            p.prevElemIor = medium ? medium->ior() : 1;
            p.schemaWavelenSI = lambda;
            dynamic->calcDynamicMatrix(p);
        }
    }
}

} // namespace FunctionUtils

