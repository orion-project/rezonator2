#include "BeamCalculator.h"

#include "FunctionUtils.h"
#include "../core/Schema.h"

BeamCalculator::BeamCalculator(Schema *schema) : _schema(schema)
{
    if (schema->isSP())
    {
        auto pump = schema->activePump();
        if (!pump)
        {
            _error = qApp->translate("Calc error",
                "There is no active pump in the schema. "
                "Use 'Pumps' window to create a new pump or activate one of the existing ones.");
            return;
        }
        _pump = PumpCalculator(pump, schema->wavelenSi());
       // FunctionUtils::prepareDynamicElements(schema, nullptr, &*_pump);
    }
    else
    {
        _abcd = AbcdCalculator(schema->wavelenSi());
    }
}

void BeamCalculator::calcRoundTrip(Element *ref, bool splitRange, const char *reason)
{
    _rt = RoundTripCalculator(_schema, ref);
    _rt->calcRoundTrip(splitRange);
    _rt->multMatrix(reason);
}

double BeamCalculator::beamRadius(Z::WorkPlane ts, double ior)
{
    if (!_rt)
        return qQNaN();
    if (_pump) {
        BeamResult r = _pump->calc(ts, _rt->M(ts), ior);
        return r.beamRadius;
    }
    if (_abcd) {
        Z::PointTS r = _abcd->beamRadius(_rt->Mt(), _rt->Ms(), ior);
        return r[ts];
    }
    return qQNaN();
}

double BeamCalculator::frontRadius(Z::WorkPlane ts, double ior)
{
    if (!_rt)
        return qQNaN();
    if (_pump) {
        BeamResult r = _pump->calc(ts, _rt->M(ts), ior);
        return r.frontRadius;
    }
    if (_abcd) {
        Z::PointTS r = _abcd->frontRadius(_rt->Mt(), _rt->Ms(), ior);
        return r[ts];
    }
    return qQNaN();
}

double BeamCalculator::halfAngle(Z::WorkPlane ts, double ior)
{
    if (!_rt)
        return qQNaN();
    if (_pump) {
        BeamResult r = _pump->calc(ts, _rt->M(ts), ior);
        return r.halfAngle;
    }
    if (_abcd) {
        Z::PointTS r = _abcd->halfAngle(_rt->Mt(), _rt->Ms(), ior);
        return r[ts];
    }
    return qQNaN();
}
