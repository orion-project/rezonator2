#include "BeamVariationFunction.h"

#include "../core/Perf.h"
#include "../core/Schema.h"
#include "../math/AbcdCalculator.h"
#include "../math/FunctionUtils.h"
#include "../math/PumpCalculator.h"
#include "../math/RoundTripCalculator.h"

void BeamVariationFunction::calculate(CalculationMode calcMode)
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

    if (calcMode != CALC_PLOT) return;

    auto rangeElem = _pos.element->asRange();

    // If we plot in the same range element which we change
    // then subrange matrices could change every step
    // https://github.com/orion-project/rezonator2/issues/54
    bool recalcSubrange = _pos.element == arg()->element;
    double subrangeSi = 0;

    if (rangeElem)
    {
        _ior = rangeElem->ior();
        subrangeSi = _pos.offset.toSi();
        if (_pos.offset.unit() == UNIT(percent))
            subrangeSi *= rangeElem->axisLengthSI() / 100.0;
        if (subrangeSi < 0)
            subrangeSi += rangeElem->axisLengthSI();
        rangeElem->setSubRangeSI(subrangeSi);
    }

    bool isResonator = _schema->isResonator();
    bool prepared = isResonator
            ? prepareResonator()
            : prepareSinglePass();
    if (!prepared) return;

    auto param = arg()->parameter;
    auto unitX = range.unit();

    ElementEventsLocker elemLock(param, "BeamVariationFunction::calculate");
    Z::ParamValueBackup paramLock(param, "BeamVariationFunction::calculate");

    Z_PERF_RESET
    Z_PERF_BEGIN("BeamVariationFunction")

    for (auto x : range.values())
    {
        Z_PERF_BEGIN("setValue")
        param->setValue({x, unitX});
        Z_PERF_END

        Z_PERF_BEGIN("recalcSubrange")
        if (rangeElem && recalcSubrange)
            rangeElem->setSubRangeSI(subrangeSi);
        Z_PERF_END

        if (!isResonator) {
            // If the variating element located further than a dynamic element
            // it does not affect the dynamic element matrices
            // But we calculate dynamic matrices anyway, for simplicity
            FunctionUtils::prepareDynamicElements(_schema, nullptr, _pumpCalc.get());
        }

        Z_PERF_BEGIN("multMatrix")
        _calc->multMatrix("BeamVariationFunction::calculate");
        Z_PERF_END

        Z_PERF_BEGIN("addResultPoint")
        addResultPoint(x, isResonator ? calculateResonator() : calculateSinglePass());
        Z_PERF_END
    }

    Z_PERF_END
    Z_PERF_PRINT

    finishResults();
}

Z::PointTS BeamVariationFunction::calculateAt(const Z::Value& v)
{
    auto param = arg()->parameter;
    ElementEventsLocker elemLock(param, "BeamVariationFunction::calculateAt");
    Z::ParamValueBackup paramLock(param, "BeamVariationFunction::calculateAt");
    auto rangeElem = _pos.element->asRange();
    if (rangeElem) {
        auto offset = _pos.offset.toSi();
        if (_pos.offset.unit() == UNIT(percent))
            offset *= rangeElem->axisLengthSI() / 100.0;
        if (offset < 0)
            offset += rangeElem->axisLengthSI();
        rangeElem->setSubRangeSI(offset);
    }
    param->setValue(v);
    bool isResonator = _schema->isResonator();
    if (!isResonator)
    {
        // Is the variating element located further than a dynamic element
        // it does not affect the dynamic element matrices
        // But we calculate dynamic matrices anyway, for simplicity
        FunctionUtils::prepareDynamicElements(_schema, nullptr, _pumpCalc.get());
    }
    _calc->multMatrix("BeamVariationFunction::calculateAt");
    return isResonator ? calculateResonator() : calculateSinglePass();
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
    _beamCalc.reset(new AbcdCalculator(schema()->wavelenSi()));
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

PlotFuncDeps BeamVariationFunction::dependsOn() const
{
    auto deps = PlotFunction::dependsOn();
    deps.elems << _pos.element;
    return deps;
}