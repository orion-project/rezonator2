#include "RoundTripCalculator.h"

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
        _matrsT.push_back(range->pMt1());
        _matrsS.push_back(range->pMs1());
        i++;
    }
    // all other elements as a whole
    while (i < c)
    {
        const auto& item = _roundTrip.at(i);
        if (item.secondPass && item.element->hasOption(Element_Asymmetrical))
        {
            _matrsT.push_back(item.element->pMt_inv());
            _matrsS.push_back(item.element->pMs_inv());
        }
        else
        {
            _matrsT.push_back(item.element->pMt());
            _matrsS.push_back(item.element->pMs());
        }
        i++;
    }
    // remaining part of the range under investigation
    if (range)
    {
        _matrsT.push_back(range->pMt2());
        _matrsS.push_back(range->pMs2());
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
            _matrsT.push_back(range->pMt1());
            _matrsS.push_back(range->pMs1());
            i++;
        }
    }
    // all other element as whole
    while (i < c)
    {
        const auto& item = _roundTrip.at(i);
        _matrsT.push_back(item.element->pMt());
        _matrsS.push_back(item.element->pMs());
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
    return {
        calcStability((_mt.A + _mt.D) * 0.5),
        calcStability((_ms.A + _ms.D) * 0.5)
    };
}

Z::PairTS<bool> RoundTripCalculator::isStable() const
{
    return {
        isStable((_mt.A + _mt.D) * 0.5),
        isStable((_ms.A + _ms.D) * 0.5)
    };
}

bool RoundTripCalculator::isStable(double half_of_A_plus_D) const
{
    return (half_of_A_plus_D > -1) && (half_of_A_plus_D < 1);
}

double RoundTripCalculator::calcStability(double half_of_A_plus_D) const
{
    switch (_stabilityCalcMode)
    {
    case Z::Enums::StabilityCalcMode::Normal:
        return half_of_A_plus_D;

    case Z::Enums::StabilityCalcMode::Squared:
        return 1 - half_of_A_plus_D * half_of_A_plus_D;
    }
    return 0;
}

Elements RoundTripCalculator::roundTrip() const
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
