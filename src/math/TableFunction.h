#ifndef TABLEFUNCTION_H
#define TABLEFUNCTION_H

#include <memory>

#include "FunctionBase.h"

#include "core/OriTemplates.h"

class BeamCalculator;
class Schema;

class TableFunction : public FunctionBase
{
public:
    typedef Ori::Optional<double> OptionalIor;
    BOOL_PARAM(IsTwoSides)

    struct Params
    {
        /// Calculate additional points which are excessive for user
        /// because they are duplicated (already calculated via neigbor elements)
        /// but can be useful for testing
        bool calcMediumEnds = false;
        bool calcEmptySpaces = false;
        bool calcSpaceMids = false;
    };
    
    struct ColumnDef
    {
        QString label;
        QString title;
        Z::Dim dim = Z::Dims::none();
        enum Hint { hintNone, hintBeamsize, hintWavefront };
        Hint hint = hintNone;
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
        QString icon_path;
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

    void calculate();

    const QVector<ColumnDef>& columns() { return _columns; }
    Z::Unit columnUnit(const ColumnDef &col) const;
    void setColumnUnit(const QString &colLabel, Z::Unit unit);
    const QMap<QString, Z::Unit>& columntUnits() const { return _colUnits; }

    const QVector<Result>& results() const { return _results; }

    Params params() const { return _params; }
    void setParams(const Params& params);

protected:
    // These are valid only during calculate() call
    std::shared_ptr<BeamCalculator> _beamCalc;
    QList<Element*> _activeElements;

    QVector<ColumnDef> _columns;
    QMap<QString, Z::Unit> _colUnits;
    Params _params;

    Element* prevElement(int index);
    Element* nextElement(int index);
    QString calculateAtElem(Element* elem, int index, IsTwoSides twoSides);
    QString calculateAtInterface(ElementInterface* iface, int index);
    QString calculateAtCrystal(ElementRange* range, int index);
    QString calculateAtPlane(Element* elem, int index);
    QString calculateInMiddle(Element* elem, Element *prevElem, Element *nextElem, IsTwoSides twoSides);
    void calculateAt(CalcElem calcElem, ResultElem resultElem, OptionalIor overrideIor = OptionalIor());

    virtual bool prepare() { return true; }
    virtual void unprepare() {}
    virtual QVector<Z::PointTS> calculatePumpBeforeSchema(Element *elem) { return {}; };
    virtual QVector<Z::PointTS> calculateInternal(Element *elem, double ior) = 0;
    
private:
    QVector<Result> _results;
};

#endif // TABLEFUNCTION_H
