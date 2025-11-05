#ifndef FUNCTION_UTILS_H
#define FUNCTION_UTILS_H

class Element;
class PumpCalculator;
class Schema;

namespace FunctionUtils {

/// The function calculates matrices of elements that depend on beam parameters before the element.
/// Function is used in SP schemas in Caustic and Beam Params functions
/// Disabled elements will never be passed as stopElem
void prepareDynamicElements(Schema* schema, Element* stopElem, PumpCalculator* pumpCalc);

/// Returns an element which is the previous to the given one
/// respecting round-trip rules for different schema kinds (SW, SP, RR)
Element* prevElem(Schema *schema, Element *elem);

/// Returns an element which is the next to the given one
/// respecting round-trip rules for different schema kinds (SW, SP, RR)
Element* nextElem(Schema *schema, Element *elem);

/// Returns IOR that should be used for calculation of the beamsize
/// when the round-trip is calculated against the given element.
/// For example, if the given element is an interface,
/// the IOR should be that of the next neighbour medium.
double ior(Schema *schema, Element *elem, bool splitRange);

} // namespace FunctionUtils

#endif // FUNCTION_UTILS_H
