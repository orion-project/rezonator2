#ifndef PUMP_CALCULATOR_H
#define PUMP_CALCULATOR_H

#include "../core/Math.h"
#include "../core/Values.h"

namespace Z {
class PumpParams_Waist;
class PumpParams_Front;
class PumpParams_RayVector;
class PumpParams_TwoSections;
class PumpParams_Complex;
class PumpParams_InvComplex;
}

class PumpCalculator
{
public:
   static Z::PairTS<Z::Complex> calcFront(Z::PumpParams_Waist *pump, double lambda);
   static Z::PairTS<Z::Complex> calcFront(Z::PumpParams_Front *pump, double lambda);
   static Z::PairTS<Z::RayVector> calcFront(Z::PumpParams_RayVector *pump);
   static Z::PairTS<Z::RayVector> calcFront(Z::PumpParams_TwoSections *pump);
   static Z::PairTS<Z::Complex> calcFront(Z::PumpParams_Complex *pump);
   static Z::PairTS<Z::Complex> calcFront(Z::PumpParams_InvComplex *pump);
};

#endif // PUMP_CALCULATOR_H
