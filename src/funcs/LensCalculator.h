#ifndef LENS_CALCULATOR_H
#define LENS_CALCULATOR_H

#include "core/OriFloatingPoint.h"

/*                      Lens
                     |<-- T -->|
              R1 --->|/////////|<--- R2
  Front              |/// n ///|              Rear
  focus              |/////////|              focus
  ----*--------------+/-/-/-/-/+--------------*-------- Axis
      |<----- F1 --->|/////////|<----- F2 --->|
      |                |     |                |
      |<------ F ----->|     |<------ F ----->|
      |                |     |                |
      F                H     H'               F'
                   Front     Rear
               principal     principal
                   plane     plane
*/

struct LensCalculator
{
    // Distance between surfaces on axis
    double T;

    // Lens material IOR
    // We only calculate lens in air
    double n;

    // Left ROC
    // Negative value means right-bulged surface, positive value means left-bulged surface
    double R1;

    // Right ROC
    // Negative value means right-bulged surface, positive value means left-bulged surface
    double R2;

    // Focal range - distance between focal point and cardinal plane
    // There is no difference between front and rear for lens in air
    double F;

    // Optical power
    double P;

    // Front focal range
    // Distance from the front focal point to the left surface
    double F1;

    // Rear focal range
    // Distance from the rear focal point to the right surface
    double F2;

    bool planar = false;

    void calc()
    {
        bool plane1 = Double(R1).is(0);
        bool plane2 = Double(R2).is(0);

        if (plane1 && plane2)
        {
            P = 0;
            F = F2 = F1 = Double::infinity();
            planar = true;
            return;
        }

        planar = false;

        if (plane1)
        {
            P = -(n - 1)/R2;
            F = 1/P;
            F1 = -F * (1 + (n-1)*T/n/R2);
            F2 =  F;
            return;
        }

        if (plane2)
        {
            P = (n - 1)/R1;
            F = 1/P;
            F1 = -F;
            F2 =  F * (1 - (n-1)*T/n/R1);
            return;
        }

        P = (n - 1)*(1/R1 - 1/R2 + (n-1)*T/n/R1/R2);
        F = 1/P;
        F1 = -F * (1 + (n-1)*T/n/R2);
        F2 =  F * (1 - (n-1)*T/n/R1);
    }
};

#endif // LENS_CALCULATOR_H
