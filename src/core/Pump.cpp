#include "Pump.h"

#include <QDebug>

namespace Z {

using namespace Units;

//--------------------------------------------------------------------------------
//                                   PumpParams
//--------------------------------------------------------------------------------

PumpParams::~PumpParams()
{
    qDeleteAll(_params);
}

void PumpParams::addParam(ParameterTS *param, double value, Unit unit)
{
    param->setValue(ValueTS(value, value, unit));
    _params.append(param);
}

//--------------------------------------------------------------------------------
//                                PumpParams_Waist
//--------------------------------------------------------------------------------

PumpParams_Waist::PumpParams_Waist()
{
    _waist = new ParameterTS(Dims::linear(),
                             QStringLiteral("w_0"),
                             QStringLiteral("ω<sub>0</sub>"),
                             qApp->translate("Pump param", "Waist radius"));
    _distance = new ParameterTS(Dims::linear(),
                                QStringLiteral("z_w"),
                                QStringLiteral("z<sub>ω</sub>"),
                                qApp->translate("Pump param", "Distance to waist"));
    _MI = new ParameterTS(Dims::linear(),
                          QStringLiteral("MI"),
                          QStringLiteral("M²"),
                          qApp->translate("Pump param", "Beam quality"));
    addParam(_waist, 100, mkm());
    addParam(_distance, 100, mm());
    addParam(_MI, 1);
}

//--------------------------------------------------------------------------------
//                               PumpParams_Front
//--------------------------------------------------------------------------------

PumpParams_Front::PumpParams_Front()
{
    _beamRadius = new ParameterTS(Dims::linear(),
                                  QStringLiteral("w"),
                                  QStringLiteral("ω"),
                                  qApp->translate("Pump param", "Beam radius"));
    _frontRadius = new ParameterTS(Dims::linear(),
                                   QStringLiteral("R"),
                                   QStringLiteral("R"),
                                   qApp->translate("Pump param", "Wavefront ROC"));
    _MI = new ParameterTS(Dims::linear(),
                          QStringLiteral("MI"),
                          QStringLiteral("M²"),
                          qApp->translate("Pump param", "Beam quality"));
    addParam(_beamRadius, 1000, mkm());
    addParam(_frontRadius, 100, mm());
    addParam(_MI, 1);
}

//--------------------------------------------------------------------------------
//                               PumpParams_TwoSections
//--------------------------------------------------------------------------------

PumpParams_TwoSections::PumpParams_TwoSections()
{
    _radius1 = new ParameterTS(Dims::linear(),
                               QStringLiteral("y_1"),
                               QStringLiteral("y<sub>1</sub>"),
                               qApp->translate("Pump param", "Beam radius 1"));
    _radius2 = new ParameterTS(Dims::linear(),
                               QStringLiteral("y_2"),
                               QStringLiteral("y<sub>2</sub>"),
                               qApp->translate("Pump param", "Beam radius 2"));
    _distance = new ParameterTS(Dims::linear(),
                                QStringLiteral("z_y"),
                                QStringLiteral("z<sub>y</sub>"),
                                qApp->translate("Pump param", "Distance between"));
    addParam(_radius1, 100, mkm());
    addParam(_radius2, 1000, mkm());
    addParam(_distance, 100, mm());
}

//--------------------------------------------------------------------------------
//                                PumpParams_RayVector
//--------------------------------------------------------------------------------

PumpParams_RayVector::PumpParams_RayVector()
{
    _radius = new ParameterTS(Dims::linear(),
                              QStringLiteral("y"),
                              QStringLiteral("y>"),
                              qApp->translate("Pump param", "Beam radius"));
    _angle = new ParameterTS(Dims::linear(),
                             QStringLiteral("V"),
                             QStringLiteral("V"),
                             qApp->translate("Pump param", "Half angle of divergence"));
    _distance = new ParameterTS(Dims::linear(),
                                QStringLiteral("z_y"),
                                QStringLiteral("z<sub>y</sub>"),
                                qApp->translate("Pump param", "Distance to radius"));
    addParam(_radius, 100, mkm());
    addParam(_angle, 10, deg());
    addParam(_distance, 100, mm());
}

//--------------------------------------------------------------------------------
//                                PumpParams_Complex
//--------------------------------------------------------------------------------

PumpParams_Complex::PumpParams_Complex()
{
    _real = new ParameterTS(Dims::none(),
                            QStringLiteral("Re"),
                            QStringLiteral("Re"),
                            qApp->translate("Pump param", "Real part"));
    _imag = new ParameterTS(Dims::none(),
                            QStringLiteral("Im"),
                            QStringLiteral("Im"),
                            qApp->translate("Pump param", "Imaginary part"));
    _MI = new ParameterTS(Dims::none(),
                          QStringLiteral("MI"),
                          QStringLiteral("M²"),
                          qApp->translate("Pump param", "Beam quality"));
    addParam(_real, 0);
    addParam(_imag, 0);
    addParam(_MI, 1);
}

//--------------------------------------------------------------------------------
//                              PumpParams_InvComplex
//--------------------------------------------------------------------------------

PumpParams_InvComplex::PumpParams_InvComplex() : PumpParams_Complex()
{
}

//--------------------------------------------------------------------------------
//                                      Pump
//--------------------------------------------------------------------------------

const QVector<PumpMode*>& Pump::allModes()
{
    static PumpMode_Waist waist;
    static PumpMode_Front front;
    static PumpMode_RayVector ray_vector;
    static PumpMode_TwoSections sections;
    static PumpMode_Complex complex;
    static PumpMode_InvComplex inv_complex;
    static QVector<PumpMode*> producers({
        reinterpret_cast<PumpMode*>(&waist),
        reinterpret_cast<PumpMode*>(&front),
        reinterpret_cast<PumpMode*>(&ray_vector),
        reinterpret_cast<PumpMode*>(&sections),
        reinterpret_cast<PumpMode*>(&complex),
        reinterpret_cast<PumpMode*>(&inv_complex),
    });
    return producers;
}

PumpMode* Pump::findByModeName(const QString& name)
{
    for (auto producer : allModes())
        if (producer->modeName() == name)
            return producer;
    return nullptr;
}

} // namespace Z
