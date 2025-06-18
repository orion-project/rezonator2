#include "BeamParamsAtElemsFunction.h"

#include "AbcdBeamCalculator.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../app/AppSettings.h"

#include <QApplication>

namespace {

enum Columns { COL_BEAMSIZE, COL_WAVEFRONT, COL_ANGLE, COL_COUNT };

}

BeamParamsAtElemsFunction::BeamParamsAtElemsFunction(Schema *schema) : TableFunction(schema)
{
}

QVector<TableFunction::ColumnDef> BeamParamsAtElemsFunction::columns() const
{
    ColumnDef beamRadius;
    beamRadius.titleT = "Wt";
    beamRadius.titleS = "Ws";
    beamRadius.unit = _colUnits.value(COL_BEAMSIZE, AppSettings::instance().defaultUnitBeamRadius);

    ColumnDef frontRadius;
    frontRadius.titleT = "Rt";
    frontRadius.titleS = "Rs";
    frontRadius.unit = _colUnits.value(COL_WAVEFRONT, AppSettings::instance().defaultUnitFrontRadius);

    ColumnDef halfAngle;
    halfAngle.titleT = "Vt";
    halfAngle.titleS = "Vs";
    halfAngle.unit = _colUnits.value(COL_ANGLE, AppSettings::instance().defaultUnitAngle);

    return {beamRadius, frontRadius, halfAngle};
}

QString BeamParamsAtElemsFunction::columnTitle(int colIndex) const
{
    switch (colIndex) {
    case COL_BEAMSIZE:
        return qApp->tr("Beam radius", "Table function column");
    case COL_WAVEFRONT:
        return qApp->tr("Wavefront ROC", "Table function column");
    case COL_ANGLE:
        return qApp->tr("Half div. angle", "Table function column");
    }
    return QString("#%1").arg(colIndex);
}

int BeamParamsAtElemsFunction::columnCount() const
{
    return COL_COUNT;
}

QVector<Z::PointTS> BeamParamsAtElemsFunction::calculatePumpBeforeSchema()
{
    Z::Matrix unity;
    BeamResult beamT = _pumpCalc->calcT(unity, 1);
    BeamResult beamS = _pumpCalc->calcS(unity, 1);
    return {
        { beamT.beamRadius, beamS.beamRadius },
        { beamT.frontRadius, beamS.frontRadius },
        { beamT.halfAngle, beamS.halfAngle },
    };
}

QVector<Z::PointTS> BeamParamsAtElemsFunction::calculateSinglePass(RoundTripCalculator* calc, double ior) const
{
    BeamResult beamT = _pumpCalc->calcT(calc->Mt(), ior);
    BeamResult beamS = _pumpCalc->calcS(calc->Ms(), ior);
    return {
        { beamT.beamRadius, beamS.beamRadius },
        { beamT.frontRadius, beamS.frontRadius },
        { beamT.halfAngle, beamS.halfAngle },
    };
}

QVector<Z::PointTS> BeamParamsAtElemsFunction::calculateResonator(RoundTripCalculator *calc, double ior) const
{
    return {
        _beamCalc->beamRadius(calc->Mt(), calc->Ms(), ior),
        _beamCalc->frontRadius(calc->Mt(), calc->Ms(), ior),
        _beamCalc->halfAngle(calc->Mt(), calc->Ms(), ior),
    };
}
