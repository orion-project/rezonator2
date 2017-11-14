#include <QApplication>
#include <QDebug>

#include "CausticFunction.h"
#include "../funcs/Calculator.h"
#include "../core/Protocol.h"

void CausticFunction::calculate()
{
    if (!checkArguments()) return;

    auto elem = arg()->element;
    auto param = arg()->parameter;

    auto rangeElem = Z::Utils::asRange(elem);
    if (!rangeElem)
    {
        setError("CausticFunction.arg.element is not range");
        return;
    }

    _wavelenSI = schema()->wavelength().value().toSi();

    auto tmpRange = arg()->range;
    tmpRange.stop = Z::Value(rangeElem->axisLengthSI(), Z::Units::m());
    auto range = tmpRange.plottingRange();
    if (!prepareResults(range)) return;
    if (!prepareCalculator(elem, true)) return;

    auto tripType = _schema->tripType();
    if (tripType == TripType::SP)
        if (!prepareSP()) return;

    BackupAndLock locker(elem, param);

    int index = 0;
    double x = range.start();
    auto argUnit = param->value().unit();
    while (index < range.points())
    {
        rangeElem->setSubRangeSI(x);
        _calc->multMatrix();

        Z::PointTS res;
        switch (tripType)
        {
        case TripType::SW:
        case TripType::RR: res = calculateResonator(); break;
        case TripType::SP: res = calculateSinglePass(); break;
        }

        Z_INFO(index << x << res.T << res.S);

        convertFromSiToModeUnits(res);

        _x_t[index] = argUnit->fromSi(x), _y_t[index] = res.T;
        _x_s[index] = argUnit->fromSi(x), _y_s[index] = res.S;

        index++;
        x = qMin(x + range.step(), range.stop());
    }
}

bool CausticFunction::prepareSP()
{
// TODO:NEXT-VER
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
// TODO:NEXT-VER
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
// TODO:NEXT-VER
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

Z::PointTS CausticFunction::calculateSinglePass() const
{
    //const Z::Units::Set& units = _schema->units();
    Z::PointTS result;
    /* TODO:NEXT-VER switch (_pumpMode)
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

Z::PointTS CausticFunction::calculateResonator() const
{
#define CALC_TS_POINT(func) return Z::PointTS(func(_calc->Mt()), func(_calc->Ms()))

    switch (_mode)
    {
    case BeamRadius: CALC_TS_POINT(calculateResonator_beamRadius);
    case FontRadius: CALC_TS_POINT(calculateResonator_frontRadius);
    case HalfAngle: CALC_TS_POINT(calculateResonator_halfAngle);
    }
    return Z::PointTS();

#undef CALC_TS_POINT
}

// TODO:NEXT-VER should be in Calculator as it can be reused by another functions, e.g. BeamParamsAtElems
double CausticFunction::calculateResonator_beamRadius(const Z::Matrix& m) const
{
    double p = 1 - SQR((m.A + m.D) * 0.5);
    if (p <= 0) return NAN;
    double w2 = _wavelenSI * qAbs(m.B) * M_1_PI / sqrt(p);
    if (w2 < 0) return NAN;
    return sqrt(w2);
}

// TODO:NEXT-VER should be in Calculator as it can be reused by another functions, e.g. BeamParamsAtElems
double CausticFunction::calculateResonator_frontRadius(const Z::Matrix& m) const
{
    if (m.D != m.A)
        return 2 * m.B / (m.D - m.A);
    return (m.B < 0) ? -INFINITY : +INFINITY;
}

// TODO:NEXT-VER should be in Calculator as it can be reused by another functions, e.g. BeamParamsAtElems
double CausticFunction::calculateResonator_halfAngle(const Z::Matrix& m) const
{
    double p = 4.0 - SQR(m.A + m.D);
    if (p <= 0) return NAN;
    return sqrt(_wavelenSI * M_1_PI * 2.0 * qAbs(m.C) / sqrt(p));
}

QString CausticFunction::calculatePoint(const double& arg)
{
    Q_UNUSED(arg)
    return QString();

/* TODO:NEXT-VER
    if (!_elem || !_calc || !_range.has(arg)) return QString();

    _elem->setSubRange(arg);
    _calc->multMatrix();

    Z::PointTS value;
    switch (_schema->tripType())
    {
    case TripType::SW: break; // TODO
    case TripType::RR: break; // TODO
    case TripType::SP: value = calculateSinglePass(); break;
    }

    return QString::fromUtf8("ω<sub>T</sub>: %1; ω<sub>S</sub>: %2")
            .arg(Z::format(value.T), Z::format(value.S)); */
}

void CausticFunction::convertFromSiToModeUnits(Z::PointTS& point) const
{
    switch (_mode)
    {
    case BeamRadius:
        point.T = _beamsizeUnit->fromSi(point.T);
        point.S = _beamsizeUnit->fromSi(point.S);
        break;

    case FontRadius:
        point.T = _curvatureUnit->fromSi(point.T);
        point.S = _curvatureUnit->fromSi(point.S);
        break;

    case HalfAngle:
        point.T = _angleUnit->fromSi(point.T);
        point.S = _angleUnit->fromSi(point.S);
        break;
    }
}

