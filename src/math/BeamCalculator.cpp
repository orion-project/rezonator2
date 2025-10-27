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
        _pump = new PumpCalculator(pump, schema->wavelenSi());
        FunctionUtils::prepareDynamicElements(schema, nullptr, _pump);
    }
    else
    {
        _abcd = new AbcdCalculator(schema->wavelenSi());
    }
}

BeamCalculator::~BeamCalculator()
{
    if (_abcd) delete _abcd;
    if (_pump) delete _pump;
    if (_rt) delete _rt;
}

void BeamCalculator::calcRoundTrip(Element *ref, bool splitRange, const char *reason)
{
    if (_rt) delete _rt;
    _rt = new RoundTripCalculator(_schema, ref);
    _rt->calcRoundTrip(splitRange);
    _rt->multMatrix(reason);
}

double BeamCalculator::beamRadius()
{
    if (!_rt)
        return qQNaN();
    if (_pump) {
        BeamResult r = _pump->calc(_ts, _rt->M(_ts), _ior);
        return r.beamRadius;
    }
    if (_abcd) {
        Z::PointTS r = _abcd->beamRadius(_rt->Mt(), _rt->Ms(), _ior);
        return r[_ts];
    }
    return qQNaN();
}

double BeamCalculator::frontRadius()
{
    if (!_rt)
        return qQNaN();
    if (_pump) {
        BeamResult r = _pump->calc(_ts, _rt->M(_ts), _ior);
        return r.frontRadius;
    }
    if (_abcd) {
        Z::PointTS r = _abcd->frontRadius(_rt->Mt(), _rt->Ms(), _ior);
        return r[_ts];
    }
    return qQNaN();
}

double BeamCalculator::halfAngle()
{
    if (!_rt)
        return qQNaN();
    if (_pump) {
        BeamResult r = _pump->calc(_ts, _rt->M(_ts), _ior);
        return r.halfAngle;
    }
    if (_abcd) {
        Z::PointTS r = _abcd->halfAngle(_rt->Mt(), _rt->Ms(), _ior);
        return r[_ts];
    }
    return qQNaN();
}
