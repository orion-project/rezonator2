#include "CustomTableFunction.h"

#include "AbcdBeamCalculator.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../app/AppSettings.h"

#include <QApplication>

#define COL_BEAMSIZE QStringLiteral("beamsize")
#define COL_APER_RATIO QStringLiteral("aper")
#define COL_WAVEFRONT QStringLiteral("wavefront")
#define COL_ANGLE QStringLiteral("angle")
#define COL_COUNT 3

CustomTableFunction::CustomTableFunction(Schema *schema) : TableFunction(schema)
{
}

QVector<TableFunction::ColumnDef> CustomTableFunction::columns() const
{
    ColumnDef beamRadius;
    beamRadius.id = COL_BEAMSIZE;
    beamRadius.titleT = QStringLiteral("Wt");
    beamRadius.titleS = QStringLiteral("Ws");
    beamRadius.unit = _colUnits.value(COL_BEAMSIZE, AppSettings::instance().defaultUnitBeamRadius);

    // ColumnDef aperRatio;
    // aperRatio.id = COL_APER_RATIO;
    // aperRatio.titleT = QStringLiteral("At");
    // aperRatio.titleS = QStringLiteral("As");
    // aperRatio.unit = Z::Units::none();

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

    return { beamRadius, /*aperRatio,*/ frontRadius, halfAngle };
}

QString CustomTableFunction::columnTitle(const ColumnId &id) const
{
    if (id == COL_BEAMSIZE)
        return qApp->tr("Beam radius", "Table function column");
    if (id == COL_BEAMSIZE)
        return qApp->tr("Aperture ratio", "Table function column");
    if (id == COL_WAVEFRONT)
        return qApp->tr("Wavefront ROC", "Table function column");
    if (id == COL_ANGLE)
        return qApp->tr("Half div. angle", "Table function column");
    return id;
}

int CustomTableFunction::columnCount() const
{
    return COL_COUNT;
}

QVector<Z::PointTS> CustomTableFunction::calculatePumpBeforeSchema(Element *elem)
{
    Q_UNUSED(elem)

    Z::Matrix unity;
    BeamResult beamT = _pumpCalc->calcT(unity, 1);
    BeamResult beamS = _pumpCalc->calcS(unity, 1);

    Z::PointTS beamRadius(beamT.beamRadius, beamS.beamRadius);
    Z::PointTS frontRadius(beamT.frontRadius, beamS.frontRadius);
    Z::PointTS halfAngle(beamT.halfAngle, beamS.halfAngle);
    //Z::PointTS aperRatio = calcApertureRatio(beamRadius, elem);

    return { beamRadius, /*aperRatio,*/ frontRadius, halfAngle };
}

QVector<Z::PointTS> CustomTableFunction::calculateSinglePass(Element *elem, RoundTripCalculator* calc, double ior) const
{
    Q_UNUSED(elem)

    BeamResult beamT = _pumpCalc->calcT(calc->Mt(), ior);
    BeamResult beamS = _pumpCalc->calcS(calc->Ms(), ior);

    Z::PointTS beamRadius(beamT.beamRadius, beamS.beamRadius);
    Z::PointTS frontRadius(beamT.frontRadius, beamS.frontRadius);
    Z::PointTS halfAngle(beamT.halfAngle, beamS.halfAngle);
    //Z::PointTS aperRatio = calcApertureRatio(beamRadius, elem);

    return { beamRadius, /*aperRatio,*/ frontRadius, halfAngle };
}

QVector<Z::PointTS> CustomTableFunction::calculateResonator(Element *elem, RoundTripCalculator *calc, double ior) const
{
    Q_UNUSED(elem)
    
    Z::PointTS beamRadius = _beamCalc->beamRadius(calc->Mt(), calc->Ms(), ior);
    Z::PointTS frontRadius = _beamCalc->frontRadius(calc->Mt(), calc->Ms(), ior);
    Z::PointTS halfAngle = _beamCalc->halfAngle(calc->Mt(), calc->Ms(), ior);
    //Z::PointTS aperRatio = calcApertureRatio(beamRadius, elem);
    
    return { beamRadius, /*aperRatio,*/ frontRadius, halfAngle };
}

