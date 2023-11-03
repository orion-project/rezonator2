#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../core/Schema.h"
#include "../core/Elements.h"

namespace FunctionUtils {

void prepareDynamicElements(Schema* schema, Element* stopElem, PumpCalculator* pumpCalc)
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
            p.pumpCalc = pumpCalc;
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
            p.pumpCalc = pumpCalc;
            auto medium = dynamic_cast<ElemMediumRange*>(prevElem);
            p.prevElemIor = medium ? medium->ior() : 1;
            p.schemaWavelenSi = schema->wavelenSi();
            dynamic->calcDynamicMatrix(p);
        }
    }
}

} // namespace FunctionUtils
