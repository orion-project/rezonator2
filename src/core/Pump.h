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
   ValueTS distance = ValueTS(100, 100, Units::mm());
   ValueTS waist = ValueTS(100, 100, Units::mkm());
   PointTS MI = PointTS(1, 1);
};

class FrontParams : public PumpParams
{
public:
   ValueTS beamRadius = ValueTS(1000, 1000, Units::mkm());
   ValueTS frontRadius = ValueTS(100, 100, Units::mm());
   PointTS MI = PointTS(1, 1);
};

class TwoSectionsParams : public PumpParams
{
public:
    ValueTS radius1 = ValueTS(100, 100, Units::mkm());
    ValueTS radius2 = ValueTS(1000, 1000, Units::mkm());
    ValueTS distance = ValueTS(100, 100, Units::mm());
};

class RayVectorParams : public PumpParams
{
public:
    ValueTS radius = ValueTS(100, 100, Units::mkm());
    ValueTS angle = ValueTS(10, 10, Units::deg());
    ValueTS distance = ValueTS(100, 100, Units::mm());
};

class ComplexParams : public PumpParams
{
public:
    PointTS real = PointTS(0, 0);
    PointTS imag = PointTS(0, 0);
    PointTS MI = PointTS(1, 1);
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
