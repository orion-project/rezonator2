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

    _wavelenSI = schema()->wavelength().value().toSi();

    auto range = arg()->range.plottingRange();
    if (!prepareResults(range)) return;
    if (!prepareCalculator(_pos.element, true)) return;

    auto rangeElem = Z::Utils::asRange(_pos.element);
    if (rangeElem)
    {
        rangeElem->setSubRangeSI(_pos.offset.toSi());
        _wavelenSI /= rangeElem->ior();
    }

    auto tripType = _schema->tripType();
    bool prepared = tripType == TripType::SP
            ? prepareSinglePass()
            : prepareResonator();
    if (!prepared) return;

    auto param = arg()->parameter;
    auto elem = arg()->element;
    BackupAndLock locker(elem, param);

    for (auto x : range.values())
    {
        auto value = Z::Value(x, range.unit());

        param->setValue(value);
        elem->calcMatrix("BeamVariationFunction::calculate");
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
    // Cached _wavelenSI can be reduced in order to account medium IOR,
    // but pump is supposed to be in air, so get wavelength from schema.
    const double lambda = schema()->wavelength().value().toSi();
    if (!_pumpCalc.T->init(pump, lambda) ||
        !_pumpCalc.S->init(pump, lambda))
    {
        setError("Unsupported pump mode");
        return false;
    }
    return true;
}

bool BeamVariationFunction::prepareResonator()
{
    if (!_beamCalc) _beamCalc.reset(new AbcdBeamCalculator);
    _beamCalc->setWavelenSI(_wavelenSI);
    return true;
}

Z::PointTS BeamVariationFunction::calculateSinglePass() const
{
    BeamResult beamT = _pumpCalc.T->calc(_calc->Mt(), _wavelenSI);
    BeamResult beamS = _pumpCalc.S->calc(_calc->Ms(), _wavelenSI);
    return { beamT.beamRadius, beamS.beamRadius };
}

Z::PointTS BeamVariationFunction::calculateResonator() const
{
    return _beamCalc->beamRadius(_calc->Mt(), _calc->Ms());
}
