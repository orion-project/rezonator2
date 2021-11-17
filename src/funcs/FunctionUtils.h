#ifndef FUNCTION_UTILS_H
#define FUNCTION_UTILS_H

#include "../core/Values.h"

#include <memory>

class Element;
class PumpCalculator;
class PumpParams;
class Schema;

namespace FunctionUtils {

QString preparePumpCalculator(Schema* schema,
                              PumpParams* useThisPump,
                              Z::PairTS<std::shared_ptr<PumpCalculator>>& pumpCalc,
                              bool writeProtocol = false);

/// Function is used in SP schemas in Caustic and Beam Params functions
/// Disabled elements will never be passed as stopElem
/// because the above functions are not calcxlated for disableds
void prepareDynamicElements(Schema* schema, Element* stopElem, const Z::PairTS<std::shared_ptr<PumpCalculator> > &pumpCalc);

} // namespace FunctionUtils

#endif // FUNCTION_UTILS_H
