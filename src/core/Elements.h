#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "Element.h"

#include <QApplication>

DECLARE_ELEMENT(ElemEmptyRange, ElementRange)
    TYPE_NAME(qApp->translate("Elements", "Empty space"))
    DEFAULT_LABEL("L")
    CALC_MATRIX
    SUB_RANGE
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemMediumRange, ElementRange)
    ElemMediumRange();
    TYPE_NAME(qApp->translate("Elements", "Space filled with medium"))
    DEFAULT_LABEL("L")
    CALC_MATRIX
    SUB_RANGE
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemPlate, ElementRange)
    ElemPlate();
    TYPE_NAME(qApp->translate("Elements", "Plate of matter"))
    DEFAULT_LABEL("Cr")
    CALC_MATRIX
    SUB_RANGE
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemFlatMirror, Element)
    TYPE_NAME(qApp->translate("Elements", "Flat mirror"))
    DEFAULT_LABEL("M")
    PARAMS_EDITOR(None)
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
    Z::Parameter *_radius, *_alpha;
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

DECLARE_ELEMENT(ElemTiltedCrystal, ElementRange)
    ElemTiltedCrystal();
    TYPE_NAME(qApp->translate("Elements", "Tilted plane-parallel crystal"))
    DEFAULT_LABEL("Cr")
    CALC_MATRIX
    SUB_RANGE
    double alpha() const { return _alpha->value().toSi(); }
protected:
    Z::Parameter *_alpha;
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemTiltedPlate, ElemTiltedCrystal)
    TYPE_NAME(qApp->translate("Elements", "Tilted plane-parallel plate"))
    DEFAULT_LABEL("Cr")
    CALC_MATRIX
    SUB_RANGE
    AXIS_LEN
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemBrewsterCrystal, ElementRange)
    ElemBrewsterCrystal();
    TYPE_NAME(qApp->translate("Elements", "Brewster plane-parallel crystal"))
    DEFAULT_LABEL("Cr")
    CALC_MATRIX
    SUB_RANGE
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemBrewsterPlate, ElementRange)
    ElemBrewsterPlate();
    TYPE_NAME(qApp->translate("Elements", "Brewster plane-parallel plate"))
    DEFAULT_LABEL("Cr")
    CALC_MATRIX
    SUB_RANGE
    AXIS_LEN
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemMatrix, Element)
    ElemMatrix();
    TYPE_NAME(qApp->translate("Elements", "Custom element-matrix"))
    DEFAULT_LABEL("El")
    PARAMS_EDITOR(ABCD)
    CALC_MATRIX
    void makeParam(const QString& name, const double& value);
    void setMatrixT(const double& a, const double& b, const double& c, const double& d) { setMatrix(0, a, b, c, d); }
    void setMatrixS(const double& a, const double& b, const double& c, const double& d) { setMatrix(4, a, b, c, d); }
private:
    void setMatrix(int offset, const double& a, const double& b, const double& c, const double& d);
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemPoint, Element)
    TYPE_NAME(qApp->translate("Elements", "Point"))
    DEFAULT_LABEL("El")
    PARAMS_EDITOR(None)
DECLARE_ELEMENT_END

//------------------------------------------------------------------------------

DECLARE_ELEMENT(ElemNormalInterface, ElementInterface)
    TYPE_NAME(qApp->translate("Elements", "Normal interface"))
    DEFAULT_LABEL("I")
    CALC_MATRIX
DECLARE_ELEMENT_END

#endif // ELEMENTS_H
