#ifndef PUMP_PARAMS_H
#define PUMP_PARAMS_H

#include "Parameters.h"

#include <QApplication>

#define DECLARE_PUMP_PARAMS(mode_class, base_mode_class) \
class PumpParams_##mode_class : public base_mode_class \
{ \
public: \
    PumpParams_##mode_class(); \
    QString modeName() const override { return QStringLiteral(# mode_class); }


#define DECLARE_PUMP_PARAMS_END(mode_class, mode_name, mode_descr, drawing_path, icon_path, help_topic) \
}; \
class PumpMode_##mode_class : public PumpMode \
{ \
public: \
    ~PumpMode_##mode_class() override; \
    PumpParams* makePump() const override { return new PumpParams_##mode_class(); } \
    QString drawingPath() const override { return QStringLiteral(drawing_path); } \
    QString iconPath() const override { return QStringLiteral(icon_path); } \
    QString helpTopic() const override { return QStringLiteral(help_topic); } \
    QString modeName() const override { return QStringLiteral(# mode_class); } \
    QString displayName() const override { return mode_name; } \
    QString description() const override { return mode_descr; } \
    static const PumpMode_##mode_class* instance() {\
        static PumpMode_##mode_class mode;\
        return &mode;\
    }\
};\


#define PUMP_PARAM(name) \
public: \
    Z::ParameterTS* name() const { return _##name; } \
private: \
    Z::ParameterTS *_##name;


class PumpParams
{
public:
    virtual ~PumpParams();
    virtual QString modeName() const { return QString(); }
    QString label() const { return _label; }
    QString title() const { return _title; }
    QString color() const { return _color; }
    Z::ParametersTS* params() { return &_params; }
    void setLabel(const QString& label) { _label = label; }
    void setTitle(const QString& title) { _title = title; }
    void setColor(const QString& color) { _color = color; }
    QString str() const { return _params.str(); }
    QString displayStr() const { return _params.displayStr(); }
    bool isActive() const { return _isActive; }
    void activate(bool on) { _isActive = on; }
protected:
    void addParam(Z::ParameterTS *param, double value, Z::Unit unit = Z::Units::none());
private:
    Z::ParametersTS _params;
    QString _label, _title;
    QString _color = "#555555";
    bool _isActive = false;
};


class PumpMode
{
public:
    virtual ~PumpMode();
    virtual QString modeName() const = 0;
    virtual PumpParams* makePump() const = 0;
    virtual QString helpTopic() const = 0;
    virtual QString iconPath() const = 0;
    virtual QString drawingPath() const = 0;
    virtual QString displayName() const = 0;
    virtual QString description() const = 0;
};


class Pumps {
public:
    static const QVector<const PumpMode*> &allModes();
    static const PumpMode *findByModeName(const QString& name);
    static QString labelPrefix();
    static bool isGeometric(const PumpParams*);
};


typedef QList<PumpParams*> PumpsList;


DECLARE_PUMP_PARAMS(Waist, PumpParams)
    PUMP_PARAM(waist)
    PUMP_PARAM(distance)
    PUMP_PARAM(MI)
DECLARE_PUMP_PARAMS_END(Waist,
                        qApp->translate("Pump mode", "Waist"),
                        qApp->translate("Pump mode", "Gaussian beam defined by its waist"),
                        ":/drawing/pump_waist",
                        ":/icons/pump_waist",
                        "pump-mode-waist")

DECLARE_PUMP_PARAMS(Front, PumpParams)
    PUMP_PARAM(beamRadius)
    PUMP_PARAM(frontRadius)
    PUMP_PARAM(MI)
DECLARE_PUMP_PARAMS_END(Front,
                        qApp->translate("Pump mode", "Front"),
                        qApp->translate("Pump mode", "Gaussian beam defined by its front"),
                        ":/drawing/pump_front",
                        ":/icons/pump_front",
                        "pump-mode-front")

DECLARE_PUMP_PARAMS(RayVector, PumpParams)
    PUMP_PARAM(radius)
    PUMP_PARAM(angle)
    PUMP_PARAM(distance)
DECLARE_PUMP_PARAMS_END(RayVector,
                        qApp->translate("Pump mode", "Ray Vector"),
                        qApp->translate("Pump mode", "Ray vector defined by radius and angle"),
                        ":/drawing/pump_ray_vector",
                        ":/icons/pump_ray_vector",
                        "pump-mode-vector")

DECLARE_PUMP_PARAMS(TwoSections, PumpParams)
    PUMP_PARAM(radius1)
    PUMP_PARAM(radius2)
    PUMP_PARAM(distance)
DECLARE_PUMP_PARAMS_END(TwoSections,
                        qApp->translate("Pump mode", "Two Sections"),
                        qApp->translate("Pump mode", "Ray vector defined by two sections"),
                        ":/drawing/pump_two_sections",
                        ":/icons/pump_two_sections",
                        "pump-mode-sections")

DECLARE_PUMP_PARAMS(Complex, PumpParams)
    PUMP_PARAM(real)
    PUMP_PARAM(imag)
    PUMP_PARAM(MI)
DECLARE_PUMP_PARAMS_END(Complex,
                        qApp->translate("Pump mode", "Complex"),
                        qApp->translate("Pump mode", "Gaussian beam defined by complex parameter"),
                        ":/drawing/pump_complex",
                        ":/icons/pump_complex",
                        "pump-mode-complex")

DECLARE_PUMP_PARAMS(InvComplex, PumpParams_Complex)
DECLARE_PUMP_PARAMS_END(InvComplex,
                        qApp->translate("Pump mode", "Inv. Complex"),
                        qApp->translate("Pump mode", "Gaussian beam defined by inverted complex parameter"),
                        ":/drawing/pump_complex",
                        ":/icons/pump_inv_complex",
                        "pump-mode-complex")

#endif // PUMP_PARAMS_H
