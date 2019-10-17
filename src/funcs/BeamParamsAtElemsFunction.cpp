#include "BeamParamsAtElemsFunction.h"

#include "../AppSettings.h"
#include "../core/Schema.h"

BeamParamsAtElemsFunction::BeamParamsAtElemsFunction(Schema *schema) : TableFunction(schema)
{

}

QVector<TableFunction::ColumnDef> BeamParamsAtElemsFunction::columns() const
{
    ColumnDef beamRadius;
    beamRadius.titleT = "Wt";
    beamRadius.titleS = "Ws";
    beamRadius.unit = AppSettings::instance().defaultUnitBeamRadius;

    ColumnDef frontRadius;
    frontRadius.titleT = "Rt";
    frontRadius.titleS = "Rs";
    frontRadius.unit = AppSettings::instance().defaultUnitFrontRadius;

    ColumnDef halfAngle;
    halfAngle.titleT = "Vt";
    halfAngle.titleS = "Vs";
    halfAngle.unit = AppSettings::instance().defaultUnitAngle;

    return {beamRadius, frontRadius, halfAngle};
}

void BeamParamsAtElemsFunction::calculate()
{
    _results.clear();

    for (auto elem : schema()->elements())
    {
        Result res;
        res.element = elem;
        res.values = {
            {1, 2}, {3, 4}, {5, 6}
        };
        _results << res;
    }
}
