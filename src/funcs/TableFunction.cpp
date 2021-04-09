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

    #define CHECK_ERR(f) {\
        QString res = f;\
        if (!res.isEmpty()) {\
            _errorText = res;\
            break;\
        }\
    }

    auto elems = schema()->elements();
    for (int i = 0; i < elems.size(); i++)
    {
        auto elem = elems.at(i);
        if (elem->disabled()) continue;

        if (elem->hasOption(Element_ChangesWavefront))
        {
            CHECK_ERR(calculateAtElem(elem, i, AlwaysTwoSides(true)));
            continue;
        }

        auto iface = Z::Utils::asInterface(elem);
        if (iface)
        {
            CHECK_ERR(calculateAtInterface(iface, i))
            continue;
        }

        auto range = Z::Utils::asRange(elem);
        if (!range)
        {
            CHECK_ERR(calculateAtElem(elem, i, AlwaysTwoSides(false)))
            continue;
        }

        auto space = Z::Utils::asSpace(elem);
        if (space)
        {
            if (calcEmptySpaces)
            {
                calculateAt(CalcElem::RangeBeg(space), ResultElem(space, ResultPosition::LEFT_INSIDE));
                calculateAt(CalcElem::RangeMid(space), ResultElem(space, ResultPosition::MIDDLE));
                calculateAt(CalcElem::RangeEnd(space), ResultElem(space, ResultPosition::RIGHT_INSIDE));
            }
            continue;
        }

        auto rangeN = Z::Utils::asMedium(elem);
        if (rangeN)
        {
            // By default, don't need to calculate LEFT_INSIDE and RIGHT_INSIDE points,
            // because they will be calculated when processing the left and right
            // neighbor elements (they always must be in a properly defined schema).
            if (calcMediumEnds)
                calculateAt(CalcElem::RangeBeg(rangeN), ResultElem(rangeN, ResultPosition::LEFT_INSIDE));

            calculateAt(CalcElem::RangeMid(rangeN), ResultElem(rangeN, ResultPosition::MIDDLE));

            if (calcMediumEnds)
                calculateAt(CalcElem::RangeEnd(rangeN), ResultElem(rangeN, ResultPosition::RIGHT_INSIDE));
            continue;
        }

        CHECK_ERR(calculateAtCrystal(range, i))
    }

    #undef CHECK_ERR

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

QString TableFunction::calculateAtElem(Element *elem, int index, AlwaysTwoSides alwaysTwoSides)
{
    auto prevElem = prevElement(index);
    auto nextElem = nextElement(index);

    switch (schema()->tripType()) {
    case TripType::SW:
        if (prevElem && nextElem)
            return calculateInMiddle(elem, prevElem, nextElem, alwaysTwoSides);
        else if (prevElem || nextElem)
        {
            // It's the left end mirror (probably attached to a medium)
            // If it's not a mirror then the system is invalid, but we don't check here
            if (!prevElem)
            {
                auto nextMedium = Z::Utils::asMedium(nextElem);
                auto overrideIor = nextMedium ? OptionalIor(nextMedium->ior()) : OptionalIor();
                calculateAt(CalcElem(elem), ResultElem(elem, ResultPosition::ELEMENT), overrideIor);
                return "";
            }

            // It's the right end mirror (probably attached to a medium)
            // If it's not a mirror then the system is invalid, but we don't check here
            if (!nextElem)
            {
                auto prevMedium = Z::Utils::asMedium(prevElem);
                auto overrideIor = prevMedium ? OptionalIor(prevMedium->ior()) : OptionalIor();
                calculateAt(CalcElem(elem), ResultElem(elem, ResultPosition::ELEMENT), overrideIor);
                return "";
            }
        }
        return "Too few elements";

    case TripType::RR:
        // In ring systems we always are somewhere in a middle
        if (prevElem && nextElem)
            return calculateInMiddle(elem, prevElem, nextElem, alwaysTwoSides);
        return "Too few elements";

    case TripType::SP:
        // In middle of schema
        if (prevElem && nextElem)
            return calculateInMiddle(elem, prevElem, nextElem, alwaysTwoSides);
        if (!prevElem)
        {
            calculatePumpBeforeSchema(elem, ResultPosition::LEFT);
            // We can calculate beam after the first elem like if beam was in a medium
            // but the schema is invalid anyway - beamsize will show step-change
            // at the first elem when the second is medium, which is not physically correct.
            // But at least this calculation is consistent with those for SW schema.
            auto nextMedium = Z::Utils::asMedium(nextElem);
            auto overrideIor = nextMedium ? OptionalIor(nextMedium->ior()) : OptionalIor();
            calculateAt(CalcElem(elem), ResultElem(elem, ResultPosition::RIGHT), overrideIor);
            return "";
        }
        if (!nextElem)
        {
            // We can calculate beam before the last elem like if beam was in a medium
            // but the schema is invalid anyway - beamsize will show step-change
            // at the last elem if pre-last is medium, which is not physically correct.
            // But at least this calculation is consistent with those for SW schema.
            auto prevMedium = Z::Utils::asMedium(prevElem);
            auto overrideIor = prevMedium ? OptionalIor(prevMedium->ior()) : OptionalIor();
            calculateAt(CalcElem(prevElem), ResultElem(elem, ResultPosition::LEFT), overrideIor);
            // Calculate pump params after the last element
            calculateAt(CalcElem(elem), ResultElem(elem, ResultPosition::RIGHT));
            return "";
        }
    }
    return QString("Invalid schema: Unknown trip-type %1.").arg(int(schema()->tripType()));
}

QString TableFunction::calculateAtInterface(ElementInterface* iface, int index)
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
            return QString(
                "Invalid SW schema: Interface element %1 is at the left end of the schema. "
                "The end element must be a mirror.").arg(iface->displayLabel());

        case TripType::RR:
            return "Too few elements in RR schema";
        }
    }
    else
    {
        ElementRange* prevRange = Z::Utils::asSpace(prevElem);
        if (!prevRange) prevRange = Z::Utils::asMedium(prevElem);
        if (!prevRange)
            return QString(
                "Invalid schema: There is a %1 element at the left of the interface %2. "
                "The only valid elements at both sides of an interface are %3, %4.").arg(
                prevElem->typeName(), iface->displayLabel(), ElemEmptyRange::_typeName_(),
                ElementsCatalog::instance().getMediumTypeNames().join(", "));
        calculateAt(CalcElem::RangeEnd(prevRange), ResultElem(iface, ResultPosition::IFACE_LEFT));
    }

    auto nextElem = nextElement(index);
    if (!nextElem)
    {
        switch (schema()->tripType())
        {
        case TripType::SP:
            calculateAt(CalcElem(iface), ResultElem(iface, ResultPosition::IFACE_RIGHT));
            return "";

        case TripType::SW:
            return QString(
                "Invalid SW schema: Interface element %1 is at the right end of the schema. "
                "The end element must be a mirror.").arg(iface->displayLabel());

        case TripType::RR:
            return "Too few elements in RR schema";
        }
    }

    ElementRange* nextRange = Z::Utils::asSpace(nextElem);
    if (!nextRange) nextRange = Z::Utils::asMedium(nextElem);
    if (!nextRange)
        return QString(
            "Invalid schema: There is a %1 at the right of the interface %2. "
            "The only valid elements at both sides of an interface are %3, %4.").arg(
            nextElem->typeName(), iface->displayLabel(), ElemEmptyRange::_typeName_(),
            ElementsCatalog::instance().getMediumTypeNames().join(", "));
    calculateAt(CalcElem::RangeBeg(nextRange), ResultElem(iface, ResultPosition::IFACE_RIGHT));
    return "";
}

QString TableFunction::calculateAtCrystal(ElementRange* range, int index)
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
            return QString(
                "Invalid SW schema: Crystal-like element %1 is at the left end of schema, "
                "but the end element must be a mirror.").arg(range->displayLabel());

        case TripType::RR:
            return "Too few elements in RR schema";
        }
    }
    else
        calculateAt(CalcElem(prevElem), ResultElem(range, ResultPosition::LEFT_OUTSIDE));
    calculateAt(CalcElem::RangeBeg(range), ResultElem(range, ResultPosition::LEFT_INSIDE));
    calculateAt(CalcElem::RangeMid(range), ResultElem(range, ResultPosition::MIDDLE));
    calculateAt(CalcElem::RangeEnd(range), ResultElem(range, ResultPosition::RIGHT_INSIDE));
    calculateAt(CalcElem(range), ResultElem(range, ResultPosition::RIGHT_OUTSIDE));
    return "";
}

QString TableFunction::calculateInMiddle(Element* elem, Element* prevElem, Element* nextElem, AlwaysTwoSides alwaysTwoSides)
{
    // In this method the elem is guaranteed to be not at the ends of a schema
    // for SW and SP systems, for RR any element is always in a 'middle'

    auto prevMedium = Z::Utils::asMedium(prevElem);
    auto nextMedium = Z::Utils::asMedium(nextElem);

    if ((prevMedium && !nextMedium) || (!prevMedium && nextMedium))
        return QString(
            "Invalid schema: Position of element %1 is incorrect, "
            "only interface element types can be placed at the edge of medium: %2").arg(
            elem->displayLabel(), ElementsCatalog::instance().getInterfaceTypeNames().join(", "));

    // Position between two media doesn't guarantee that schema is valid
    // but at least it CAN BE valid (especially if both media have the same IOR)
    // if element's matrix doesn't imply air around the element.
    if (prevMedium && nextMedium)
    {
        calculateAt(CalcElem::RangeEnd(prevMedium), ResultElem(elem, ResultPosition::LEFT));
        calculateAt(CalcElem(elem), ResultElem(elem, ResultPosition::RIGHT), OptionalIor(nextMedium->ior()));
        return "";
    }

    if (alwaysTwoSides)
    {
        auto prevRange = Z::Utils::asRange(prevElem);
        auto calcElem = prevRange ? CalcElem::RangeEnd(prevRange) : CalcElem(prevElem);
        calculateAt(calcElem, ResultElem(elem, ResultPosition::LEFT));
        calculateAt(CalcElem(elem), ResultElem(elem, ResultPosition::RIGHT));
    }
    else
        calculateAt(CalcElem(elem), ResultElem(elem, ResultPosition::ELEMENT));
    return "";
}

void TableFunction::calculateAt(CalcElem calcElem, ResultElem resultElem, OptionalIor overrideIor)
{
    if (calcElem.range)
    {
        if (calcElem.subrangeOpt == CalcElem::SubrangeOpt::END)
            calcElem.range->setSubRangeSI(calcElem.range->axisLengthSI());
        else if (calcElem.subrangeOpt == CalcElem::SubrangeOpt::MID)
            calcElem.range->setSubRangeSI(calcElem.range->axisLengthSI()/2.0);
        else
            calcElem.range->setSubRangeSI(calcElem.subrange);
    }

    RoundTripCalculator calc(schema(), calcElem.ref());
    calc.calcRoundTrip(calcElem.range);
    calc.multMatrix();

    const double ior = overrideIor.set ? overrideIor.value : (calcElem.range ? calcElem.range->ior() : 1);

    Result res;
    res.element = resultElem.elem;
    res.position = resultElem.pos;
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
