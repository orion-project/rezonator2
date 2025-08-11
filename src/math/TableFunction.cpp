#include "TableFunction.h"

#include "AbcdBeamCalculator.h"
#include "FunctionUtils.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../app/AppSettings.h"
#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../core/ElementsCatalog.h"

#include <QDebug>

const TableFunction::ResultPositionInfo& TableFunction::resultPositionInfo(TableFunction::ResultPosition pos)
{
#define I_(pos, ascii, tooltip, icon_path)\
    {TableFunction::ResultPosition::pos, {QString(ascii), QString(tooltip), QString(icon_path)}}

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

void TableFunction::setParams(const Params& params)
{
    _params = params;
}

bool TableFunction::prepareSinglePass()
{
    auto pump = _schema->activePump();
    if (!pump)
    {
        setError(qApp->translate("Calc error",
                                 "There is no active pump in the schema. "
                                 "Use 'Pumps' window to create a new pump or activate one of the existing ones."));
        return false;
    }

    _beamCalc.reset();
    _pumpCalc.reset(new PumpCalculator(pump, schema()->wavelenSi()));
    FunctionUtils::prepareDynamicElements(schema(), nullptr, _pumpCalc.get());
    return true;
}

bool TableFunction::prepareResonator()
{
    _beamCalc.reset(new AbcdBeamCalculator(schema()->wavelenSi()));
    _pumpCalc.reset();
    return true;
}

void TableFunction::calculate()
{
    _results.clear();
    _errorText.clear();
    
    if (!prepare())
        return;
    
    bool isPrepared = _schema->isResonator()
           ? prepareResonator()
           : prepareSinglePass();
    if (!isPrepared) return;

    #define CHECK_ERR(f) {\
        QString res = f;\
        if (!res.isEmpty()) {\
            setError(res);\
            break;\
        }\
    }

    _activeElements = schema()->activeElements();

    for (int i = 0; i < _activeElements.size(); i++)
    {
        if (!ok())
            break;

        auto elem = _activeElements.at(i);
        if (elem->disabled()) continue;

        if (elem->hasOption(Element_ChangesWavefront))
        {
            CHECK_ERR(calculateAtElem(elem, i, IsTwoSides(true)));
            continue;
        }
        
        if (elem->hasOption(Element_Unity)) {
            CHECK_ERR(calculateAtElem(elem, i, IsTwoSides(false)));
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
            CHECK_ERR(calculateAtElem(elem, i, IsTwoSides(false)))
            continue;
        }

        auto space = Z::Utils::asSpace(elem);
        if (space)
        {
            if (_params.calcEmptySpaces)
            {
                calculateAt(CalcElem::RangeBeg(space), ResultElem(space, ResultPosition::LEFT_INSIDE));
                if (_params.calcSpaceMids)
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
            if (_params.calcMediumEnds)
                calculateAt(CalcElem::RangeBeg(rangeN), ResultElem(rangeN, ResultPosition::LEFT_INSIDE));

            if (_params.calcSpaceMids)
                calculateAt(CalcElem::RangeMid(rangeN), ResultElem(rangeN, ResultPosition::MIDDLE));

            if (_params.calcMediumEnds)
                calculateAt(CalcElem::RangeEnd(rangeN), ResultElem(rangeN, ResultPosition::RIGHT_INSIDE));
            continue;
        }

        CHECK_ERR(calculateAtCrystal(range, i))
    }

    #undef CHECK_ERR

    if (!ok())
        _results.clear();
        
    unprepare();
}

Element* TableFunction::prevElement(int index)
{
    if (index > 0)
        return _activeElements.at(index - 1);
    switch (schema()->tripType())
    {
    case TripType::SP:
    case TripType::SW:
        return nullptr;

    case TripType::RR:
        int prevIndex = _activeElements.size() - 1;
        if (prevIndex == index) return nullptr;
        return _activeElements.at(prevIndex);
    }
    return nullptr;
}

Element* TableFunction::nextElement(int index)
{
    if (index < _activeElements.size() - 1)
        return _activeElements.at(index + 1);
    switch (schema()->tripType())
    {
    case TripType::SP:
    case TripType::SW:
        return nullptr;

    case TripType::RR:
        if (index == 0) return nullptr;
        return _activeElements.at(0);
    }
    return nullptr;
}

QString TableFunction::calculateAtElem(Element *elem, int index, IsTwoSides twoSides)
{
    auto prevElem = prevElement(index);
    auto nextElem = nextElement(index);

    switch (schema()->tripType()) {
    case TripType::SW:
        if (prevElem && nextElem)
            return calculateInMiddle(elem, prevElem, nextElem, twoSides);
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
            return calculateInMiddle(elem, prevElem, nextElem, twoSides);
        return "Too few elements";

    case TripType::SP:
        // In middle of schema
        if (prevElem && nextElem)
            return calculateInMiddle(elem, prevElem, nextElem, twoSides);
        if (!prevElem)
        {
            if (twoSides)
            {
                Result res;
                res.element = elem;
                res.position = ResultPosition::LEFT;
                res.values = calculatePumpBeforeSchema(elem);
                _results << res;
            }

            // We can calculate beam after the first elem like if beam was in a medium
            // but the schema is invalid anyway - beamsize will show step-change
            // at the first elem when the second is medium, which is not physically correct.
            // But at least this calculation is consistent with those for SW schema.
            auto nextMedium = Z::Utils::asMedium(nextElem);
            auto overrideIor = nextMedium ? OptionalIor(nextMedium->ior()) : OptionalIor();
            auto resultPosition = twoSides ? ResultPosition::RIGHT : ResultPosition::ELEMENT;
            calculateAt(CalcElem(elem), ResultElem(elem, resultPosition), overrideIor);
            return "";
        }
        if (!nextElem)
        {
            if (twoSides) 
            {
                // We can calculate beam before the last elem like if beam was in a medium
                // but the schema is invalid anyway - beamsize will show step-change
                // at the last elem if pre-last is medium, which is not physically correct.
                // But at least this calculation is consistent with those for SW schema.
                auto prevMedium = Z::Utils::asMedium(prevElem);
                auto overrideIor = prevMedium ? OptionalIor(prevMedium->ior()) : OptionalIor();
                calculateAt(CalcElem(prevElem), ResultElem(elem, ResultPosition::LEFT), overrideIor);
            }
            // Calculate pump params after the last element
            auto resultPosition = twoSides ? ResultPosition::RIGHT : ResultPosition::ELEMENT;
            calculateAt(CalcElem(elem), ResultElem(elem, resultPosition));
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
        case TripType::SP: {
            Result res;
            res.element = iface;
            res.position = ResultPosition::IFACE_LEFT;
            res.values = calculatePumpBeforeSchema(iface);
            _results << res;
            break; // Don't return!
        }

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
        case TripType::SP: {
            Result res;
            res.element = range;
            res.position = ResultPosition::LEFT_OUTSIDE;
            res.values = calculatePumpBeforeSchema(range);
            _results << res;
            break; // Don't return!
        }

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

QString TableFunction::calculateInMiddle(Element* elem, Element* prevElem, Element* nextElem, IsTwoSides twoSides)
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

    if (twoSides)
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
    calc.multMatrix("TableFunction::calculateAt");

    const double ior = overrideIor.set ? overrideIor.value : (calcElem.range ? calcElem.range->ior() : 1);

    Result res;
    res.element = resultElem.elem;
    res.position = resultElem.pos;
    res.values = schema()->isResonator()
            ? calculateResonator(resultElem.elem, &calc, ior)
            : calculateSinglePass(resultElem.elem, &calc, ior);
    _results << res;
}

Z::Unit TableFunction::columnUnit(const ColumnDef &col) const
{
    if (_colUnits.contains(col.label))
        return _colUnits[col.label];
    if (col.dim == Z::Dims::linear()) {
        if (col.hint == ColumnDef::hintWavefront)
            return AppSettings::instance().defaultUnitFrontRadius;
        return AppSettings::instance().defaultUnitBeamRadius;
    }
    if (col.dim == Z::Dims::angular())
        return AppSettings::instance().defaultUnitAngle;
    return Z::Units::none();
}

void TableFunction::setColumnUnit(const QString &colLabel, Z::Unit unit)
{
    _colUnits[colLabel] = unit;
}
