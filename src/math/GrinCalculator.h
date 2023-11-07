#ifndef GRIN_CALCULATOR_H
#define GRIN_CALCULATOR_H

#include "core/OriResult.h"

namespace GrinCalculator {

using GrinResult = Ori::Result<double>;

GrinResult calc_focus(const double& L, const double& n0, const double& n2);
GrinResult solve_n2(const double& L, const double& n0, const double& F);

} // namespace GrinFocusCalculator

#endif // GRIN_FOCUS_CALCULATOR_H
