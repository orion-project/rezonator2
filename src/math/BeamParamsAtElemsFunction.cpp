#include "BeamParamsAtElemsFunction.h"

#include "AbcdBeamCalculator.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../app/AppSettings.h"

#include <QApplication>

#define COL_BEAMSIZE QStringLiteral("beamsize")
#define COL_WAVEFRONT QStringLiteral("wavefront")
#define COL_ANGLE QStringLiteral("angle")
#define COL_COUNT 3

BeamParamsAtElemsFunction::BeamParamsAtElemsFunction(Schema *schema) : TableFunction(schema)
{
}

QVector<TableFunction::ColumnDef> BeamParamsAtElemsFunction::columns() const
{
    ColumnDef beamRadius;
    beamRadius.id = COL_BEAMSIZE;
    beamRadius.titleT = QStringLiteral("Wt");
    beamRadius.titleS = QStringLiteral("Ws");
    beamRadius.unit = _colUnits.value(COL_BEAMSIZE, AppSettings::instance().defaultUnitBeamRadius);

    ColumnDef frontRadius;
    frontRadius.id = COL_WAVEFRONT;
    frontRadius.titleT = QStringLiteral("Rt");
    frontRadius.titleS = QStringLiteral("Rs");
    frontRadius.unit = _colUnits.value(COL_WAVEFRONT, AppSettings::instance().defaultUnitFrontRadius);

    ColumnDef halfAngle;
    halfAngle.id = COL_ANGLE;
    halfAngle.titleT = QStringLiteral("Vt");
    halfAngle.titleS = QStringLiteral("Vs");
    halfAngle.unit = _colUnits.value(COL_ANGLE, AppSettings::instance().defaultUnitAngle);

    return {beamRadius, frontRadius, halfAngle};
}

QString BeamParamsAtElemsFunction::columnTitle(const ColumnId &id) const
{
    if (id == COL_BEAMSIZE)
        return qApp->tr("Beam radius", "Table function column");
    if (id == COL_WAVEFRONT)
        return qApp->tr("Wavefront ROC", "Table function column");
    if (id == COL_ANGLE)
        return qApp->tr("Half div. angle", "Table function column");
    return id;
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
