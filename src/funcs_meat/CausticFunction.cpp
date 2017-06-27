#include <QApplication>
#include <QDebug>

#include "CausticFunction.h"
#include "../funcs/Calculator.h"
#include "../core/Protocol.h"

#include <math.h>

void CausticFunction::calculate()
{/* TODO
    Z_REPORT("CALCULATE" << alias())

    _elem = argElementRange();
    if (!_elem) return;

    _arg.start = 0;
    _arg.stop = _elem->axisLength();

    int points;
    double step;
    if (!prepareResults(points, step)) return;
    if (!prepareCalculator(_elem, true)) return;

    auto tripType = _schema->tripType();
    switch (tripType)
    {
    case Schema::SW: break; // TODO
    case Schema::RR: break; // TODO
    case Schema::SP: if (!prepareSP()) return; break;
    }

    int index = 0;
    double arg = 0;
    while (index < points)
    {
        Z_INFO("Point:" << index << "| Argument:" << arg)

        _elem->setSubRange(arg);
        _calc->multMatrix();

        Z::ValueTS value;
        switch (tripType)
        {
        case Schema::SW: break; // TODO
        case Schema::RR: break; // TODO
        case Schema::SP: value = calculateSP(); break;
        }

        _x_t[index] = arg; _y_t[index] = value.T;
        _x_s[index] = arg; _y_s[index] = value.S;

        index++;
        arg += step;
        if (arg > _arg.stop)
            arg = _arg.stop;
    }

    _range.set(0, _arg.stop);
    Z_INFO("Range:" << _range.str())*/
}

bool CausticFunction::prepareSP()
{
// TODO
//    const Z::Pump::Params& pump = _schema->pump();

//    auto error = pump.verify();
//    if (!error.isEmpty())
//    {
//        setError(qApp->translate("Calc error", "Invalid input beam parameters: ") + error);
//        clearResults();
//        return false;
//    }

//    switch (pump.mode)
//    {
//    case Z::Pump::PumpMode_vector: prepareSP_vector(); break;
//    case Z::Pump::PumpMode_sections: prepareSP_sections(); break;
//    }
    return true;
}

void CausticFunction::prepareSP_vector()
{
// TODO
//    const Z::Pump::Params& pump = _schema->pump();
//    const Z::Units::Set& units = _schema->units();
//    double r_t = units.beamsize2linear(pump.vector.radius().T);
//    double r_s = units.beamsize2linear(pump.vector.radius().S);
//    double v_t = units.angle2si(pump.vector.angle().T);
//    double v_s = units.angle2si(pump.vector.angle().S);
//    _ray_in_t.set(r_t + pump.vector.distance().T*tan(v_t), v_t);
//    _ray_in_s.set(r_s + pump.vector.distance().T*tan(v_s), v_s);
//    Z_INFO("Input rays: T =" << _ray_in_t.str() << "S =" << _ray_in_s.str())
//    _pumpMode = Pump_Ray;
}

void CausticFunction::prepareSP_sections()
{
// TODO
//    const Z::Pump::Params& pump = _schema->pump();
//    const Z::Units::Set& units = _schema->units();
//    double r1_t = units.beamsize2linear(pump.sections.radius_1().T);
//    double r2_t = units.beamsize2linear(pump.sections.radius_2().T);
//    double r1_s = units.beamsize2linear(pump.sections.radius_1().S);
//    double r2_s = units.beamsize2linear(pump.sections.radius_2().S);
//    _ray_in_t.set(r2_t, atan((r2_t - r1_t)/pump.sections.distance().T));
//    _ray_in_s.set(r2_s, atan((r2_s - r1_s)/pump.sections.distance().S));
//    Z_INFO("Input rays: T =" << _ray_in_t.str() << "S =" << _ray_in_s.str())
//    _pumpMode = Pump_Ray;
}

Z::ValueTS CausticFunction::calculateSP()
{
    //const Z::Units::Set& units = _schema->units();
    Z::ValueTS result;
    /* TODO switch (_pumpMode)
    {
    case Pump_Ray:
        Z::RayVector ray_t(_ray_in_t, _calc->Mt());
        Z::RayVector ray_s(_ray_in_s, _calc->Ms());
        Z_INFO("T =" << ray_t.str() << "S =" << ray_s.str());
        switch (_mode)
        {
        case Mode::Beamsize:
            result.set(units.linear2beamsize(ray_t.Y),
                       units.linear2beamsize(ray_s.Y));
            break;

        case Mode::Curvature: // there is no sense
            result.set(0, 0);
            break;

        case Mode::Angle:
            result.set(units.angle2si(ray_t.V),
                       units.angle2si(ray_s.V));
            break;
        }
        break;
    }*/
    return result;
}

QString CausticFunction::calculatePoint(const double& arg)
{
    if (!_elem || !_calc || !_range.has(arg)) return QString();

    _elem->setSubRange(arg);
    _calc->multMatrix();

    Z::ValueTS value;
    switch (_schema->tripType())
    {
    case Schema::SW: break; // TODO
    case Schema::RR: break; // TODO
    case Schema::SP: value = calculateSP(); break;
    }

    return QString::fromUtf8("ω<sub>T</sub>: %1; ω<sub>S</sub>: %2")
            .arg(Z::format(value.T), Z::format(value.S));
}

QString CausticFunction::calculateNotables()
{
    return QString();
}

