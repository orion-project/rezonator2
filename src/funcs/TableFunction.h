#ifndef TABLEFUNCTION_H
#define TABLEFUNCTION_H

#include <memory>

#include "FunctionBase.h"

#include "core/OriTemplates.h"

class AbcdBeamCalculator;
class Schema;

// TODO: currently, this is BeamParamsAtElemsFunction function, not a general solution

class TableFunction : public FunctionBase
{
public:
    typedef Ori::Optional<double> OptionalIor;
    BOOL_PARAM(AlwaysTwoSides)

    struct ColumnDef
    {
        QString titleT, titleS;
        Z::Unit unit = Z::Units::none();
    };

    enum class ResultPosition
    {
        ELEMENT,       //   ()
        LEFT,          // ->()
        RIGHT,         //   ()->
        LEFT_OUTSIDE,  // ->[   ]
        LEFT_INSIDE,   //   [-> ]
        MIDDLE,        //   [ + ]
        RIGHT_INSIDE,  //   [ ->]
        RIGHT_OUTSIDE, //   [   ]->
        IFACE_LEFT,    // ->|
        IFACE_RIGHT,   //   |->
    };

    struct ResultPositionInfo
    {
        QString ascii;
        QString tooltip;
        QString pixmap;
    };

    static const ResultPositionInfo& resultPositionInfo(ResultPosition pos);

    struct Result
    {
        Element* element;
        ResultPosition position = ResultPosition::ELEMENT;
        QVector<Z::PointTS> values;

        QString str() const;
    };

    struct CalcElem
    {
        Element* elem = nullptr;
        ElementRange *range = nullptr;
        double subrange = 0;
        enum class SubrangeOpt {NONE, MID, END};
        SubrangeOpt subrangeOpt = SubrangeOpt::NONE;
        Element* ref() const { return range ? range : elem; }
        CalcElem(Element *elem): elem(elem) {}
        static CalcElem Range(ElementRange *range, double subrange) { return CalcElem(range, subrange); }
        static CalcElem RangeBeg(ElementRange *range) { return CalcElem(range, 0.0); }
        static CalcElem RangeMid(ElementRange *range) { return CalcElem(range, SubrangeOpt::MID); }
        static CalcElem RangeEnd(ElementRange *range) { return CalcElem(range, SubrangeOpt::END); }
     private:
        CalcElem(ElementRange *range, double subrange): range(range), subrange(subrange) {}
        CalcElem(ElementRange *range, SubrangeOpt opt): range(range), subrangeOpt(opt) {}
    };

    struct ResultElem
    {
        Element* elem;
        ResultPosition pos;
        ResultElem(Element* elem, ResultPosition pos): elem(elem), pos(pos) {}
    };

public:
    TableFunction(Schema *schema);

    /// Returns a path to function icon.
    /// Icon can be used to display in window title or menus.
    virtual const char* iconPath() const { return ""; }

    virtual void calculate();

    bool ok() const { return _errorText.isEmpty(); }
    const QString& errorText() const { return _errorText; }

    virtual QVector<ColumnDef> columns() const;

    const QVector<Result>& results() const { return _results; }

    /// Calculate additional points which are excessive for user
    /// because they are duplicated (already calculated via neigbor elements)
    /// but can be useful for testing
    bool calcMediumEnds = false;
    bool calcEmptySpaces = false;

protected:
    QString _errorText;
    QVector<Result> _results;
    Z::PairTS<std::shared_ptr<PumpCalculator>> _pumpCalc;
    std::shared_ptr<AbcdBeamCalculator> _beamCalc;
    QList<Element*> _activeElements; // valid only during calculate() call

    bool prepareSinglePass();
    bool prepareResonator();
    Element* prevElement(int index);
    Element* nextElement(int index);
    QString calculateAtElem(Element* elem, int index, AlwaysTwoSides alwaysTwoSides);
    QString calculateAtInterface(ElementInterface* iface, int index);
    QString calculateAtCrystal(ElementRange* range, int index);
    QString calculateAtPlane(Element* elem, int index);
    QString calculateInMiddle(Element* elem, Element *prevElem, Element *nextElem, AlwaysTwoSides alwaysTwoSides);
    void calculateAt(CalcElem calcElem, ResultElem resultElem, OptionalIor overrideIor = OptionalIor());
    void calculatePumpBeforeSchema(Element* elem, ResultPosition resultPos);
    QVector<Z::PointTS> calculateSinglePass(RoundTripCalculator* calc, double ior) const;
    QVector<Z::PointTS> calculateResonator(RoundTripCalculator* calc, double ior) const;
};

#endif // TABLEFUNCTION_H
