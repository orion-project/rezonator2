#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "../core/Schema.h"
#include "../core/Element.h"

class RoundTripCalculator
{
public:
    RoundTripCalculator(Schema *owner, Element *ref = NULL);

    void calcRoundTrip(bool splitRange = false);
    void multMatrix();
    void reset();
    bool isEmpty() { return _roundTrip.isEmpty(); }

    Z::PointTS stability() const;
    Z::PairTS<bool> isStable() const;
    Z::Enums::StabilityCalcMode stabilityCalcMode() const { return _stabilityCalcMode; }
    void setStabilityCalcMode(Z::Enums::StabilityCalcMode mode) { _stabilityCalcMode = mode; }

    inline Element* reference() const { return _reference; }
    inline Schema* owner() const { return _schema; }
    inline const Z::Matrix& Mt() const { return _mt; }
    inline const Z::Matrix& Ms() const { return _ms; }
    inline const Z::MatrixArray& matrsT() const { return _matrsT; }
    inline const Z::MatrixArray& matrsS() const { return _matrsS; }
    Elements roundTrip() const;
    QString roundTripStr() const;

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

    Z::Enums::StabilityCalcMode _stabilityCalcMode = Z::Enums::StabilityCalcMode::Normal;

private:
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

    /// Array of elements in order of round-trip.
    /// Valid only after calcRoundTrip() call.
    QVector<RoundTripElemInfo> _roundTrip;

    bool _splitRange = false;
    void calcRoundTripSW();
    void calcRoundTripRR();
    void calcRoundTripSP();
    void collectMatrices();
    void collectMatricesSP();

    double calcStability(double half_of_A_plus_D) const;
    bool isStable(double half_of_A_plus_D) const;
};

//------------------------------------------------------------------------------

namespace Calc {

Z::PairTS<bool> isStable(Schema *schema);

} // namespace Calc

#endif // CALCULATOR_H

