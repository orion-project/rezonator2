#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "../core/Schema.h"
#include "../core/Element.h"

class Calculator
{
public:
    Calculator(Schema *owner, Element *ref = NULL);

    void calcRoundTrip(bool splitRange = false);
    void multMatrix();
    void reset();
    bool isEmpty() { return _roundTrip.isEmpty(); }

    double stabilityT() const;
    double stabilityS() const;

    inline Element* reference() const { return _reference; }
    inline Schema* owner() const { return _schema; }
    inline const Z::Matrix& Mt() const { return _mt; }
    inline const Z::Matrix& Ms() const { return _ms; }
    inline const Z::MatrixArray& matrsT() const { return _matrsT; }
    inline const Z::MatrixArray& matrsS() const { return _matrsS; }
    inline const Elements& roundTrip() const { return _roundTrip; }
    QString roundTripStr() const;

protected:
    /// Array of elements in order of round-trip.
    /// Valid only after calcRoundTrip() call.
    Elements _roundTrip;

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

private:
    bool _splitRange = false;
    void calcRoundTripSW();
    void calcRoundTripRR();
    void calcRoundTripSP();
    void collectMatrices();
    void collectMatricesSP();
};

////////////////////////////////////////////////////////////////////////////////

namespace Calc {

void isStable(Schema *schema, bool& t, bool& s);

} // namespace Calc

#endif // CALCULATOR_H

