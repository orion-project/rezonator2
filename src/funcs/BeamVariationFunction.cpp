#include "BeamVariationFunction.h"

#include "AbcdBeamCalculator.h"
#include "FunctionUtils.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../core/Schema.h"

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

    auto isResonator = _schema->isResonator();
    bool prepared = isResonator
            ? prepareResonator()
            : prepareSinglePass();
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
        if (isResonator)
            res = calculateResonator();
        else
        {
            // Is the variating element located further than a dynamic element
            // it does not affect the dynamic element matrices
            // But we calculate dynamic matrices anyway, for simplicity
            FunctionUtils::prepareDynamicElements(_schema, nullptr, _pumpCalc.get());
            res = calculateSinglePass();
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
    if (_schema->isResonator())
        return calculateResonator();
    return calculateSinglePass();
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
    // Do not prepare dynamic elements here, the will be prepared at each plot step
    _pumpCalc.reset(new PumpCalculator(pump, schema()->wavelenSi()));
    return true;
}

bool BeamVariationFunction::prepareResonator()
{
    _beamCalc.reset(new AbcdBeamCalculator(schema()->wavelenSi()));
    return true;
}

Z::PointTS BeamVariationFunction::calculateSinglePass() const
{
    return _pumpCalc->beamRadius(_calc->Mt(), _calc->Ms(), _ior);
}

Z::PointTS BeamVariationFunction::calculateResonator() const
{
    return _beamCalc->beamRadius(_calc->Mt(), _calc->Ms(), _ior);
}
