#include "BeamParamsAtElemsFunction.h"

#include "AbcdBeamCalculator.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"

#include <QApplication>

BeamParamsAtElemsFunction::BeamParamsAtElemsFunction(Schema *schema) : TableFunction(schema)
{
    _columns = {
        ColumnDef {
            .label = "W",
            .title = qApp->tr("Beam radius", "Table function column"),
            .dim = Z::Dims::linear(),
            .hint = ColumnDef::hintBeamsize,
        },
        // ColumnDef {
        //     .label = "A",
        //     .title = qApp->tr("Aperture ratio", "Table function column"),
        // },
        ColumnDef {
            .label = "R",
            .title = qApp->tr("Wavefront ROC", "Table function column"),
            .dim = Z::Dims::linear(),
            .hint = ColumnDef::hintWavefront,
        },
        ColumnDef {
            .label = "V",
            .title = qApp->tr("Half div. angle", "Table function column"),
            .dim = Z::Dims::angular(),
        }
    };
}

QVector<Z::PointTS> BeamParamsAtElemsFunction::calculatePumpBeforeSchema(Element *elem)
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

QVector<Z::PointTS> BeamParamsAtElemsFunction::calculateSinglePass(Element *elem, RoundTripCalculator* calc, double ior) const
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

QVector<Z::PointTS> BeamParamsAtElemsFunction::calculateResonator(Element *elem, RoundTripCalculator *calc, double ior) const
{
    Q_UNUSED(elem)
    
    Z::PointTS beamRadius = _beamCalc->beamRadius(calc->Mt(), calc->Ms(), ior);
    Z::PointTS frontRadius = _beamCalc->frontRadius(calc->Mt(), calc->Ms(), ior);
    Z::PointTS halfAngle = _beamCalc->halfAngle(calc->Mt(), calc->Ms(), ior);
    //Z::PointTS aperRatio = calcApertureRatio(beamRadius, elem);
    
    return { beamRadius, /*aperRatio,*/ frontRadius, halfAngle };
}

Z::PointTS BeamParamsAtElemsFunction::calcApertureRatio(const Z::PointTS &beamRadius, Element *elem) const
{
    auto aper = elem->aperture();
    if (!aper)
        return { qQNaN(), qQNaN() };
    const double aperR = aper->toSi() / 2.0;
    return { aperR / beamRadius.T, aperR / beamRadius.S };
}
