#include "Elements.h"

#include "Format.h"
#include "../funcs/PumpCalculator.h"
#include "core/OriFloatingPoint.h"

#include <math.h>

// TODO:NEXT-VER in general case all parameters units should be verified too.
// But currenly we have verification only in that places which always use correct uints (e.g. Element props dialog).

class CurvatureRadiusVerifier : public Z::ValueVerifier
{
public:
    bool enabled() const override { return true; }
    QString verify(const Z::Value& value) const override;
};

class FocalLengthVerifier : public Z::ValueVerifier
{
public:
    bool enabled() const override { return true; }
    QString verify(const Z::Value& value) const override;
};

QString CurvatureRadiusVerifier::verify(const Z::Value& value) const
{
    if (Double(value.value()).is(0))
        return qApp->translate("Param", "Curvature radius can not be zero.");
    return QString();
}

QString FocalLengthVerifier::verify(const Z::Value& value) const
{
    if (Double(value.value()).is(0))
        return qApp->translate("Param", "Focal length can not be zero.");
    return QString();
}

CurvatureRadiusVerifier* globalCurvatureRadiusVerifier()
{
    static CurvatureRadiusVerifier v; return &v;
}

FocalLengthVerifier* globalFocalLengthVerifier()
{
    static FocalLengthVerifier v; return &v;
}

//------------------------------------------------------------------------------
//                             ElemEmptyRange
//------------------------------------------------------------------------------

void ElemEmptyRange::calcMatrixInternal()
{
    _mt.assign(1, lengthSI(), 0, 1);
    _ms = _mt;
    _mt_inv = _mt;
    _ms_inv = _ms;
}

void ElemEmptyRange::setSubRangeSI(double value)
{
    _mt1.assign(1, value, 0, 1);
    _ms1 = _mt1;
    _mt2.assign(1, lengthSI() - value, 0, 1);
    _ms2 = _mt2;
}

//------------------------------------------------------------------------------
//                             ElemMediaRange
//------------------------------------------------------------------------------

ElemMediumRange::ElemMediumRange() : ElementRange()
{
    _ior->setVisible(true);
}

void ElemMediumRange::calcMatrixInternal()
{
    _mt.assign(1, lengthSI(), 0, 1);
    _ms = _mt;
    _mt_inv = _mt;
    _ms_inv = _ms;
}

void ElemMediumRange::setSubRangeSI(double value)
{
    _mt1.assign(1, value, 0, 1);
    _ms1 = _mt1;
    _mt2.assign(1, lengthSI() - value, 0, 1);
    _ms2 = _mt2;
}

//------------------------------------------------------------------------------
//                                ElemPlate
//------------------------------------------------------------------------------

ElemPlate::ElemPlate() : ElementRange()
{
    _ior->setVisible(true);
}

void ElemPlate::calcMatrixInternal()
{
    _mt.assign(1, lengthSI() / ior(), 0, 1);
    _ms = _mt;
    _mt_inv = _mt;
    _ms_inv = _ms;
}

void ElemPlate::setSubRangeSI(double value)
{
    _mt1.assign(1, value / ior(), 0, 1/ior());
    _ms1 = _mt1;
    _mt2.assign(1, lengthSI() - value, 0, ior());
    _ms2 = _mt2;
}

//------------------------------------------------------------------------------
//                              ElemFlatMirror
//------------------------------------------------------------------------------

ElemFlatMirror::ElemFlatMirror() : Element()
{
}

//------------------------------------------------------------------------------
//                              ElemCurveMirror
//------------------------------------------------------------------------------

ElemCurveMirror::ElemCurveMirror()
{
    _radius = new Z::Parameter(Z::Dims::linear(), QStringLiteral("R"), QStringLiteral("R"),
                               qApp->translate("Param", "Radius of curvature"),
                               qApp->translate("Param", "Positive for concave mirror, negative for convex mirror."));
    _alpha = new Z::Parameter(Z::Dims::angular(), QStringLiteral("Alpha"), Z::Strs::alpha(),
                              qApp->translate("Param", "Angle of incidence "),
                              qApp->translate("Param", "Zero angle is normal incidence."));

    _radius->setValue(100_mm);
    _alpha->setValue(0_deg);

    addParam(_radius);
    addParam(_alpha);

    setOption(Element_ChangesWavefront);

    _radius->setVerifier(globalCurvatureRadiusVerifier());
}

void ElemCurveMirror::calcMatrixInternal()
{
    _mt.assign(1, 0, -2.0 / radius() / cos(alpha()), 1);
    _ms.assign(1, 0, -2.0 / radius() * cos(alpha()), 1);
    _mt_inv = _mt;
    _ms_inv = _ms;
}

//------------------------------------------------------------------------------
//                                ElemThinLens
//------------------------------------------------------------------------------

ElemThinLens::ElemThinLens()
{
    _focus = new Z::Parameter(Z::Dims::linear(), QStringLiteral("F"), QStringLiteral("F"),
                               qApp->translate("Param", "Focal length"),
                               qApp->translate("Param", "Positive for collecting lens, negative for diverging lens."));
    _alpha = new Z::Parameter(Z::Dims::angular(), QStringLiteral("Alpha"), Z::Strs::alpha(),
                              qApp->translate("Param", "Angle of incidence "),
                              qApp->translate("Param", "Zero angle is normal incidence."));

    _focus->setValue(100_mm);
    _alpha->setValue(0_deg);

    addParam(_focus);
    addParam(_alpha);

    setOption(Element_ChangesWavefront);

    _focus->setVerifier(globalFocalLengthVerifier());
}

void ElemThinLens::calcMatrixInternal()
{
    _mt.assign(1.0, 0.0, -1.0 / focus() / cos(alpha()), 1.0);
    _ms.assign(1.0, 0.0, -1.0 / focus() * cos(alpha()), 1.0);
    _mt_inv = _mt;
    _ms_inv = _ms;
}

//------------------------------------------------------------------------------
//                              ElemCylinderLensT
//------------------------------------------------------------------------------

void ElemCylinderLensT::calcMatrixInternal()
{
    _mt.assign(1.0, 0.0, -1.0 / focus() / cos(alpha()), 1.0);
    _ms.unity();
    _mt_inv = _mt;
    _ms_inv = _ms;
}

//------------------------------------------------------------------------------
//                              ElemCylinderLensS
//------------------------------------------------------------------------------

void ElemCylinderLensS::calcMatrixInternal()
{
    _mt.unity();
    _ms.assign(1.0, 0.0, -1.0 / focus() * cos(alpha()), 1.0);
    _mt_inv = _mt;
    _ms_inv = _ms;
}

//------------------------------------------------------------------------------
//                             ElemTiltedCrystal
//------------------------------------------------------------------------------

ElemTiltedCrystal::ElemTiltedCrystal() : ElementRange()
{
    _ior->setVisible(true);

    _alpha = new Z::Parameter(Z::Dims::angular(), QStringLiteral("Alpha"), Z::Strs::alpha(),
                              qApp->translate("Param", "Angle of incidence "),
                              qApp->translate("Param", "Zero angle is normal incidence."));

    _alpha->setValue(0_deg);

    addParam(_alpha);
}

void ElemTiltedCrystal::calcMatrixInternal()
{
    const double L = lengthSI();
    const double a = alpha();
    const double n = ior();

    _mt.assign(1, L * n * SQR(cos(a)) / (SQR(n) - SQR(sin(a))), 0.0, 1.0);
    _ms.assign(1, L / n, 0, 1);
    _mt_inv = _mt;
    _ms_inv = _ms;
}

void ElemTiltedCrystal::setSubRangeSI(double value)
{
    const double n = ior();
    const double cos_a = cos(alpha());
    const double cos_b = cos(asin(sin(alpha()) / n)); // cosine of angle inside medium
    const double cos_ab = cos_a / cos_b;
    const double cos_ba = cos_b / cos_a;
    const double L1 = value;
    const double L2 = lengthSI() - value;

    //  --> /:: -->  half lengh * input to medium
    _mt1.assign(cos_ba, L1/n * cos_ab, 0, 1/n * cos_ab);
    _ms1.assign(1, L1/n, 0, 1/n);

    //  --> ::/ -->  output from media * half length
    _mt2.assign(cos_ab, L2 * cos_ab, 0, n * cos_ba);
    _ms2.assign(1, L2, 0, n);
}

//------------------------------------------------------------------------------
//                              ElemTiltedPlate
//------------------------------------------------------------------------------

void ElemTiltedPlate::calcMatrixInternal()
{
    const double L = lengthSI();
    const double n = ior();
    const double sin_a = sin(alpha());
    const double s = n*n - sin_a*sin_a;

    _mt.assign(1, L * n*n * (1 - sin_a*sin_a) / sqrt(s*s*s), 0, 1);
    _ms.assign(1, L / sqrt(s), 0, 1);
    _mt_inv = _mt;
    _ms_inv = _ms;
}

void ElemTiltedPlate::setSubRangeSI(double value)
{
    const double n = ior();
    const double cos_a = cos(alpha());
    const double cos_b = cos(asin( sin(alpha()) / n)); // cosine of angle inside medium
    const double cos_ab = cos_a / cos_b;
    const double cos_ba = cos_b / cos_a;
    const double L1 = value;
    const double L2 = axisLengthSI() - L1;

    //  --> /:: -->  half lengh * input to medium
    _mt1.assign(cos_ba, L1/n * cos_ab, 0, 1/n * cos_ab);
    _ms1.assign(1, L1/n, 0, 1/n);

    //  --> ::/ -->  output from medium * half length
    _mt2.assign(cos_ab, L2 * cos_ab, 0, n * cos_ba);
    _ms2.assign(1, L2, 0, n);
}

double ElemTiltedPlate::axisLengthSI() const
{
    return lengthSI() / cos( asin( sin( alpha() ) / ior() ) );
}

//------------------------------------------------------------------------------
//                            ElemBrewsterCrystal
//------------------------------------------------------------------------------

ElemBrewsterCrystal::ElemBrewsterCrystal() : ElementRange()
{
    _ior->setVisible(true);
}

void ElemBrewsterCrystal::calcMatrixInternal()
{
    const double L = lengthSI();
    const double n = ior();

    _ms.assign(1, L / n, 0, 1);
    _mt.assign(1, _ms.B / SQR(n), 0, 1);
    _mt_inv = _mt;
    _ms_inv = _ms;
}

void ElemBrewsterCrystal::setSubRangeSI(double value)
{
    const double n = ior();
    const double L1 = value;
    const double L2 = lengthSI() - L1;

    //  --> /:: -->  half lengh * input to medium
    _mt1.assign(n, L1/n/n, 0, 1/n/n);
    _ms1.assign(1, L1/n,   0, 1/n);

    //  --> ::/ -->  output from media * half length
    _mt2.assign(1/n, L2/n, 0, n*n);
    _ms2.assign(1, L2, 0, n);
}

//------------------------------------------------------------------------------
//                             ElemBrewsterPlate
//------------------------------------------------------------------------------

ElemBrewsterPlate::ElemBrewsterPlate() : ElementRange()
{
    _ior->setVisible(true);
}

void ElemBrewsterPlate::calcMatrixInternal()
{
    const double n = ior();

    _ms.assign(1, axisLengthSI() / n, 0, 1);
    _mt.assign(1, _ms.B / SQR(n), 0, 1);
    _mt_inv = _mt;
    _ms_inv = _ms;
}

void ElemBrewsterPlate::setSubRangeSI(double value)
{
    const double L1 = value;
    const double L2 = axisLengthSI() - L1;
    const double n = ior();

    //  --> /:: -->  half lengh * input to medium
    _mt1.assign(n, L1/n/n, 0, 1/n/n);
    _ms1.assign(1, L1/n, 0, 1/n);

    //  --> ::/ -->  output from media * half length
    _mt2.assign(1/n, L2/n, 0, n*n);
    _ms2.assign(1, L2, 0, n);
}

double ElemBrewsterPlate::axisLengthSI() const
{
    // L_eff = L / cos( asin( sin( atan(n) )/n ) ) = L * Sqrt(n^2 + 1) / n
    const double n = ior();
    return lengthSI() * sqrt(n*n + 1) / n;
}

//------------------------------------------------------------------------------
//                                ElemMatrix
//------------------------------------------------------------------------------

ElemMatrix::ElemMatrix()
{
    _At = new Z::Parameter(Z::Dims::none(), "At", "At", "At");
    _Bt = new Z::Parameter(Z::Dims::linear(), "Bt", "Bt", "Bt");
    _Ct = new Z::Parameter(Z::Dims::fixed(), "Ct", "Ct", "Ct");
    _Dt = new Z::Parameter(Z::Dims::none(), "Dt", "Dt", "Dt");
    _As = new Z::Parameter(Z::Dims::none(), "As", "As", "As");
    _Bs = new Z::Parameter(Z::Dims::linear(), "Bs", "Bs", "Bs");
    _Cs = new Z::Parameter(Z::Dims::fixed(), "Cs", "Cs", "Cs");
    _Ds = new Z::Parameter(Z::Dims::none(), "Ds", "Ds", "Ds");

    _At->setValue(1);
    _Bt->setValue(0_m);
    _Ct->setValue(Z::Value(0, Z::Units::inv_m()));
    _Dt->setValue(1);
    _As->setValue(1);
    _Bs->setValue(0_m);
    _Cs->setValue(Z::Value(0, Z::Units::inv_m()));
    _Ds->setValue(1);

    addParam(_At);
    addParam(_Bt);
    addParam(_Ct);
    addParam(_Dt);
    addParam(_As);
    addParam(_Bs);
    addParam(_Cs);
    addParam(_Ds);
}

void ElemMatrix::setMatrix(int offset, const double& a, const double& b, const double& c, const double& d)
{
    params().at(offset+0)->setValue(Z::Value(a, Z::Units::none()));
    params().at(offset+1)->setValue(Z::Value(b, Z::Units::m()));
    params().at(offset+2)->setValue(Z::Value(c, Z::Units::inv_m()));
    params().at(offset+3)->setValue(Z::Value(d, Z::Units::none()));
}

// TODO:NEXT-VER checkParameter(): can A and D be 0 and what does it mean?

void ElemMatrix::calcMatrixInternal()
{
    _mt.assign(_params.at(0)->value().toSi(), _params.at(1)->value().toSi(),
               _params.at(2)->value().toSi(), _params.at(3)->value().toSi());
    _ms.assign(_params.at(4)->value().toSi(), _params.at(5)->value().toSi(),
               _params.at(6)->value().toSi(), _params.at(7)->value().toSi());
    _mt_inv = _mt;
    _ms_inv = _ms;
}

//------------------------------------------------------------------------------
//                                ElemMatrix
//------------------------------------------------------------------------------

ElemMatrix1::ElemMatrix1()
{
    auto A = new Z::Parameter(Z::Dims::none(), "A", "A", "A");
    auto B = new Z::Parameter(Z::Dims::linear(), "B", "B", "B");
    auto C = new Z::Parameter(Z::Dims::fixed(), "C", "C", "C");
    auto D = new Z::Parameter(Z::Dims::none(), "D", "D", "D");

    A->setValue(1);
    B->setValue(0_m);
    C->setValue(Z::Value(0, Z::Units::inv_m()));
    D->setValue(1);

    addParam(A);
    addParam(B);
    addParam(C);
    addParam(D);
}

void ElemMatrix1::setMatrix(const double& a, const double& b, const double& c, const double& d)
{
    params().at(0)->setValue(Z::Value(a, Z::Units::none()));
    params().at(1)->setValue(Z::Value(b, Z::Units::m()));
    params().at(2)->setValue(Z::Value(c, Z::Units::inv_m()));
    params().at(3)->setValue(Z::Value(d, Z::Units::none()));
}

void ElemMatrix1::calcMatrixInternal()
{
    _mt.assign(_params.at(0)->value().toSi(), _params.at(1)->value().toSi(),
               _params.at(2)->value().toSi(), _params.at(3)->value().toSi());
    _ms = _mt;
    _mt_inv = _mt;
    _ms_inv = _ms;
}

//------------------------------------------------------------------------------
//                                ElemPoint
//------------------------------------------------------------------------------

ElemPoint::ElemPoint() : Element()
{
}

//------------------------------------------------------------------------------
//                             ElemNormalInterface
//------------------------------------------------------------------------------

void ElemNormalInterface::calcMatrixInternal()
{
    const double n1 = ior1();
    const double n2 = ior2();

    _mt.assign(1, 0, 0, n1 / n2);
    _ms = _mt;

    _mt_inv.assign(1, 0, 0, n2 / n1);
    _ms_inv = _mt_inv;
}

//------------------------------------------------------------------------------
//                             ElemBrewsterInterface
//------------------------------------------------------------------------------

void ElemBrewsterInterface::calcMatrixInternal()
{
    const double n1 = ior1();
    const double n2 = ior2();

    _mt.assign(n2/n1, 0, 0, (n1/n2)*(n1/n2));
    _ms.assign(1, 0, 0, n1/n2);

    _mt_inv.assign(n1/n2, 0, 0, (n2/n1)*(n2/n1));
    _ms_inv.assign(1, 0, 0, n2/n1);
}

//------------------------------------------------------------------------------
//                             ElemTiltedInterface
//------------------------------------------------------------------------------

ElemTiltedInterface::ElemTiltedInterface() : ElementInterface()
{
    _alpha = new Z::Parameter(Z::Dims::angular(), QStringLiteral("Alpha"), Z::Strs::alpha(),
                              qApp->translate("Param", "Angle of incidence "),
                              qApp->translate("Param", "Zero angle is normal incidence. "
                              "Negative value sets angle from the side of medium <i>n<sub>2</sub></i>."));
    _alpha->setValue(0_deg);

    addParam(_alpha);
}

void ElemTiltedInterface::calcMatrixInternal()
{
    const double n1 = ior1();
    const double n2 = ior2();
    const double angle = alpha();
    /*    \ |
       n1  \| alpha      Positive angle value means we set angle Alpha (angle at the side of medium n1)
     -------+-------     Negative angle value means we set angle Beta (angle at the side of medium n2)
      beta  |\   n2
            | \          */
    const double cos_a = angle < 0 ? cos(asin( sin(angle) * n2 / n1)) : cos(angle);
    const double cos_b = angle < 0 ? cos(qAbs(angle)): cos(asin( sin(angle) * n1 / n2));

    _mt.assign(cos_b/cos_a, 0, 0, (n1/n2)*(cos_a/cos_b));
    _ms.assign(1, 0, 0, n1/n2);

    _mt_inv.assign(cos_a/cos_b, 0, 0, (n2/n1)*(cos_b/cos_a));
    _ms_inv.assign(1, 0, 0, n2/n1);
}

//------------------------------------------------------------------------------
//                             ElemSphericalInterface
//------------------------------------------------------------------------------

ElemSphericalInterface::ElemSphericalInterface() : ElementInterface()
{
    _radius = new Z::Parameter(Z::Dims::linear(), QStringLiteral("R"), QStringLiteral("R"),
                               qApp->translate("Param", "Radius of curvature"),
                               qApp->translate("Param", "Negative value means left-bulged surface, "
                                                        "positive value means right-bulged surface."));
    _radius->setValue(100_mm);

    addParam(_radius);

    _radius->setVerifier(globalCurvatureRadiusVerifier());
}

void ElemSphericalInterface::calcMatrixInternal()
{
    const double n1 = ior1();
    const double n2 = ior2();
    const double R = radius();

    _mt.assign(1, 0, (n2-n1)/R/n2, n1/n2);
    _ms = _mt;

    _mt_inv.assign(1, 0, (n1-n2)/(-R)/n1, n2/n1);
    _ms_inv = _mt_inv;
}

//------------------------------------------------------------------------------
//                             ElemThickLens
//------------------------------------------------------------------------------

ElemThickLens::ElemThickLens() : ElementRange()
{
    _ior->setVisible(true);

    _radius1 = new Z::Parameter(Z::Dims::linear(), QStringLiteral("R1"), QStringLiteral("R1"),
                                qApp->translate("Param", "Left radius of curvature"),
                                qApp->translate("Param", "Negative value means left-bulged surface, "
                                                         "positive value means right-bulged surface."));
    _radius2 = new Z::Parameter(Z::Dims::linear(), QStringLiteral("R2"), QStringLiteral("R2"),
                                qApp->translate("Param", "Right radius of curvature"),
                                qApp->translate("Param", "Negative value means left-bulged surface, "
                                                         "positive value means right-bulged surface."));

    _radius1->setValue(-100_mm);
    _radius2->setValue(100_mm);

    addParam(_radius1);
    addParam(_radius2);

    _radius1->setVerifier(globalCurvatureRadiusVerifier());
    _radius2->setVerifier(globalCurvatureRadiusVerifier());

    setOption(Element_Asymmetrical);
}

void ElemThickLens::calcMatrixInternal()
{
    const double L = lengthSI();
    const double n = ior();
    const double R1 = radius1();
    const double R2 = radius2();

    const double A = 1 + (L/R1)*(n-1)/n;
    const double B = L/n;
    const double C = (n-1)*(1/R1 - 1/R2) - L/R1/R2*(n-1)*(n-1)/n;
    const double D = 1 - (L/R2)*(n-1)/n;

    _mt.assign(A, B, C, D);
    _ms = _mt;

    const double R1_inv = -R2;
    const double R2_inv = -R1;

    const double A_inv = 1 + (L/R1_inv)*(n-1)/n;
    const double B_inv = L/n;
    const double C_inv = (n-1)*(1/R1_inv - 1/R2_inv) - L/R1_inv/R2_inv*(n-1)*(n-1)/n;
    const double D_inv = 1 - (L/R2_inv)*(n-1)/n;

    _mt_inv.assign(A_inv, B_inv, C_inv, D_inv);
    _ms_inv = _mt_inv;
}

void ElemThickLens::setSubRangeSI(double value)
{
    const double n = ior();
    const double L1 = value;
    const double L2 = lengthSI() - L1;
    const double R1 = radius1();
    const double R2 = radius2();

    //  --> (:: -->  half lengh * input to medium
    _mt1.assign(1 + L1*(n-1)/R1/n, L1/n, (n-1)/R1/n, 1/n);
    _ms1 = _mt1;

    //  --> ::) -->  output from media * half length
    _mt2.assign(1, L2, (1-n)/R2, L2*(1-n)/R2 + n);
    _ms2 = _mt2;
}

//------------------------------------------------------------------------------
//                             ElemGrinLens
//------------------------------------------------------------------------------

ElemGrinLens::ElemGrinLens() : ElementRange() {
    _ior->setVisible(true);

    _ior2t = new Z::Parameter(Z::Dims::fixed(),
        QStringLiteral("n2t"), QStringLiteral("n2t"),
        qApp->translate("Param", "IOR gradient (T)"),
        qApp->translate("Param", "Radial gradient of index of refraction in tangential plane"));
    _ior2s = new Z::Parameter(Z::Dims::fixed(),
        QStringLiteral("n2s"), QStringLiteral("n2s"),
        qApp->translate("Param", "IOR gradient (S)"),
        qApp->translate("Param", "Radial gradient of index of refraction in sagittal plane"));

    _ior->setValue(2);
    _ior2t->setValue(Z::Value(1, Z::Units::inv_m2()));
    _ior2s->setValue(Z::Value(1, Z::Units::inv_m2()));

    addParam(_ior2t);
    addParam(_ior2s);
}

void ElemGrinLens::calcMatrixInternal() {
    const double L = lengthSI();
    const double n0 = qAbs(ior());
    const double n2t = ior2t();
    const double n2s = ior2s();

    // When n2 = 0 then A = 1, C = 0, D = 1, B = 0/0 -> L/n0

    if (n2t > 0) {
        const double g = sqrt(n2t / n0);
        _mt.assign(cos(g*L), sin(g*L)/n0/g, -n0*g*sin(g*L), cos(g*L));
    } else if (n2t < 0) {
        const double g = sqrt(-n2t / n0);
        _mt.assign(cosh(g*L), sinh(g*L)/n0/g, n0*g*sinh(g*L), cosh(g*L));
    } else _mt.assign(1, L/n0, 0, 1);

    if (n2s > 0) {
        const double g = sqrt(n2s / n0);
        _ms.assign(cos(g*L), sin(g*L)/n0/g, -n0*g*sin(g*L), cos(g*L));
    } else if (n2s < 0) {
        const double g = sqrt(-n2s / n0);
        _ms.assign(cosh(g*L), sinh(g*L)/n0/g, n0*g*sinh(g*L), cosh(g*L));
    } else _ms.assign(1, L/n0, 0, 1);
}

void ElemGrinLens::setSubRangeSI(double value) {
    const double L1 = value;
    const double L2 = lengthSI() - L1;
    const double n0 = qAbs(ior());
    const double n2t = ior2t();
    const double n2s = ior2s();

    if (n2t > 0) {
        const double g = sqrt(n2t / n0);
        _mt1.assign(cos(g*L1), sin(g*L1)/n0/g, -g*sin(g*L1), cos(g*L1)/n0);
        _mt2.assign(cos(g*L2), sin(g*L2)/g, -n0*g*sin(g*L2), n0*cos(g*L2));
    } else if (n2t < 0) {
        const double g = sqrt(-n2t / n0);
        _mt1.assign(cosh(g*L1), sinh(g*L1)/n0/g, g*sinh(g*L1), cosh(g*L1)/n0);
        _mt2.assign(cosh(g*L2), sinh(g*L2)/g, n0*g*sinh(g*L2), n0*cosh(g*L2));
    } else {
        _mt1.assign(1, L1/n0, 0, 1/n0);
        _mt2.assign(1, L2, 0, n0);
    }

    if (n2s > 0) {
        const double g = sqrt(n2s / n0);
        _ms1.assign(cos(g*L1), sin(g*L1)/n0/g, -g*sin(g*L1), cos(g*L1)/n0);
        _ms2.assign(cos(g*L2), sin(g*L2)/g, -n0*g*sin(g*L2), n0*cos(g*L2));
    } else if (n2s < 0) {
        const double g = sqrt(-n2s / n0);
        _ms1.assign(cosh(g*L1), sinh(g*L1)/n0/g, g*sinh(g*L1), cosh(g*L1)/n0);
        _ms2.assign(cosh(g*L2), sinh(g*L2)/g, n0*g*sinh(g*L2), n0*cosh(g*L2));
    } else {
        _ms1.assign(1, L1/n0, 0, 1/n0);
        _ms2.assign(1, L2, 0, n0);
    }
}

//------------------------------------------------------------------------------
//                             ElemGrinMedium
//------------------------------------------------------------------------------

ElemGrinMedium::ElemGrinMedium() : ElementRange() {
    _ior->setVisible(true);

    _ior2t = new Z::Parameter(Z::Dims::fixed(),
        QStringLiteral("n2t"), QStringLiteral("n2t"),
        qApp->translate("Param", "IOR gradient (T)"),
        qApp->translate("Param", "Radial gradient of index of refraction in tangential plane"));
    _ior2s = new Z::Parameter(Z::Dims::fixed(),
        QStringLiteral("n2s"), QStringLiteral("n2s"),
        qApp->translate("Param", "IOR gradient (S)"),
        qApp->translate("Param", "Radial gradient of index of refraction in sagittal plane"));

    _ior->setValue(2);
    _ior2t->setValue(Z::Value(1, Z::Units::inv_m2()));
    _ior2s->setValue(Z::Value(1, Z::Units::inv_m2()));

    addParam(_ior2t);
    addParam(_ior2s);
}

void ElemGrinMedium::calcMatrixInternal() {
    const double L = lengthSI();
    const double n0 = qAbs(ior());
    const double n2t = ior2t();
    const double n2s = ior2s();

    // When n2 = 0 then A = 1, C = 0, D = 1, B = 0/0 -> L

    if (n2t > 0) {
        const double g = sqrt(n2t / n0);
        _mt.assign(cos(g*L), sin(g*L)/g, -g*sin(g*L), cos(g*L));
    } else if (n2t < 0) {
        const double g = sqrt(-n2t / n0);
        _mt.assign(cosh(g*L), sinh(g*L)/g, g*sinh(g*L), cosh(g*L));
    } else _mt.assign(1, L, 0, 1);

    if (n2s > 0) {
        const double g = sqrt(n2s / n0);
        _ms.assign(cos(g*L), sin(g*L)/g, -g*sin(g*L), cos(g*L));
    } else if (n2s < 0) {
        const double g = sqrt(-n2s / n0);
        _ms.assign(cosh(g*L), sinh(g*L)/g, g*sinh(g*L), cosh(g*L));
    } else _ms.assign(1, L, 0, 1);
}

void ElemGrinMedium::setSubRangeSI(double value) {
    const double L1 = value;
    const double L2 = lengthSI() - L1;
    const double n0 = qAbs(ior());
    const double n2t = ior2t();
    const double n2s = ior2s();

    if (n2t > 0) {
        const double g = sqrt(n2t / n0);
        _mt1.assign(cos(g*L1), sin(g*L1)/g, -g*sin(g*L1), cos(g*L1));
        _mt2.assign(cos(g*L2), sin(g*L2)/g, -g*sin(g*L2), cos(g*L2));
    } else if (n2t < 0) {
        const double g = sqrt(-n2t / n0);
        _mt1.assign(cosh(g*L1), sinh(g*L1)/g, g*sinh(g*L1), cosh(g*L1));
        _mt2.assign(cosh(g*L2), sinh(g*L2)/g, g*sinh(g*L2), cosh(g*L2));
    } else {
        _mt1.assign(1, L1, 0, 1);
        _mt2.assign(1, L2, 0, 1);
    }

    if (n2s > 0) {
        const double g = sqrt(n2s / n0);
        _ms1.assign(cos(g*L1), sin(g*L1)/g, -g*sin(g*L1), cos(g*L1));
        _ms2.assign(cos(g*L2), sin(g*L2)/g, -g*sin(g*L2), cos(g*L2));
    } else if (n2s < 0) {
        const double g = sqrt(-n2s / n0);
        _ms1.assign(cosh(g*L1), sinh(g*L1)/g, g*sinh(g*L1), cosh(g*L1));
        _ms2.assign(cosh(g*L2), sinh(g*L2)/g, g*sinh(g*L2), cosh(g*L2));
    } else {
        _ms1.assign(1, L1, 0, 1);
        _ms2.assign(1, L2, 0, 1);
    }
}

//------------------------------------------------------------------------------
//                             ElemAxiconMirror
//------------------------------------------------------------------------------

ElemAxiconMirror::ElemAxiconMirror() : ElementDynamic()
{
    _theta = new Z::Parameter(Z::Dims::angular(), QStringLiteral("Theta"), Z::Strs::theta(),
                              qApp->translate("Param", "Axicon angle"));
    _alpha = new Z::Parameter(Z::Dims::angular(), QStringLiteral("Alpha"), Z::Strs::alpha(),
                              qApp->translate("Param", "Angle of incidence "),
                              qApp->translate("Param", "Zero angle is normal incidence."));

    _theta->setValue(1_deg);
    _alpha->setValue(0_deg);

    addParam(_theta);
    addParam(_alpha);

    setOption(Element_ChangesWavefront);
}

void ElemAxiconMirror::calcDynamicMatrix(const CalcParams& p)
{
    auto beamT = p.pumpCalcT->calc(*p.Mt, p.prevElemWavelenSI);
    auto beamS = p.pumpCalcS->calc(*p.Ms, p.prevElemWavelenSI);

    auto cosA = cos(alpha());
    auto tmp = 2 * theta();

    _mt_dyn.assign(1, 0, -tmp / qAbs(beamT.beamRadius) / cosA, 1);
    _ms_dyn.assign(1, 0, -tmp / qAbs(beamS.beamRadius) * cosA, 1);
}

//------------------------------------------------------------------------------
//                             ElemAxiconLens
//------------------------------------------------------------------------------

ElemAxiconLens::ElemAxiconLens() : ElementDynamic()
{
    _theta = new Z::Parameter(Z::Dims::angular(), QStringLiteral("Theta"), Z::Strs::theta(),
                              qApp->translate("Param", "Axicon angle"));
    _alpha = new Z::Parameter(Z::Dims::angular(), QStringLiteral("Alpha"), Z::Strs::alpha(),
                              qApp->translate("Param", "Angle of incidence "),
                              qApp->translate("Param", "Zero angle is normal incidence."));
    _ior = new Z::Parameter(Z::Dims::none(),
                            QStringLiteral("n"), QStringLiteral("n"),
                            qApp->translate("Param", "Index of refraction"));

    _theta->setValue(1_deg);
    _alpha->setValue(0_deg);
    _ior->setValue(1.5);

    addParam(_theta);
    addParam(_alpha);
    addParam(_ior);

    setOption(Element_ChangesWavefront);
}

void ElemAxiconLens::calcDynamicMatrix(const CalcParams& p)
{
    auto beamT = p.pumpCalcT->calc(*p.Mt, p.prevElemWavelenSI);
    auto beamS = p.pumpCalcS->calc(*p.Ms, p.prevElemWavelenSI);

    auto cosA = cos(alpha());
    auto tmp = asin(sin(theta()) * ior()) - theta();

    _mt_dyn.assign(1, 0, -tmp / qAbs(beamT.beamRadius) / cosA, 1);
    _ms_dyn.assign(1, 0, -tmp / qAbs(beamS.beamRadius) * cosA, 1);
}
