#ifndef LENS_CALCULATOR_H
#define LENS_CALCULATOR_H

struct LensCalculator
{
    double T;
    double n;
    double R1;
    double R2;

    double F; // Focus range
    double P; // Optical power

    void calc()
    {
        P = (n - 1)*(1/R1 - 1/R2 + (n-1)*T/n/R1/R2);
        F = 1/P;
    }
};

#endif // LENS_CALCULATOR_H
