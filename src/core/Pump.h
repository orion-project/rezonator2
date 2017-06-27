#ifndef PUMP_PARAMS_H
#define PUMP_PARAMS_H

//#include "Types.h"
//#include "../../orion/core/Templates.h"

//#include <QString>

namespace Z {
namespace Pump {
/*
DECLARE_ENUM(PumpMode, 0,
             PumpMode_waist,
             PumpMode_front,
             PumpMode_complex,
             PumpMode_icomplex,
             PumpMode_vector,
             PumpMode_sections)

class Raw
{
public:
    ValueTS param1;
    ValueTS param2;
    ValueTS param3;

    virtual QString verify() const { return QString(); }
};

#define DECLARE_PUMP_PARAMS(class_name, param1_name, param2_name, param3_name)\
    class class_name : public Raw\
    {\
    public:\
        const ValueTS& param1_name() const { return param1; }\
        const ValueTS& param2_name() const { return param2; }\
        const ValueTS& param3_name() const { return param3; }\
        QString verify() const override;\
    };

DECLARE_PUMP_PARAMS(Waist, radius, distance, mi)
DECLARE_PUMP_PARAMS(Front, radius, curvature, mi)
DECLARE_PUMP_PARAMS(Complex, re, im, mi)
DECLARE_PUMP_PARAMS(Icomplex, re, im, mi)
DECLARE_PUMP_PARAMS(Vector, radius, angle, distance)
DECLARE_PUMP_PARAMS(Sections, radius_1, radius_2, distance)
*/
class Params
{
public:
    Params();

//    PumpMode mode;

//    Waist waist;
//    Front front;
//    Complex complex;
//    Icomplex icomplex;
//    Vector vector;
//    Sections sections;

//    const Raw& current() const;

//    QString verify() const;
};

} // namespace Pump
} // namespace Z

#endif // PUMP_PARAMS_H
