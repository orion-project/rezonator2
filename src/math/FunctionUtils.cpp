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
        auto dynamic = elem->asDynamic();
        if (!dynamic) continue;

        if (i == 0)
        {
            Z::Matrix unity;
            DynamicElemCalcParams p;
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
            calc.multMatrix("global::prepareDynamicElements");
            DynamicElemCalcParams p;
            p.Mt = calc.pMt();
            p.Ms = calc.pMs();
            p.pumpCalc = pumpCalc;
            // TODO: adjust for custom medium elements
            auto medium = Z::Utils::asMedium(prevElem);
            p.prevElemIor = medium ? medium->ior() : 1;
            p.schemaWavelenSi = schema->wavelenSi();
            dynamic->calcDynamicMatrix(p);
        }
    }
}

Element* prevElem(Schema *schema, Element *elem)
{
    auto elems = schema->activeElements();
    if (elems.size() < 2)
        return nullptr;
    int index = elems.indexOf(elem);
    if (index < 0)
        return nullptr;
    int prev = -1;
    bool isFirst = index == 0;
    switch (schema->tripType())
    {
    case TripType::SP:
        prev = isFirst ? -1 : index-1;
        break;
    case TripType::SW:
        prev = isFirst ? index+1 : index-1;
        break;
    case TripType::RR:
        prev = isFirst ? elems.size()-1 : index-1;
        break;
    }
    if (prev >= 0)
        return elems.at(prev);
    return nullptr;
}

Element* nextElem(Schema *schema, Element *elem)
{
    auto elems = schema->activeElements();
    if (elems.size() < 2)
        return nullptr;
    int index = elems.indexOf(elem);
    if (index < 0)
        return nullptr;
    int next = -1;
    bool isLast = index == elems.size()-1;
    switch (schema->tripType())
    {
    case TripType::SP:
        next = isLast ? -1 : index+1;
        break;
    case TripType::SW:
        next = isLast ? index-1 : index+1;
        break;
    case TripType::RR:
        next = isLast ? 0 : index+1;
        break;
    }
    if (next >= 0)
        return elems.at(next);
    return nullptr;
}

double ior(Schema *schema, Element *elem, bool splitRange)
{
    // TODO: adjust for custom medium elements
    if (auto medium = Z::Utils::asMedium(elem); medium)
        return medium->ior();

    if (auto range = elem->asRange(); range)
        return splitRange ? range->ior() : 1;
    
    if (Z::Utils::isSpace(elem))
        return 1;
    
    // TODO: custom element

    if (auto nextMedium = Z::Utils::asMedium(nextElem(schema, elem)); nextMedium)
        return nextMedium->ior();
    
    return 1;
}

} // namespace FunctionUtils
