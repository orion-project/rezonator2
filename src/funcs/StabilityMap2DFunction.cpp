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

    auto rangeX = _paramX.range.plottingRange();
    auto rangeY = _paramY.range.plottingRange();

    if (!prepareCalculator(_paramX.element)) return;
    _calc->setStabilityCalcMode(stabilityCalcMode());

    int nx = rangeX.points();
    int ny = rangeY.points();
    auto unitX = rangeX.unit();
    auto unitY = rangeY.unit();

    auto data = graphT->data();

    data->setSize(nx, ny);
    data->setRange({rangeX.start(), rangeX.stop()}, {rangeY.start(), rangeY.stop()});

    double valueX, valueY;
    for (int ix = 0; ix < nx; ix++)
    {
        data->cellToCoord(ix, 0, &valueX, nullptr);
        _paramX.parameter->setValue({valueX, unitX});

        for (int iy = 0; iy < ny; iy++)
        {
            data->cellToCoord(ix, iy, nullptr, &valueY);
            _paramY.parameter->setValue({valueY, unitY});

            _calc->multMatrix();

            auto stab = _calc->stability();
            data->setCell(ix, iy, stab.T);
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

Z::Unit StabilityMap2DFunction::defaultUnitX() const
{
    return _paramX.range.start.unit();
}

Z::Unit StabilityMap2DFunction::defaultUnitY() const
{
    return _paramY.range.start.unit();
}
