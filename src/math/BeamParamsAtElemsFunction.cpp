#include "BeamParamsAtElemsFunction.h"

#include "BeamCalculator.h"

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
    BeamResult beamT = _beamCalc->pumpCalc().calcT(unity, 1);
    BeamResult beamS = _beamCalc->pumpCalc().calcS(unity, 1);
    return {
        Z::PointTS(beamT.beamRadius, beamS.beamRadius),
        Z::PointTS(beamT.frontRadius, beamS.frontRadius),
        Z::PointTS(beamT.halfAngle, beamS.halfAngle),
    };
}

QVector<Z::PointTS> BeamParamsAtElemsFunction::calculateInternal(Element *elem, double ior)
{
    return {
        Z::PointTS(_beamCalc->beamRadius(Z::T, ior), _beamCalc->beamRadius(Z::S, ior)),
        Z::PointTS(_beamCalc->frontRadius(Z::T, ior), _beamCalc->frontRadius(Z::S, ior)),
        Z::PointTS(_beamCalc->halfAngle(Z::T, ior), _beamCalc->halfAngle(Z::S, ior)),
    };
}
