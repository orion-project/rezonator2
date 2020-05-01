#include "RoundTripCalculator.h"

#include "../core/Schema.h"
#include "../core/Element.h"

RoundTripCalculator::RoundTripCalculator(Schema *owner, Element *ref)
{
    _schema = owner;
    _reference = ref;
    if (!_reference && _schema->count() > 0)
        _reference = _schema->elements().first();
}

void RoundTripCalculator::calcRoundTrip(bool splitRange)
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

void RoundTripCalculator::reset()
{
    _matrixOwners.clear();
    _roundTrip.clear();
    _matrsT.clear();
    _matrsS.clear();
    _mt.unity();
    _ms.unity();
}

void RoundTripCalculator::calcRoundTripSW()
{
    const int ref = _schema->indexOf(_reference);

    // from the reference element to the first one
    int i = ref;
    while (i > 0)
        _roundTrip.push_back(_schema->element(i--));

    // from the first element to the last one
    int c = _schema->count();
    // if the last is the reference then skip it because it is already added
    if (ref == c-1) c--;

    while (i < c) {
        // end elements of SW-schema should not be treated
        // as "second-passed" because of they are passed ony once
        bool secondPass = (i != 0) && (i != _schema->count()-1);

        _roundTrip.push_back({ _schema->element(i++), secondPass });
    }

    // from the last element to the reference one
    i -= 2;
    while (i > ref)
        _roundTrip.push_back(_schema->element(i--));

    collectMatrices();
}

void RoundTripCalculator::calcRoundTripRR()
{
    int ref = _schema->indexOf(_reference);

    // from the reference element to the first one
    int i = ref;
    while (i >= 0)
        _roundTrip.push_back(_schema->element(i--));

    // from the last element to the reference one
    i = _schema->count()-1;
    while (i > ref)
        _roundTrip.push_back(_schema->element(i--));

    collectMatrices();
}

void RoundTripCalculator::calcRoundTripSP()
{
    int i = _schema->indexOf(_reference);

    // from the reference element to the first one
    while (i >= 0)
        _roundTrip.push_back(_schema->element(i--));

    collectMatricesSP();
}

void RoundTripCalculator::collectMatrices()
{
    int i = 0;
    int c = _roundTrip.size();

    ElementRange *range = nullptr;
    if (_splitRange)
        range = Z::Utils::asRange(_reference);
    // part of the range from current point to the next element
    if (range)
    {
        _matrixOwners << range;
        _matrsT << range->pMt1();
        _matrsS << range->pMs1();
        i++;
    }
    // all other elements as a whole
    while (i < c)
    {
        const auto& item = _roundTrip.at(i);
        _matrixOwners << item.element;
        if (item.secondPass)
        {
            _matrsT << item.element->pMt_inv();
            _matrsS << item.element->pMs_inv();
        }
        else
        {
            _matrsT << item.element->pMt();
            _matrsS << item.element->pMs();
        }
        i++;
    }
    // remaining part of the range under investigation
    if (range)
    {
        _matrixOwners << range;
        _matrsT << range->pMt2();
        _matrsS << range->pMs2();
    }
}

void RoundTripCalculator::collectMatricesSP()
{
    int i = 0;
    int c = _roundTrip.size();
    // part of range from current point to next element
    if (_splitRange && i < c)
    {
        auto range = Z::Utils::asRange(_roundTrip.at(i).element);
        if (range)
        {
            _matrixOwners << range;
            _matrsT << range->pMt1();
            _matrsS << range->pMs1();
            i++;
        }
    }
    // all other element as whole
    while (i < c)
    {
        const auto& item = _roundTrip.at(i);
        _matrixOwners << item.element;
        auto dynamicElem = dynamic_cast<ElementDynamic*>(item.element);
        if (dynamicElem)
        {
            _matrsT << dynamicElem->pMt_dyn();
            _matrsS << dynamicElem->pMs_dyn();
        }
        else
        {
            _matrsT << item.element->pMt();
            _matrsS << item.element->pMs();
        }
        i++;
    }
}

void RoundTripCalculator::multMatrix()
{
    _mt.unity();
    _ms.unity();
    for (int i = 0; i < _matrsT.size(); i++)
    {
        _mt *= _matrsT[i];
        _ms *= _matrsS[i];
    }
}

Z::PointTS RoundTripCalculator::stability() const
{
    return { calcStability(_mt), calcStability(_ms) };
}

Z::PairTS<bool> RoundTripCalculator::isStable() const
{
    return { isStable(_mt), isStable(_ms) };
}

bool RoundTripCalculator::isStable(const Z::Matrix& m) const
{
    // TODO:COMPLEX: what about imaginary part?
    auto half_of_A_plus_D = ((m.A + m.D) * 0.5).real();
    return (half_of_A_plus_D > -1) && (half_of_A_plus_D < 1);
}

double RoundTripCalculator::calcStability(const Z::Matrix& m) const
{
    // TODO:COMPLEX: what about imaginary part?
    auto half_of_A_plus_D = (m.A + m.D) * 0.5;
    switch (_stabilityCalcMode)
    {
    case Z::Enums::StabilityCalcMode::Normal:
        return half_of_A_plus_D.real();

    case Z::Enums::StabilityCalcMode::Squared:
        return (Z::Complex(1, 0) - half_of_A_plus_D * half_of_A_plus_D).real();
    }
    return 0;
}

QList<Element*> RoundTripCalculator::roundTrip() const
{
    Elements elements;
    for (auto& item : _roundTrip)
        elements.append(item.element);
    return elements;
}

QString RoundTripCalculator::roundTripStr() const
{
    QStringList res;
    for (auto& item : _roundTrip)
        res << item.element->displayLabel();
    return res.join(' ');
}

//------------------------------------------------------------------------------
//                               namespace Calc
//------------------------------------------------------------------------------

namespace Calc {

Z::PairTS<bool> isStable(Schema *schema)
{
    RoundTripCalculator c(schema);
    c.calcRoundTrip();
    c.multMatrix();
    return c.isStable();
}

} // namespace Calc
