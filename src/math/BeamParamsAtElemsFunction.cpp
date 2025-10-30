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

QVector<Z::PointTS> BeamParamsAtElemsFunction::calculatePumpBeforeSchema()
{
    Z::Matrix unity;
    BeamResult beamT = _beamCalc->pumpCalc()->calcT(unity, 1);
    BeamResult beamS = _beamCalc->pumpCalc()->calcS(unity, 1);
    return {
        Z::PointTS(beamT.beamRadius, beamS.beamRadius),
        Z::PointTS(beamT.frontRadius, beamS.frontRadius),
        Z::PointTS(beamT.halfAngle, beamS.halfAngle),
    };
}

QVector<Z::PointTS> BeamParamsAtElemsFunction::calculateInternal(const ResultElem &resultElem)
{
    Q_UNUSED(resultElem);

    _beamCalc->setPlane(Z::T);
    BeamResult beamT = _beamCalc->calc();

    _beamCalc->setPlane(Z::S);
    BeamResult beamS = _beamCalc->calc();

    return {
        Z::PointTS(beamT.beamRadius, beamS.beamRadius),
        Z::PointTS(beamT.frontRadius, beamS.frontRadius),
        Z::PointTS(beamT.halfAngle, beamS.halfAngle),
    };
}
