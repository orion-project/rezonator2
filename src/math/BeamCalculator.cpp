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
    _ref = ref;
    _rt = new RoundTripCalculator(_schema, ref);
    _rt->calcRoundTrip(splitRange);
    _rt->multMatrix(reason);
}

void BeamCalculator::multMatrix(const char *reason)
{
    if (!_rt) {
        qWarning() << "BeamCalculator: round-trip is not calculated";
        return;
    }
    _rt->multMatrix(reason);
}

BeamResult BeamCalculator::calc()
{
    if (!_rt) {
        return { qQNaN(), qQNaN(), qQNaN() };
        qWarning() << "BeamCalculator: round-trip is not calculated";
    }
    if (_pump)
        return _pump->calc(_ts, _rt->M(_ts), _ior);
    if (_abcd)
        return _abcd->calc(_rt->M(_ts), _ior);
    qWarning() << "BeamCalculator::calc: neither pump or abcd calc inited";
    return { qQNaN(), qQNaN(), qQNaN() };
}
    
#define CHECK_RT \
    if (!_rt) { \
        return qQNaN(); \
        qWarning() << "BeamCalculator: round-trip is not calculated"; \
    }

double BeamCalculator::beamRadius()
{
    CHECK_RT
    if (_pump)
        return _pump->calc(_ts, _rt->M(_ts), _ior).beamRadius;
    if (_abcd)
        return _abcd->beamRadius(_rt->M(_ts), _ior);
    qWarning() << "BeamCalculator::beamRadius: neither pump or abcd calc inited";
    return qQNaN();
}

double BeamCalculator::frontRadius()
{
    CHECK_RT
    if (_pump)
        return _pump->calc(_ts, _rt->M(_ts), _ior).frontRadius;
    if (_abcd)
        return _abcd->frontRadius(_rt->M(_ts), _ior);
    qWarning() << "BeamCalculator::frontRadius: neither pump or abcd calc inited";
    return qQNaN();
}

double BeamCalculator::halfAngle()
{
    CHECK_RT
    if (_pump)
        return _pump->calc(_ts, _rt->M(_ts), _ior).halfAngle;
    if (_abcd)
        return _abcd->halfAngle(_rt->M(_ts), _ior);
    qWarning() << "BeamCalculator::halfAngle: neither pump or abcd calc inited";
    return qQNaN();
}

Z::Matrix BeamCalculator::matrix() const
{
    if (!_rt) {
        qWarning() << "BeamCalculator: round-trip is not calculated";
        return Z::Matrix();
    }
    return _rt->M(_ts);
}

double BeamCalculator::stability_normal() const
{
    CHECK_RT
    return _rt->stability(_ts, Z::Enums::StabilityCalcMode::Normal);
}

double BeamCalculator::stability_squared() const
{
    CHECK_RT
    return _rt->stability(_ts, Z::Enums::StabilityCalcMode::Squared);
}
