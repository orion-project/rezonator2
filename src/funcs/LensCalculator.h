#ifndef LENS_CALCULATOR_H
#define LENS_CALCULATOR_H

/*                      Lens
                     |<-- T -->|
              R1 --->|/////////|<--- R2
  Front              |/// n ///|              Rear
  focus              |/////////|              focus
  ----*--------------+/-/-/-/-/+--------------*-------- Axis
      |<----- FF --->|/////////|<----- BF --->|
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
    double FF;

    // Rear focal range
    // Distance from the rear focal point to the right surface
    double RF;

    void calc()
    {
        P = (n - 1)*(1/R1 - 1/R2 + (n-1)*T/n/R1/R2);
        F = 1/P;
        RF =  F * (1 - (n-1)*T/n/R1);
        FF = -F * (1 + (n-1)*T/n/R2);
    }
};

#endif // LENS_CALCULATOR_H
