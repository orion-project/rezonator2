#ifndef PUMP_PARAMS_H
#define PUMP_PARAMS_H

#include "Values.h"

#include <QObject>

namespace Z {
namespace Pump {

class PumpParams
{
public:
    virtual QString verify() const { return QString(); }
};

class WaistParams : public PumpParams
{
public:
   ValueTS distance;
   ValueTS waist;
   PointTS MI;
};

class FrontParams : public PumpParams
{
public:
   ValueTS distance;
   ValueTS beamsize;
   PointTS MI;
};

class TwoSectionsParams : public PumpParams
{
public:
    ValueTS radius1;
    ValueTS radius2;
    ValueTS distance;
};

class RayVectorParams : public PumpParams
{
public:
    ValueTS radius;
    ValueTS angle;
    ValueTS distance;
};

class ComplexParams : public PumpParams
{
public:
    PairTS<DoublePoint> param;
    PointTS MI;
};

class Params
{
    Q_GADGET

public:
    enum Mode
    {
        Waist, Front, TwoSections, RayVector, Complex, InvComplex
    };
    Q_ENUM(Mode)

    Params() {}

    Mode mode = Waist;

    WaistParams waist;
    FrontParams front;
    ComplexParams complex;
    ComplexParams icomplex;
    RayVectorParams vector;
    TwoSectionsParams sections;
};

} // namespace Pump
} // namespace Z

#endif // PUMP_PARAMS_H
