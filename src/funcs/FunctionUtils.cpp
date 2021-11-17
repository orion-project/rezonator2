#include "FunctionUtils.h"

#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../core/Schema.h"
#include "../core/Elements.h"

namespace FunctionUtils {

QString preparePumpCalculator(Schema* schema,
                              PumpParams* useThisPump,
                              Z::PairTS<std::shared_ptr<PumpCalculator>>& pumpCalc,
                              bool writeProtocol)
{
    auto pump = useThisPump ? useThisPump : schema->activePump();
    if (!pump)
    {
        return "There is no active pump in the schema. "
            "Use 'Pumps' window to create a new pump or activate one of the existing ones.";
    }

    if (!pumpCalc.T) pumpCalc.T = PumpCalculator::T();
    if (!pumpCalc.S) pumpCalc.S = PumpCalculator::S();
    if (!pumpCalc.T->init(pump, schema->wavelenSi(), "T", writeProtocol) ||
        !pumpCalc.S->init(pump, schema->wavelenSi(), "S", writeProtocol))
    {
        return "Unsupported pump mode";
    }

    return QString();
}

void prepareDynamicElements(Schema* schema, Element* stopElem, const Z::PairTS<std::shared_ptr<PumpCalculator>>& pumpCalc)
{
    auto elems = schema->activeElements();
    for (int i = 0; i < elems.size(); i++)
    {
        auto elem = elems.at(i);
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
            p.schemaWavelenSi = schema->wavelenSi();
            p.prevElemIor = 1;
            dynamic->calcDynamicMatrix(p);
        }
        else
        {
            auto prevElem = elems.at(i - i);
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
            p.schemaWavelenSi = schema->wavelenSi();
            dynamic->calcDynamicMatrix(p);
        }
    }
}

} // namespace FunctionUtils

