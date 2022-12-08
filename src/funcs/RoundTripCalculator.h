#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "../core/CommonTypes.h"
#include "../core/Math.h"
#include "../core/Values.h"

#include <QString>

class Schema;
class Element;

class RoundTripCalculator
{
public:
    struct MatrixInfo {
        Element* owner;
        enum { WHOLE, BACK_PASS, LEFT_HALF, RIGHT_HALF } kind;
    };

public:
    RoundTripCalculator(Schema *owner, Element *ref);

    void calcRoundTrip(bool splitRange = false);
    void multMatrix();
    void reset();
    bool isEmpty() const { return _roundTrip.isEmpty(); }
    QString error() const { return _error; }

    Z::PointTS stability() const;
    Z::PairTS<bool> isStable() const;
    Z::Enums::StabilityCalcMode stabilityCalcMode() const { return _stabilityCalcMode; }
    void setStabilityCalcMode(Z::Enums::StabilityCalcMode mode) { _stabilityCalcMode = mode; }

    inline Element* reference() const { return _reference; }
    inline Schema* owner() const { return _schema; }
    inline const Z::Matrix& Mt() const { return _mt; }
    inline const Z::Matrix& Ms() const { return _ms; }
    inline const Z::Matrix* pMt() const { return &_mt; }
    inline const Z::Matrix* pMs() const { return &_ms; }
    inline const Z::MatrixArray& matrsT() const { return _matrsT; }
    inline const Z::MatrixArray& matrsS() const { return _matrsS; }

    QList<Element*> roundTrip() const;
    QString roundTripStr() const;

    // Returns info array of the same length as matrsT/matrsS
    QList<MatrixInfo> matrixInfo() const { return _matrixInfo; }

    struct RoundTripElemInfo
    {
        Element* element;

        /// In SW schemas beam passes each element twice
        /// and the second time passes it in opposite direction than the first time.
        /// It is importand to know which pass it is,
        /// in order to choose a proper matrix of interface element.
        bool secondPass = false;

        RoundTripElemInfo() = default;
        RoundTripElemInfo(Element* e): element(e) {}
        RoundTripElemInfo(Element* e, bool second): element(e), secondPass(second) {}
    };

    const QVector<RoundTripElemInfo>& rawRoundTrip() const { return _roundTrip; }

    bool splitRange() const { return _splitRange; }

protected:
    /// Array of T-matrices for production (round-trip).
    /// Valid only after calcRoundTrip() call.
    Z::MatrixArray _matrsT;

    /// Array of S-matrices for production (round-trip).
    /// Valid only after calcRoundTrip() call.
    Z::MatrixArray _matrsS;

    /// Round-trip matrices. Valid only after multMatrix() call.
    Z::Matrix _mt, _ms;

    Schema* _schema;

    /// Reference element for round-trip calculation.
    Element* _reference;

    QList<MatrixInfo> _matrixInfo;

    /// A reason why round-trip is empty.
    /// Valid only after calcRoundTrip() call.
    QString _error;

    Z::Enums::StabilityCalcMode _stabilityCalcMode = Z::Enums::StabilityCalcMode::Normal;

private:
    /// Array of elements in order of round-trip.
    /// Valid only after calcRoundTrip() call.
    QVector<RoundTripElemInfo> _roundTrip;

    bool _splitRange = false;
    void calcRoundTripSW(const QList<Element*>& elems);
    void calcRoundTripRR(const QList<Element*>& elems);
    void calcRoundTripSP(const QList<Element*>& elems);
    void collectMatrices();
    void collectMatricesSP();

    double calcStability(const Z::Matrix &m) const;
    bool isStable(const Z::Matrix &m) const;
};

//------------------------------------------------------------------------------

namespace Calc {

Z::PairTS<bool> isStable(Schema *schema);

} // namespace Calc

#endif // CALCULATOR_H

