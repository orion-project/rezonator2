#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "Element.h"

#include <QApplication>

DECLARE_ELEMENT(ElemEmptyRange, Element)
    ElemEmptyRange();
    TYPE_NAME(qApp->translate("Elements", "Empty space"))
    DEFAULT_LABEL("d")
    CALC_MATRIX
    SUB_RANGE
    Z::Parameter* paramLength() const { return _length; }
    Z::Parameter* paramIor() const { return _ior; }
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemMediumRange, Element)
    ElemMediumRange();
    TYPE_NAME(qApp->translate("Elements", "Space filled with medium"))
    DEFAULT_LABEL("d")
    CALC_MATRIX
    SUB_RANGE
    Z::Parameter* paramLength() const { return _length; }
    Z::Parameter* paramIor() const { return _ior; }
    double ior() const { return _ior->value().value(); }
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemPlate, Element)
    ElemPlate();
    TYPE_NAME(qApp->translate("Elements", "Plate of matter"))
    DEFAULT_LABEL("G")
    CALC_MATRIX
    SUB_RANGE
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemFlatMirror, Element)
    ElemFlatMirror();
    TYPE_NAME(qApp->translate("Elements", "Flat mirror"))
    DEFAULT_LABEL("M")
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemCurveMirror, Element)
    ElemCurveMirror();
    TYPE_NAME(qApp->translate("Elements", "Spherical mirror"))
    DEFAULT_LABEL("M")
    CALC_MATRIX
    //CHECK_PARAM
    double radius() const { return _radius->value().toSi(); }
    double alpha() const { return _alpha->value().toSi(); }
private:
    Z::Parameter *_radius, *_alpha, *_aper;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemThinLens, Element)
    ElemThinLens();
    TYPE_NAME(qApp->translate("Elements", "Thin lens"))
    DEFAULT_LABEL("F")
    CALC_MATRIX
    //CHECK_PARAM
    double focus() const { return _focus->value().toSi(); }
    double alpha() const { return _alpha->value().toSi(); }
protected:
    Z::Parameter *_focus, *_alpha;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemCylinderLensT, ElemThinLens)
    TYPE_NAME(qApp->translate("Elements", "Thin cylindrical tangential lens"))
    DEFAULT_LABEL("F")
    CALC_MATRIX
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemCylinderLensS, ElemThinLens)
    TYPE_NAME(qApp->translate("Elements", "Thin cylindrical sagittal lens"))
    DEFAULT_LABEL("F")
    CALC_MATRIX
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemTiltedCrystal, Element)
    ElemTiltedCrystal();
    TYPE_NAME(qApp->translate("Elements", "Tilted plane-parallel crystal"))
    DEFAULT_LABEL("G")
    CALC_MATRIX
    SUB_RANGE
    double alpha() const { return _alpha->value().toSi(); }
protected:
    Z::Parameter *_alpha;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemTiltedPlate, ElemTiltedCrystal)
    TYPE_NAME(qApp->translate("Elements", "Tilted plane-parallel plate"))
    DEFAULT_LABEL("G")
    CALC_MATRIX
    SUB_RANGE
    AXIS_LEN
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemBrewsterCrystal, Element)
    ElemBrewsterCrystal();
    TYPE_NAME(qApp->translate("Elements", "Brewster plane-parallel crystal"))
    DEFAULT_LABEL("G")
    CALC_MATRIX
    SUB_RANGE
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemBrewsterPlate, Element)
    ElemBrewsterPlate();
    TYPE_NAME(qApp->translate("Elements", "Brewster plane-parallel plate"))
    DEFAULT_LABEL("G")
    CALC_MATRIX
    SUB_RANGE
    AXIS_LEN
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemMatrix, Element)
    ElemMatrix();
    TYPE_NAME(qApp->translate("Elements", "Matrix element (T&S)"))
    DEFAULT_LABEL("C")
    CALC_MATRIX
    void setMatrixT(const double& a, const double& b, const double& c, const double& d) { setMatrix(0, a, b, c, d); }
    void setMatrixS(const double& a, const double& b, const double& c, const double& d) { setMatrix(4, a, b, c, d); }
    Z::Parameter* paramAt() const { return _At; }
    Z::Parameter* paramAs() const { return _As; }
    Z::Parameter* paramBt() const { return _Bt; }
    Z::Parameter* paramBs() const { return _Bs; }
    Z::Parameter* paramCt() const { return _Ct; }
    Z::Parameter* paramCs() const { return _Cs; }
    Z::Parameter* paramDt() const { return _Dt; }
    Z::Parameter* paramDs() const { return _Ds; }
private:
    void setMatrix(int offset, const double& a, const double& b, const double& c, const double& d);
    Z::Parameter *_At, *_As, *_Bt, *_Bs, *_Ct, *_Cs, *_Dt, *_Ds;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemMatrix1, Element)
    ElemMatrix1();
    TYPE_NAME(qApp->translate("Elements", "Matrix element (T=S)"))
    DEFAULT_LABEL("C")
    CALC_MATRIX
    void setMatrix(const double& a, const double& b, const double& c, const double& d);
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemPoint, Element)
    ElemPoint();
    TYPE_NAME(qApp->translate("Elements", "Point"))
    DEFAULT_LABEL("P")
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemNormalInterface, Element)
    ElemNormalInterface();
    TYPE_NAME(qApp->translate("Elements", "Normal interface"))
    DEFAULT_LABEL("s")
    CALC_MATRIX
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemBrewsterInterface, Element)
    ElemBrewsterInterface();
    TYPE_NAME(qApp->translate("Elements", "Brewster interface"))
    DEFAULT_LABEL("s")
    CALC_MATRIX
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemTiltedInterface, Element)
    ElemTiltedInterface();
    TYPE_NAME(qApp->translate("Elements", "Tilted interface"))
    DEFAULT_LABEL("s")
    CALC_MATRIX
    double alpha() const { return _alpha->value().toSi(); }
protected:
    Z::Parameter *_alpha;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemSphericalInterface, Element)
    ElemSphericalInterface();
    TYPE_NAME(qApp->translate("Elements", "Spherical interface"))
    DEFAULT_LABEL("s")
    CALC_MATRIX
    double radius() const { return _radius->value().toSi(); }
    QList<QPair<Z::Parameter*, Z::Parameter*>> flip() override;
private:
    Z::Parameter *_radius;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemThickLens, Element)
    ElemThickLens();
    TYPE_NAME(qApp->translate("Elements", "Thick lens"))
    DEFAULT_LABEL("F")
    CALC_MATRIX
    SUB_RANGE
    double radius1() const { return _radius1->value().toSi(); }
    double radius2() const { return _radius2->value().toSi(); }
    QList<QPair<Z::Parameter*, Z::Parameter*>> flip() override;
private:
    Z::Parameter *_radius1, *_radius2;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemGrinLens, Element)
    ElemGrinLens();
    TYPE_NAME(qApp->translate("Elements", "GRIN lens"))
    DEFAULT_LABEL("GL")
    CALC_MATRIX
    SUB_RANGE
    double ior2t() const { return _ior2t->value().value(); }
    double ior2s() const { return _ior2s->value().value(); }
    Z::Parameter* paramIor() const { return _ior; }
    Z::Parameter* paramIor2t() const { return _ior2t; }
    Z::Parameter* paramIor2s() const { return _ior2s; }
protected:
    Z::Parameter *_ior2t, *_ior2s;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemGrinMedium, ElemMediumRange)
    ElemGrinMedium();
    TYPE_NAME(qApp->translate("Elements", "GRIN medium"))
    DEFAULT_LABEL("GM")
    CALC_MATRIX
    SUB_RANGE
    double ior2t() const { return _ior2t->value().value(); }
    double ior2s() const { return _ior2s->value().value(); }
    Z::Parameter* paramIor2t() const { return _ior2t; }
    Z::Parameter* paramIor2s() const { return _ior2s; }
protected:
    Z::Parameter *_ior2t, *_ior2s;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemThermoLens, Element)
    ElemThermoLens();
    TYPE_NAME(qApp->translate("Elements", "Thermal lens"))
    DEFAULT_LABEL("TL")
    CALC_MATRIX
    SUB_RANGE
    double focus() const { return _focus->value().toSi(); }
protected:
    Z::Parameter *_focus;
    double _n2;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemThermoMedium, ElemMediumRange)
    ElemThermoMedium();
    TYPE_NAME(qApp->translate("Elements", "Thermal medium"))
    DEFAULT_LABEL("TM")
    CALC_MATRIX
    SUB_RANGE
    double focus() const { return _focus->value().toSi(); }
protected:
    Z::Parameter *_focus;
    double _n2;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemAxiconMirror, Element)
    ElemAxiconMirror();
    TYPE_NAME(qApp->translate("Elements", "Axicon mirror"))
    DEFAULT_LABEL("XM")
    void calcDynamicMatrix(const DynamicElemCalcParams& p) override;
    double theta() const { return _theta->value().toSi(); }
    double alpha() const { return _alpha->value().toSi(); }
private:
    Z::Parameter *_theta, *_alpha;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemAxiconLens, Element)
    ElemAxiconLens();
    TYPE_NAME(qApp->translate("Elements", "Axicon lens"))
    DEFAULT_LABEL("XL")
    void calcDynamicMatrix(const DynamicElemCalcParams& p) override;
    double theta() const { return _theta->value().toSi(); }
    double alpha() const { return _alpha->value().toSi(); }
    double ior() const { return _ior->value().value(); }
private:
    Z::Parameter *_theta, *_alpha, *_ior;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemGaussAperture, Element)
    ElemGaussAperture();
    TYPE_NAME(qApp->translate("Elements", "Gaussian aperture"))
    DEFAULT_LABEL("GA")
    CALC_MATRIX
protected:
    Z::Parameter *_lambda, *_alpha2t, *_alpha2s;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemGaussApertureLens, Element)
    ElemGaussApertureLens();
    TYPE_NAME(qApp->translate("Elements", "Gaussian aperture with thin lens"))
    DEFAULT_LABEL("GA")
    CALC_MATRIX
    double focusT() const { return _focusT->value().toSi(); }
protected:
    Z::Parameter *_lambda, *_focusT, *_focusS, *_alpha2t, *_alpha2s;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemGaussDuctMedium, Element)
    ElemGaussDuctMedium();
    TYPE_NAME(qApp->translate("Elements", "Gaussian duct (medium)"))
    DEFAULT_LABEL("GD")
    CALC_MATRIX
    SUB_RANGE
protected:
    Z::Parameter *_lambda;
    Z::Parameter *_ior2t, *_ior2s;
    Z::Parameter *_alpha2t, *_alpha2s;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemGaussDuctSlab, Element)
    ElemGaussDuctSlab();
    TYPE_NAME(qApp->translate("Elements", "Gaussian duct (slab)"))
    DEFAULT_LABEL("GD")
    CALC_MATRIX
    SUB_RANGE
protected:
    Z::Parameter *_lambda;
    Z::Parameter *_ior2t, *_ior2s;
    Z::Parameter *_alpha2t, *_alpha2s;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

namespace Z {
namespace Utils {

inline bool isSpace(Element *elem) { return dynamic_cast<ElemEmptyRange*>(elem); }
inline ElemEmptyRange* asSpace(Element *elem) { return dynamic_cast<ElemEmptyRange*>(elem); }
inline bool isMedium(Element *elem) { return dynamic_cast<ElemMediumRange*>(elem); }
inline ElemMediumRange* asMedium(Element *elem) { return dynamic_cast<ElemMediumRange*>(elem); }

} // namespace Utils
} // namespace Z

//------------------------------------------------------------------------------

#endif // ELEMENTS_H
