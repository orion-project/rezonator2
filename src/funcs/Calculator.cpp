#include "Calculator.h"

Calculator::Calculator(Schema *owner, Element *ref)
{
    _schema = owner;
    _reference = ref;
    if (!_reference && _schema->count() > 0)
        _reference = _schema->elements().first();
}

void Calculator::calcRoundTrip(bool splitRange)
{
    _splitRange = splitRange;

    reset();

    if (!_reference) return;

    switch (_schema->tripType())
    {
    case TripType::SW: calcRoundTripSW(); break;
    case TripType::RR: calcRoundTripRR(); break;
    case TripType::SP: calcRoundTripSP(); break;
    }
}

void Calculator::reset()
{
    _roundTrip.clear();
    _matrsT.clear();
    _matrsS.clear();
    _mt.unity();
    _ms.unity();
}

void Calculator::calcRoundTripSW()
{
    int ref = _schema->indexOf(_reference);

    // from reference element to first
    int i = ref;
    while (i > 0)
        _roundTrip.push_back(_schema->element(i--));

    // from first to last
    int c = _schema->count();
    if (ref == c-1) c--; // if last is reference then skip it

    while (i < c)
        _roundTrip.push_back(_schema->element(i++));

    // from last to reference
    i -= 2;
    while (i > ref)
        _roundTrip.push_back(_schema->element(i--));

    collectMatrices();
}

void Calculator::calcRoundTripRR()
{
    int ref = _schema->indexOf(_reference);

    // from reference element to first
    int i = ref;
    while (i >= 0)
        _roundTrip.push_back(_schema->element(i--));

    // from last to reference
    i = _schema->count()-1;
    while (i > ref)
        _roundTrip.push_back(_schema->element(i--));

    collectMatrices();
}

void Calculator::calcRoundTripSP()
{
    int i = _schema->indexOf(_reference);

    // from reference element to first
    while (i >= 0)
        _roundTrip.push_back(_schema->element(i--));

    collectMatricesSP();
}

void Calculator::collectMatrices()
{
    int i = 0;
    int c = _roundTrip.size();

    ElementRange *range = nullptr;
    if (_splitRange)
        range = Z::Utils::asRange(_reference);
    // part of range from current point to next element
    if (range)
    {
        _matrsT.push_back(range->pMt1());
        _matrsS.push_back(range->pMs1());
        i++;
    }
    // all other element as whole
    while (i < c)
    {
        auto elem = _roundTrip[i];
        _matrsT.push_back(elem->pMt());
        _matrsS.push_back(elem->pMs());
        i++;
    }
    // remaining part of range under investigation
    if (range)
    {
        _matrsT.push_back(range->pMt2());
        _matrsS.push_back(range->pMs2());
    }
}

void Calculator::collectMatricesSP()
{
    int i = 0;
    int c = _roundTrip.size();
    // part of range from current point to next element
    if (_splitRange && i < c)
    {
        auto range = Z::Utils::asRange(_roundTrip.at(i));
        if (range)
        {
            _matrsT.push_back(range->pMt1());
            _matrsS.push_back(range->pMs1());
            i++;
        }
    }
    // all other element as whole
    while (i < c)
    {
        auto elem = _roundTrip.at(i);
        _matrsT.push_back(elem->pMt());
        _matrsS.push_back(elem->pMs());
        i++;
    }
}

void Calculator::multMatrix()
{
    _mt.unity();
    _ms.unity();
    for (int i = 0; i < _matrsT.size(); i++)
    {
        _mt *= _matrsT[i];
        _ms *= _matrsS[i];
    }
}

double Calculator::stabilityT() const
{
    // TODO: process another stability calculation ways
    return (_mt.A + _mt.D) * 0.5;
}

double Calculator::stabilityS() const
{
    // TODO: process another stability calculation ways
    return (_ms.A + _ms.D) * 0.5;
}

QString Calculator::roundTripStr() const
{
    QString res;
    for (auto elem : _roundTrip)
        res += QString("%1 ").arg(elem? elem->displayLabel(): "null");
    return res;
}

////////////////////////////////////////////////////////////////////////////////
//                               namespace Calc
////////////////////////////////////////////////////////////////////////////////

namespace Calc {

void isStable(Schema *schema, bool& t, bool& s)
{
    Calculator c(schema);
    c.calcRoundTrip();
    c.multMatrix();
    double pt = c.stabilityT();
    double ps = c.stabilityS();
    t = pt > -1 && pt < 1;
    s = ps > -1 && ps < 1;
}

} // namespace Calc
