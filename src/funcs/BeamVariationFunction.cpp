#include "BeamVariationFunction.h"

#include "../core/Schema.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "AbcdBeamCalculator.h"

void BeamVariationFunction::calculate()
{
    if (!checkArguments()) return;

    if (!_pos.element)
    {
        setError("BeamVariationFunction.pos.element is not set");
        return;
    }

    _ior = 1;

    auto range = arg()->range.plottingRange();
    if (!prepareResults(range)) return;
    if (!prepareCalculator(_pos.element, true)) return;

    auto rangeElem = Z::Utils::asRange(_pos.element);
    if (rangeElem)
    {
        rangeElem->setSubRangeSI(_pos.offset.toSi());
        _ior = rangeElem->ior();
    }

    auto tripType = _schema->tripType();
    bool prepared = tripType == TripType::SP
            ? prepareSinglePass()
            : prepareResonator();
    if (!prepared) return;

    auto param = arg()->parameter;
    auto elem = arg()->element;

    ElementEventsLocker elemLock(elem);
    Z::ParamValueBackup paramLock(param);

    for (auto x : range.values())
    {
        auto value = Z::Value(x, range.unit());

        param->setValue(value);
        _calc->multMatrix();

        Z::PointTS res;
        switch (tripType)
        {
        case TripType::SW:
        case TripType::RR: res = calculateResonator(); break;
        case TripType::SP: res = calculateSinglePass(); break;
        }

        addResultPoint(x, res);
    }

    finishResults();
}

Z::PointTS BeamVariationFunction::calculateAt(const Z::Value& v)
{
    auto elem = arg()->element;
    auto param = arg()->parameter;
    ElementEventsLocker elemLock(elem);
    Z::ParamValueBackup paramLock(param);
    auto rangeElem = Z::Utils::asRange(_pos.element);
    if (rangeElem)
        rangeElem->setSubRangeSI(_pos.offset.toSi());
    param->setValue(v);
    _calc->multMatrix();
    switch (_schema->tripType())
    {
    case TripType::SW:
    case TripType::RR: return calculateResonator();
    case TripType::SP: return calculateSinglePass();
    }
    return { Double::nan(), Double::nan() };
}

bool BeamVariationFunction::prepareSinglePass()
{
    auto pump = _schema->activePump();
    if (!pump)
    {
        setError(qApp->translate("Calc error",
            "There is no active pump in the schema. "
            "Use 'Pumps' window to create a new pump or activate one of the existing ones."));
        return false;
    }
    if (!_pumpCalc.T) _pumpCalc.T = PumpCalculator::T();
    if (!_pumpCalc.S) _pumpCalc.S = PumpCalculator::S();
    if (!_pumpCalc.T->init(pump, schema()->wavelenSi()) ||
        !_pumpCalc.S->init(pump, schema()->wavelenSi()))
    {
        setError("Unsupported pump mode");
        return false;
    }
    return true;
}

bool BeamVariationFunction::prepareResonator()
{
    _beamCalc.reset(new AbcdBeamCalculator(schema()->wavelength().value().toSi()));
    return true;
}

Z::PointTS BeamVariationFunction::calculateSinglePass() const
{
    BeamResult beamT = _pumpCalc.T->calc(_calc->Mt(), _ior);
    BeamResult beamS = _pumpCalc.S->calc(_calc->Ms(), _ior);
    return { beamT.beamRadius, beamS.beamRadius };
}

Z::PointTS BeamVariationFunction::calculateResonator() const
{
    return _beamCalc->beamRadius(_calc->Mt(), _calc->Ms(), _ior);
}
