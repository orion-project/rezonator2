#ifndef PUMP_PARAMS_H
#define PUMP_PARAMS_H

#include "Parameters.h"

#include <QApplication>

#define DECLARE_PUMP_PARAMS(mode_type, base_mode) \
class PumpParams_##mode_type : public base_mode \
{ \
public: \
    PumpParams_##mode_type(); \
    QString mode() const override { return QStringLiteral(# mode_type); }


#define DECLARE_PUMP_PARAMS_END(mode_type, mode_name, drawing_path) \
}; \
class PumpProducer_##mode_type : public PumpProducer \
{ \
public: \
    PumpParams* makePump() const override { return new PumpParams_##mode_type(); } \
    QString drawingPath() const override { return QStringLiteral(drawing_path); } \
    QString mode() const { return QStringLiteral(# mode_type); } \
};


#define PUMP_PARAM(name) \
public: \
    ParameterTS* name() const { return _##name; } \
private: \
    ParameterTS *_##name;


namespace Z {


class PumpParams
{
public:
    virtual ~PumpParams();
    virtual QString mode() const { return QString(); }
protected:
    ParametersBase _params;
    void addParam(ParameterTS *param, double value, Unit unit = Units::none());
};


class PumpProducer
{
public:
    virtual QString mode() const = 0;
    virtual PumpParams* makePump() const = 0;
    virtual QString drawingPath() const = 0;
};


class Pump {
public:
    static const QVector<PumpProducer*> &allPumpProducers();
    static PumpProducer* findByMode(const QString& mode);
};


typedef QList<PumpParams*> PumpsList;


DECLARE_PUMP_PARAMS(Waist, PumpParams)
    PUMP_PARAM(waist)
    PUMP_PARAM(distance)
    PUMP_PARAM(MI)
DECLARE_PUMP_PARAMS_END(Waist,
                        qApp->translate("Pump mode", "Waist"),
                        ":/drawing/pump_waist")

DECLARE_PUMP_PARAMS(Front, PumpParams)
    PUMP_PARAM(beamRadius)
    PUMP_PARAM(frontRadius)
    PUMP_PARAM(MI)
DECLARE_PUMP_PARAMS_END(Front,
                        qApp->translate("Pump mode", "Front"),
                        ":/drawing/pump_front")

DECLARE_PUMP_PARAMS(TwoSections, PumpParams)
    PUMP_PARAM(radius1)
    PUMP_PARAM(radius2)
    PUMP_PARAM(distance)
DECLARE_PUMP_PARAMS_END(TwoSections,
                        qApp->translate("Pump mode", "Two Sections"),
                        ":/drawing/pump_two_section")

DECLARE_PUMP_PARAMS(RayVector, PumpParams)
    PUMP_PARAM(radius)
    PUMP_PARAM(angle)
    PUMP_PARAM(distance)
DECLARE_PUMP_PARAMS_END(RayVector,
                        qApp->translate("Pump mode", "Ray Vector"),
                        ":/drawing/pump_ray_vector")

DECLARE_PUMP_PARAMS(Complex, PumpParams)
    PUMP_PARAM(real)
    PUMP_PARAM(imag)
    PUMP_PARAM(MI)
DECLARE_PUMP_PARAMS_END(Complex,
                        qApp->translate("Pump mode", "Complex"),
                        ":/drawing/pump_complex")

DECLARE_PUMP_PARAMS(InvComplex, PumpParams_Complex)
DECLARE_PUMP_PARAMS_END(InvComplex,
                        qApp->translate("Pump mode", "Inv. Complex"),
                        ":/drawing/pump_complex")

} // namespace Z

#endif // PUMP_PARAMS_H
