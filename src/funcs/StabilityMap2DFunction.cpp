#include "StabilityMap2DFunction.h"

#include "RoundTripCalculator.h"

#include "../CustomPrefs.h"

#include "libs/qcustomplot/qcustomplot.h"

void StabilityMap2DFunction::calculate()
{
    if (!checkArg(&_paramX)) return;
    if (!checkArg(&_paramY)) return;

    BackupAndLock lockerX(_paramX.element, _paramX.parameter);
    BackupAndLock lockerY(_paramY.element, _paramY.parameter);

    _rangeX = _paramX.range.plottingRange();
    _rangeY = _paramY.range.plottingRange();

    if (!prepareCalculator(_paramX.element)) return;
    _calc->setStabilityCalcMode(stabilityCalcMode());

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
        _paramX.element->calcMatrix("StabilityMap2DFunction::calculate");

        for (int iy = 0; iy < ny; iy++)
        {
            _paramY.parameter->setValue({valuesY.at(iy), unitY});
            _paramY.element->calcMatrix("StabilityMap2DFunction::calculate");

            _calc->multMatrix();

            auto stab = _calc->stability();
            int index = ix * ny + iy;
            _resultsT[index] = stab.T;
            _resultsS[index] = stab.S;
        }
    }
}

void StabilityMap2DFunction::loadPrefs()
{
    _stabilityCalcMode = Z::Enums::fromStr(
                CustomPrefs::recentStr(QStringLiteral("func_stab_2d_map_mode")),
                Z::Enums::StabilityCalcMode::Normal);
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
