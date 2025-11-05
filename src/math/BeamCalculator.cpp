#include "BeamCalculator.h"

#include "FunctionUtils.h"
#include "../core/Schema.h"

#define CHECK_RT(ret) \
    if (!_rt) { \
        qWarning() << "BeamCalculator: round-trip is not calculated"; \
        return ret; \
    }
    
#define BAD_CALCS(ret) \
    qWarning() << "BeamCalculator::beamRadius: neither pump or abcd calc inited"; \
    return ret;
    
#define BAD_TS(ret) \
    qWarning() << "BeamCalculator::matrix: invalid work plane" << _ts; \
    return ret;

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
    CHECK_RT(BeamResult::nan())
    if (_pump)
        return _pump->calc(_ts, _rt->M(_ts), _ior);
    if (_abcd)
        return _abcd->calc(_rt->M(_ts), _ior);
    BAD_CALCS(BeamResult::nan())
}
    
double BeamCalculator::beamRadius()
{
    CHECK_RT(qQNaN())
    if (_pump)
        return _pump->calc(_ts, _rt->M(_ts), _ior).beamRadius;
    if (_abcd)
        return _abcd->beamRadius(_rt->M(_ts), _ior);
    BAD_CALCS(qQNaN())
}

double BeamCalculator::frontRadius()
{
    CHECK_RT(qQNaN())
    if (_pump)
        return _pump->calc(_ts, _rt->M(_ts), _ior).frontRadius;
    if (_abcd)
        return _abcd->frontRadius(_rt->M(_ts), _ior);
    BAD_CALCS(qQNaN())
}

double BeamCalculator::halfAngle()
{
    CHECK_RT(qQNaN())
    if (_pump)
        return _pump->calc(_ts, _rt->M(_ts), _ior).halfAngle;
    if (_abcd)
        return _abcd->halfAngle(_rt->M(_ts), _ior);
    BAD_CALCS(qQNaN())
}

Z::Matrix BeamCalculator::matrix() const
{
    CHECK_RT({})
    return _rt->M(_ts);
}

Element* BeamCalculator::elem(int index) const
{
    CHECK_RT(nullptr)
    const auto &info = _rt->matrixInfo();
    if (index >= 0 && index < info.size())
        return info.at(index).owner;
    return nullptr;
}

std::optional<Z::Matrix> BeamCalculator::matrix(int index) const
{
    CHECK_RT({})
    if (_ts == Z::T) {
        if (index >= 0 && index < _rt->matrsT().size())
            return _rt->matrsT().at(index);
        return {};
    }
    if (_ts == Z::S) {
        if (index >= 0 && index < _rt->matrsS().size())
            return _rt->matrsS().at(index);
        return {};
    }
    BAD_TS({})
}

int BeamCalculator::matrixCount() const
{
    CHECK_RT(0)
    if (_ts == Z::T)
        return _rt->matrsT().size();
    if (_ts == Z::S)
        return _rt->matrsS().size();
    BAD_TS(0)
}

double BeamCalculator::stability_normal() const
{
    CHECK_RT(qQNaN())
    return _rt->stability(_ts, Z::Enums::StabilityCalcMode::Normal);
}

double BeamCalculator::stability_squared() const
{
    CHECK_RT(qQNaN())
    return _rt->stability(_ts, Z::Enums::StabilityCalcMode::Squared);
}
