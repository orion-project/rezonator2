#ifndef FUNCTION_UTILS_H
#define FUNCTION_UTILS_H

class Element;
class PumpCalculator;
class Schema;

namespace FunctionUtils {

/// Function is used in SP schemas in Caustic and Beam Params functions
/// Disabled elements will never be passed as stopElem
void prepareDynamicElements(Schema* schema, Element* stopElem, PumpCalculator* pumpCalc);

} // namespace FunctionUtils

#endif // FUNCTION_UTILS_H
