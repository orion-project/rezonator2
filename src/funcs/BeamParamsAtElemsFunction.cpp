#include "BeamParamsAtElemsFunction.h"

#include "../AppSettings.h"

BeamParamsAtElemsFunction::BeamParamsAtElemsFunction(Schema *schema) : TableFunction(schema)
{

}

QVector<TableFunction::ColumnDef> BeamParamsAtElemsFunction::columns() const
{
    ColumnDef position;
    position.width = ColumnDef::WIDTH_STRETCH;
    position.isHtml = true;
    position.title = "Position";

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

    return {position, beamRadius, frontRadius, halfAngle};
}
