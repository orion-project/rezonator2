#include "StabilityMap2DFunction.h"

#include "../app/PersistentState.h"
#include "../core/Schema.h"
#include "../math/RoundTripCalculator.h"

void StabilityMap2DFunction::calculate(CalculationMode calcMode)
{
    if (!checkArg(&_paramX)) return;
    if (!checkArg(&_paramY)) return;

    ElementEventsLocker elemLockX(_paramX.parameter, "StabilityMap2DFunction::calculate");
    ElementEventsLocker elemLockY(_paramY.parameter, "StabilityMap2DFunction::calculate");
    Z::ParamValueBackup paramLockX(_paramX.parameter, "StabilityMap2DFunction::calculate");
    Z::ParamValueBackup paramLockY(_paramY.parameter, "StabilityMap2DFunction::calculate");

    _rangeX = _paramX.range.plottingRange();
    _rangeY = _paramY.range.plottingRange();
    
    auto ref = _paramX.element;
    if (ref == _schema->globalParamsAsElem()) {
        // Use any non-locked element as the reference for round-trip
        auto activeElems = _schema->activeElements();
        if (activeElems.isEmpty()) {
            setError(qApp->translate("Calc error", "No active elements in the schema"));
            return;
        }
        ref = activeElems.first();
    }

    if (!prepareCalculator(ref)) return;
    _calc->setStabilityCalcMode(stabilityCalcMode());

    if (calcMode != CALC_PLOT) return;

    int nx = _rangeX.points();
    int ny = _rangeY.points();
    auto unitX = _rangeX.unit();
    auto unitY = _rangeY.unit();

    int pointsCount = nx * ny;
    if (_resultsT.size() != pointsCount)
    {
        _resultsT.resize(pointsCount);
        _resultsS.resize(pointsCount);
    }

    auto valuesX = _rangeX.values();
    auto valuesY = _rangeY.values();

    for (int ix = 0; ix < nx; ix++)
    {
        _paramX.parameter->setValue({valuesX.at(ix), unitX});

        for (int iy = 0; iy < ny; iy++)
        {
            _paramY.parameter->setValue({valuesY.at(iy), unitY});

            _calc->multMatrix("StabilityMap2DFunction::calculate");

            auto stab = _calc->stability();
            int index = ix * ny + iy;
            _resultsT[index] = stab.T;
            _resultsS[index] = stab.S;
        }
    }
}

void StabilityMap2DFunction::loadPrefs()
{
    _stabilityCalcMode = RecentData::getEnum("func_stab_2d_map_mode", Z::Enums::StabilityCalcMode::Normal);
}

bool StabilityMap2DFunction::checkArg(Z::Variable* arg)
{
    if (!arg->element)
    {
        setError(QString("No variable element is set (StabilityMap2DFunction.%1.element == null)")
                 .arg(arg == &_paramX ? "X" : "Y"));
        return false;
    }
    if (!arg->parameter)
    {
        setError(QString("No variable parameter is set (StabilityMap2DFunction.%1.parameter == null)")
                 .arg(arg == &_paramX ? "X" : "Y"));
        return false;
    }
    return true;
}

Z::PointTS StabilityMap2DFunction::calculateAtXY(const Z::Value& x, const Z::Value& y)
{
    ElementEventsLocker elemLockX(_paramX.parameter, "StabilityMap2DFunction::calculateAtXY");
    ElementEventsLocker elemLockY(_paramY.parameter, "StabilityMap2DFunction::calculateAtXY");
    Z::ParamValueBackup paramLockX(_paramX.parameter, "StabilityMap2DFunction::calculateAtXY");
    Z::ParamValueBackup paramLockY(_paramY.parameter, "StabilityMap2DFunction::calculateAtXY");
    _paramX.parameter->setValue(x);
    _paramY.parameter->setValue(y);
    _calc->multMatrix("StabilityMap2DFunction::calculateAtXY");
    return _calc->stability();
}

PlotFuncDeps StabilityMap2DFunction::dependsOn() const
{
    return PlotFuncDeps {
        .elems = { _paramX.element, _paramY.element },
        .params = { _paramX.parameter, _paramY.parameter },
    };
}
