#include "TableFunction.h"

#include "AbcdBeamCalculator.h"
#include "FunctionUtils.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../AppSettings.h"
#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../core/Protocol.h"

#include <QDebug>

TableFunction::TableFunction(Schema *schema) : FunctionBase(schema)
{
}

bool TableFunction::prepareSinglePass()
{
    QString res = FunctionUtils::preparePumpCalculator(schema(), nullptr, _pumpCalc);
    if (!res.isEmpty())
    {
        _errorText = res;
        return false;
    }

    FunctionUtils::prepareDynamicElements(schema(), nullptr, _pumpCalc);
    return true;
}

bool TableFunction::prepareResonator()
{
    if (!_beamCalc) _beamCalc.reset(new AbcdBeamCalculator);
    return true;
}

void TableFunction::calculate()
{
   _results.clear();
   _errorText.clear();

   bool isResonator = _schema->isResonator();
   bool isPrepared = isResonator
           ? prepareResonator()
           : prepareSinglePass();
   if (!isPrepared) return;

    auto elems = schema()->elements();
    for (int i = 0; i < elems.size(); i++)
    {
        auto elem = elems.at(i);
        if (elem->disabled()) continue;

        if (elem->hasOption(Element_ChangesWavefront))
        {
            if (calculateAtMirrorOrLens(elem, i))
                continue;
            else break;
        }

        auto iface = Z::Utils::asInterface(elem);
        if (iface)
        {
            if (calculateAtInterface(iface, i))
                continue;
            else break;
        }

        auto range = Z::Utils::asRange(elem);
        if (!range)
        {
            calculateAt(elem, false, elem, ResultPosition::ELEMENT);
            continue;
        }

        if (dynamic_cast<ElemEmptyRange*>(elem))
            continue;

        auto rangeN = dynamic_cast<ElemMediumRange*>(elem);
        if (rangeN)
        {
            rangeN->setSubRangeSI(rangeN->axisLengthSI() / 2.0);
            calculateAt(rangeN, true, rangeN, ResultPosition::MIDDLE);
            continue;
        }

        if (!calculateAtCrystal(range, i)) break;
    }

    if (!_errorText.isEmpty())
        _results.clear();
}

Element* TableFunction::prevElement(int index)
{
    if (index > 0)
        return schema()->element(index - 1);
    switch (schema()->tripType())
    {
    case TripType::SP:
    case TripType::SW:
        return nullptr;

    case TripType::RR:
        int prevIndex = schema()->count() - 1;
        if (prevIndex == index) return nullptr;
        return schema()->element(prevIndex);
    }
    return nullptr;
}

Element* TableFunction::nextElement(int index)
{
    if (index < schema()->count() - 1)
        return schema()->element(index + 1);
    switch (schema()->tripType())
    {
    case TripType::SP:
    case TripType::SW:
        return nullptr;

    case TripType::RR:
        if (index == 0) return nullptr;
        return schema()->element(0);
    }
    return nullptr;
}

bool TableFunction::calculateAtMirrorOrLens(Element *elem, int index)
{
    auto prevElem = prevElement(index);
    if (!prevElem)
    {
        switch (schema()->tripType())
        {
        case TripType::SP:
            calculatePumpBeforeSchema(elem, ResultPosition::LEFT);
            calculateAt(elem, false, elem, ResultPosition::RIGHT);
            return true;

        case TripType::SW:
            // It's the end mirror and beam params before and after are the same
            calculateAt(elem, false, elem, ResultPosition::ELEMENT);
            return true;

        case TripType::RR:
            _errorText = "Too few elements in RR schema";
            return false;
        }
    }
    auto nextElem = nextElement(index);
    if (!nextElem)
    {
        switch (schema()->tripType())
        {
        case TripType::SP:
            // Calculate pump params after the last element
            calculateAt(prevElem, false, elem, ResultPosition::LEFT);
            calculateAt(elem, false, elem, ResultPosition::RIGHT);
            return true;

        case TripType::SW:
            // It's the end mirror and beam params before and after are the same
            calculateAt(elem, false, elem, ResultPosition::ELEMENT);
            return true;

        case TripType::RR:
            _errorText = "Too few elements in RR schema";
            return false;
        }
    }
    if (dynamic_cast<ElemMediumRange*>(prevElem) || Z::Utils::asInterface(prevElem))
    {
        Z_ERROR(QString("%1: Invalid schema: there is '%2' element at the left of %3.")
                        .arg(name(), prevElem->typeName(), elem->displayLabel()))
        _errorText = "Invalid SW schema, see Protocol for details";
        return false;
    }
    calculateAt(prevElem, false, elem, ResultPosition::LEFT);
    calculateAt(elem, false, elem, ResultPosition::RIGHT);
    return true;
}

bool TableFunction::calculateAtInterface(ElementInterface* iface, int index)
{
    auto prevElem = prevElement(index);
    if (!prevElem)
    {
        switch (schema()->tripType())
        {
        case TripType::SP:
            calculatePumpBeforeSchema(iface, ResultPosition::IFACE_LEFT);
            break; // Don't return!

        case TripType::SW:
            Z_ERROR(QString("%1: Invalid SW schema: The interface element %2 is at the left end of the schema. "
                            "The end element must be a mirror.").arg(name(), iface->displayLabel()))
            _errorText = "Invalid SW schema, see Protocol for details";
            return false;

        case TripType::RR:
            _errorText = "Too few elements in RR schema";
            return false;
        }
    }
    else
    {
        ElementRange *prevRange = dynamic_cast<ElemEmptyRange*>(prevElem);
        if (!prevRange) prevRange = dynamic_cast<ElemMediumRange*>(prevElem);
        if (!prevRange)
        {
            Z_ERROR(QString("%1: Invalid schema: There is a '%2' element at the left of the interface %3. "
                            "Only valid elements at both sides of an interface are '%4' or '%5'."
                            ).arg(name(), prevElem->typeName(), iface->displayLabel(),
                            ElemEmptyRange::_typeName_(), ElemMediumRange::_typeName_()))
            _errorText = "Invalid schema, see Protocol for details";
            return false;
        }
        prevRange->setSubRangeSI(prevRange->axisLengthSI());
        calculateAt(prevRange, true, iface, ResultPosition::IFACE_LEFT);
    }

    auto nextElem = nextElement(index);
    if (!nextElem)
    {
        switch (schema()->tripType())
        {
        case TripType::SP:
            calculateAt(iface, false, iface, ResultPosition::IFACE_RIGHT);
            return true;

        case TripType::SW:
            Z_ERROR(QString("%1: Invalid SW schema: The interface element %2 is at the right end of the schema. "
                            "The end element must be a mirror.").arg(name(), iface->displayLabel()))
            _errorText = "Invalid SW schema, see Protocol for details";
            return false;

        case TripType::RR:
            _errorText = "Too few elements in RR schema";
            return false;
        }
    }

    ElementRange *nextRange = dynamic_cast<ElemEmptyRange*>(nextElem);
    if (!nextRange) nextRange = dynamic_cast<ElemMediumRange*>(nextElem);
    if (!nextRange)
    {
        Z_ERROR(QString("%1: Invalid schema: There is a '%2' at the right of the interface %3. "
                        "Only valid elements at both sides of an interface are '%4' or '%5'."
                        ).arg(name(), nextElem->typeName(), iface->displayLabel(),
                        ElemEmptyRange::_typeName_(), ElemMediumRange::_typeName_()))
        _errorText = "Invalid schema, see Protocol for details";
        return false;
    }
    nextRange->setSubRangeSI(0);
    calculateAt(nextRange, true, iface, ResultPosition::IFACE_RIGHT);
    return true;
}

bool TableFunction::calculateAtCrystal(ElementRange* range, int index)
{
    auto prevElem = prevElement(index);
    if (!prevElem)
    {
        switch (schema()->tripType())
        {
        case TripType::SP:
            calculatePumpBeforeSchema(range, ResultPosition::LEFT_OUTSIDE);
            break; // Don't return!

        case TripType::SW:
            Z_ERROR(QString(
                "%1: Invalid SW schema: Crystal-like element %2 is at the left end of schema, "
                "but the end element must be a mirror.").arg(name(), range->displayLabel()))
            _errorText = "Invalid SW schema, see Protocol for details";
            return false;

        case TripType::RR:
            _errorText = "Too few elements in RR schema";
            return false;
        }
    }
    else
        calculateAt(prevElem, false, range, ResultPosition::LEFT_OUTSIDE);

    range->setSubRangeSI(0);
    calculateAt(range, true, range, ResultPosition::LEFT_INSIDE);

    auto len = range->axisLengthSI();
    range->setSubRangeSI(len / 2.0);
    calculateAt(range, true, range, ResultPosition::MIDDLE);

    range->setSubRangeSI(len);
    calculateAt(range, true, range, ResultPosition::RIGHT_INSIDE);

    calculateAt(range, false, range, ResultPosition::RIGHT_OUTSIDE);
    return true;
}

void TableFunction::calculateAt(Element* calcElem, bool calcSubrange, Element *resultElem, ResultPosition resultPos)
{
    RoundTripCalculator calc(schema(), calcElem);
    calc.calcRoundTrip(calcSubrange);
    calc.multMatrix();

    double ior = calcSubrange ? Z::Utils::asRange(calcElem)->ior() : 1;
    double wavelenSI = schema()->wavelength().value().toSi();

    Result res;
    res.element = resultElem;
    res.position = resultPos;
    res.values = schema()->isResonator()
            ? calculateResonator(&calc, wavelenSI, ior)
            : calculateSinglePass(&calc, ior);
    _results << res;
}

void TableFunction::calculatePumpBeforeSchema(Element* elem, ResultPosition resultPos)
{
    Z::Matrix unity;
    BeamResult beamT = _pumpCalc.T->calc(unity, 1);
    BeamResult beamS = _pumpCalc.S->calc(unity, 1);

    Result res;
    res.element = elem;
    res.position = resultPos;
    res.values = {
        { beamT.beamRadius, beamS.beamRadius },
        { beamT.frontRadius, beamS.frontRadius },
        { beamT.halfAngle, beamS.halfAngle },
    };
    _results << res;
}

QVector<Z::PointTS> TableFunction::calculateSinglePass(RoundTripCalculator* calc, double ior) const
{
    BeamResult beamT = _pumpCalc.T->calc(calc->Mt(), ior);
    BeamResult beamS = _pumpCalc.S->calc(calc->Ms(), ior);
    return {
        { beamT.beamRadius, beamS.beamRadius },
        { beamT.frontRadius, beamS.frontRadius },
        { beamT.halfAngle, beamS.halfAngle },
    };
}

QVector<Z::PointTS> TableFunction::calculateResonator(RoundTripCalculator *calc, double wavelenSI, double ior) const
{
    _beamCalc->setWavelenSI(wavelenSI / ior);
    return {
        _beamCalc->beamRadius(calc->Mt(), calc->Ms()),
        _beamCalc->frontRadius(calc->Mt(), calc->Ms()),
        _beamCalc->halfAngle(calc->Mt(), calc->Ms()) ,
    };
}

QVector<TableFunction::ColumnDef> TableFunction::columns() const
{
    ColumnDef beamRadius;
    beamRadius.titleT = "Wt";
    beamRadius.titleS = "Ws";
    beamRadius.unit = AppSettings::instance().defaultUnitBeamRadius;

    ColumnDef frontRadius;
    frontRadius.titleT = "Rt";
    frontRadius.titleS = "Rs";
    frontRadius.unit = AppSettings::instance().defaultUnitFrontRadius;

    ColumnDef halfAngle;
    halfAngle.titleT = "Vt";
    halfAngle.titleS = "Vs";
    halfAngle.unit = AppSettings::instance().defaultUnitAngle;

    return {beamRadius, frontRadius, halfAngle};
}
