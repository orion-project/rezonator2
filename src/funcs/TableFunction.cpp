#include "TableFunction.h"

#include "AbcdBeamCalculator.h"
#include "FunctionUtils.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../AppSettings.h"
#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../core/Protocol.h"
#include "../core/ElementsCatalog.h"

#include <QDebug>

const TableFunction::ResultPositionInfo& TableFunction::resultPositionInfo(TableFunction::ResultPosition pos)
{
#define I_(pos, ascii, tooltip, pixmap)\
    {TableFunction::ResultPosition::pos, {QString(ascii), QString(tooltip), QString(pixmap)}}

    static QMap<TableFunction::ResultPosition, TableFunction::ResultPositionInfo> info = {
        I_(ELEMENT,       "",          "",                          ""),
        I_(LEFT,          "->()",      "At the left of element",    ":/misc/beampos_left"),
        I_(RIGHT,         "  ()->",    "At the right of element",   ":/misc/beampos_right"),
        I_(LEFT_OUTSIDE,  "->[   ]",   "At the left edge outside",  ":/misc/beampos_left_out"),
        I_(LEFT_INSIDE,   "  [-> ]",   "At the left edge inside",   ":/misc/beampos_left_in"),
        I_(MIDDLE,        "  [ + ]",   "In the middle of element",  ":/misc/beampos_middle"),
        I_(RIGHT_INSIDE,  "  [ ->]",   "At the right edge inside",  ":/misc/beampos_right_in"),
        I_(RIGHT_OUTSIDE, "  [   ]->", "At the right edge outside", ":/misc/beampos_right_out"),
        I_(IFACE_LEFT,    "->|",       "At the left of interface",  ":/misc/beampos_iface_left"),
        I_(IFACE_RIGHT,   "  |->",     "At the right of interface", ":/misc/beampos_iface_right"),
    };
    return info[pos];
#undef I_
}

QString TableFunction::Result::str() const
{
    QString res;
    QTextStream stream(&res);
    stream << element->displayLabel() << "\t" << resultPositionInfo(position).ascii << "\t";
    for (const auto& value : values)
        stream << value.T << " x " << value.S << "\t";
    return res;
}

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
    _beamCalc.reset(new AbcdBeamCalculator(schema()->wavelenSi()));
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
            if (calculateAtPlane(elem, i))
                continue;
            else break;
        }

        if (Z::Utils::isSpace(elem))
            continue;

        auto rangeN = Z::Utils::asMedium(elem);
        if (rangeN)
        {
            // By default, don't need to calculate LEFT_INSIDE and RIGHT_INSIDE points,
            // because they will be calculated when processing the left and right
            // neighbor elements (they always must be in a properly defined schema).
            if (calcDebugResults)
                calculateAt(rangeN, UseSubrange(0), rangeN, ResultPosition::LEFT_INSIDE);

            calculateAt(rangeN, UseSubrange(rangeN->axisLengthSI() / 2.0), rangeN, ResultPosition::MIDDLE);

            if (calcDebugResults)
                calculateAt(rangeN, UseSubrange(rangeN->axisLengthSI()), rangeN, ResultPosition::RIGHT_INSIDE);
            continue;
        }

        if (!calculateAtCrystal(range, i)) break;
    }

    if (!_errorText.isEmpty())
    {
        Z_ERROR(name() + ": " + _errorText)
        _results.clear();
    }
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
            calculateAt(elem, UseSubrange::null(), elem, ResultPosition::RIGHT);
            return true;

        case TripType::SW:
        {
            // It's the left end mirror and beam params before and after are the same
            auto nextMedium = Z::Utils::asMedium(nextElement(index));
            auto overrideIor = nextMedium ? OptionalIor(nextMedium->ior()) : OptionalIor();
            calculateAt(elem, UseSubrange::null(), elem, ResultPosition::ELEMENT, overrideIor);
            return true;
        }

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
            calculateAt(prevElem, UseSubrange::null(), elem, ResultPosition::LEFT);
            calculateAt(elem, UseSubrange::null(), elem, ResultPosition::RIGHT);
            return true;

        case TripType::SW:
        {
            // It's the right end mirror and beam params before and after are the same
            auto prevMedium = Z::Utils::asMedium(prevElement(index));
            auto overrideIor = prevMedium ? OptionalIor(prevMedium->ior()) : OptionalIor();
            calculateAt(elem, UseSubrange::null(), elem, ResultPosition::ELEMENT, overrideIor);
            return true;
        }

        case TripType::RR:
            _errorText = "Too few elements in RR schema";
            return false;
        }
    }
    if (dynamic_cast<ElemMediumRange*>(prevElem) || Z::Utils::asInterface(prevElem))
    {
        _errorText = QString(
            "Invalid schema: there is %1 element at the left of %2.")
            .arg(prevElem->typeName(), elem->displayLabel());
        return false;
    }
    calculateAt(prevElem, UseSubrange::null(), elem, ResultPosition::LEFT);
    calculateAt(elem, UseSubrange::null(), elem, ResultPosition::RIGHT);
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
            _errorText = QString(
                "Invalid SW schema: Interface element %1 is at the left end of the schema. "
                "The end element must be a mirror.").arg(iface->displayLabel());
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
            _errorText = QString(
                "Invalid schema: There is a %1 element at the left of the interface %2. "
                "The only valid elements at both sides of an interface are %3, %4.").arg(
                prevElem->typeName(), iface->displayLabel(), ElemEmptyRange::_typeName_(),
                ElementsCatalog::instance().getMediumTypeNames().join(", "));
            _errorText = "Invalid schema, see Protocol for details";
            return false;
        }
        calculateAt(prevRange, UseSubrange(prevRange->axisLengthSI()), iface, ResultPosition::IFACE_LEFT);
    }

    auto nextElem = nextElement(index);
    if (!nextElem)
    {
        switch (schema()->tripType())
        {
        case TripType::SP:
            calculateAt(iface, UseSubrange::null(), iface, ResultPosition::IFACE_RIGHT);
            return true;

        case TripType::SW:
            _errorText = QString(
                "Invalid SW schema: Interface element %1 is at the right end of the schema. "
                "The end element must be a mirror.").arg(iface->displayLabel());
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
        _errorText = QString(
            "Invalid schema: There is a %1 at the right of the interface %2. "
            "The only valid elements at both sides of an interface are %3, %4.").arg(
            nextElem->typeName(), iface->displayLabel(), ElemEmptyRange::_typeName_(),
            ElementsCatalog::instance().getMediumTypeNames().join(", "));
        return false;
    }
    calculateAt(nextRange, UseSubrange(0.0), iface, ResultPosition::IFACE_RIGHT);
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
            _errorText = QString(
                "Invalid SW schema: Crystal-like element %1 is at the left end of schema, "
                "but the end element must be a mirror.").arg(range->displayLabel());
            return false;

        case TripType::RR:
            _errorText = "Too few elements in RR schema";
            return false;
        }
    }
    else
        calculateAt(prevElem, UseSubrange::null(), range, ResultPosition::LEFT_OUTSIDE);

    calculateAt(range, UseSubrange(0.0), range, ResultPosition::LEFT_INSIDE);

    auto len = range->axisLengthSI();
    calculateAt(range, UseSubrange(len / 2.0), range, ResultPosition::MIDDLE);

    calculateAt(range, UseSubrange(len), range, ResultPosition::RIGHT_INSIDE);

    calculateAt(range, UseSubrange::null(), range, ResultPosition::RIGHT_OUTSIDE);
    return true;
}

bool TableFunction::calculateAtPlane(Element* elem, int index)
{
    switch (schema()->tripType())
    {
    case TripType::SW:
        // It's the left end mirror attached to a medium
        // If it's not a mirror then the system is invalid, but we don't check here
        if (!prevElement(index))
        {
            auto nextMedium = Z::Utils::asMedium(nextElement(index));
            auto overrideIor = nextMedium ? OptionalIor(nextMedium->ior()) : OptionalIor();
            calculateAt(elem, UseSubrange::null(), elem, ResultPosition::ELEMENT, overrideIor);
            return true;
        }

        // It's the right end mirror attached to a medium
        // If it's not a mirror then the system is invalid, but we don't check here
        if (!nextElement(index))
        {
            auto prevMedium = Z::Utils::asMedium(prevElement(index));
            auto overrideIor = prevMedium ? OptionalIor(prevMedium->ior()) : OptionalIor();
            calculateAt(elem, UseSubrange::null(), elem, ResultPosition::ELEMENT, overrideIor);
            return true;
        }

        return calculateAtPlaneInMiddleOfSystem(elem, index);

    case TripType::RR:
        return calculateAtPlaneInMiddleOfSystem(elem, index);

    case TripType::SP:
        return calculateAtPlaneInMiddleOfSystem(elem, index);
    }

    _errorText = QString("%1: Invalid schema: Unknown trip-type %2.").arg(name()).arg(int(schema()->tripType()));
    return false;
}

bool TableFunction::calculateAtPlaneInMiddleOfSystem(Element* elem, int index)
{
    // In this method the elem is guaranteed to be not at the ends of a schema
    // for SW and SP systems, for RR any element is always in a 'middle'

    auto prevMedium = Z::Utils::asMedium(prevElement(index));
    auto nextMedium = Z::Utils::asMedium(nextElement(index));

    if ((prevMedium && !nextMedium) || (!prevMedium && nextMedium))
    {
        _errorText = QString(
            "Invalid schema: Position of element %1 is incorrect, "
            "only interface element types can be placed at the edge of medium: %2").arg(
            elem->displayLabel(), ElementsCatalog::instance().getInterfaceTypeNames().join(", "));
        return false;
    }

    // Position between two media doesn't guarantee that schema is valid
    // but at least it CAN BE valid (especially if both media have the same IOR)
    // if element's matrix doesn't imply air around the element.
    if (prevMedium && nextMedium)
    {
        calculateAt(prevMedium, UseSubrange(prevMedium->lengthSI()), elem, ResultPosition::LEFT);
        calculateAt(elem, UseSubrange::null(), elem, ResultPosition::RIGHT, OptionalIor(nextMedium->ior()));
        return true;
    }

    calculateAt(elem, UseSubrange::null(), elem, ResultPosition::ELEMENT);
    return true;
}


void TableFunction::calculateAt(Element* calcElem, UseSubrange subrange, Element *resultElem,
                                ResultPosition resultPos, OptionalIor overrideIor)
{
    if (subrange.set)
        Z::Utils::asRange(calcElem)->setSubRangeSI(subrange.value);

    RoundTripCalculator calc(schema(), calcElem);
    calc.calcRoundTrip(subrange.set);
    calc.multMatrix();

    const double ior = overrideIor.set ? overrideIor.value :
        (subrange.set ? Z::Utils::asRange(calcElem)->ior() : 1);

    Result res;
    res.element = resultElem;
    res.position = resultPos;
    res.values = schema()->isResonator()
            ? calculateResonator(&calc, ior)
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

QVector<Z::PointTS> TableFunction::calculateResonator(RoundTripCalculator *calc, double ior) const
{
    return {
        _beamCalc->beamRadius(calc->Mt(), calc->Ms(), ior),
        _beamCalc->frontRadius(calc->Mt(), calc->Ms(), ior),
        _beamCalc->halfAngle(calc->Mt(), calc->Ms(), ior),
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
