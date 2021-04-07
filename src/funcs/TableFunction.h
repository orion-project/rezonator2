#ifndef TABLEFUNCTION_H
#define TABLEFUNCTION_H

#include <memory>

#include "FunctionBase.h"

class AbcdBeamCalculator;
class Schema;

struct OptionalIor {
    bool set;
    double ior;
    OptionalIor(): set(false){}
    OptionalIor(double ior): set(true), ior(ior) {}
};

// TODO: currently, this is BeamParamsAtElemsFunction function, not a general solution

class TableFunction : public FunctionBase
{
public:
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
    bool calcDebugResults = false;

protected:
    QString _errorText;
    QVector<Result> _results;
    Z::PairTS<std::shared_ptr<PumpCalculator>> _pumpCalc;
    std::shared_ptr<AbcdBeamCalculator> _beamCalc;

    bool prepareSinglePass();
    bool prepareResonator();
    Element* prevElement(int index);
    Element* nextElement(int index);
    bool calculateAtMirrorOrLens(Element* elem, int index);
    bool calculateAtInterface(ElementInterface* iface, int index);
    bool calculateAtCrystal(ElementRange* range, int index);
    void calculateAt(Element* calcElem, bool calcSubrange, Element* resultElem,
                     ResultPosition resultPos, OptionalIor overrideIor = OptionalIor());
    void calculatePumpBeforeSchema(Element* elem, ResultPosition resultPos);
    QVector<Z::PointTS> calculateSinglePass(RoundTripCalculator* calc, double ior) const;
    QVector<Z::PointTS> calculateResonator(RoundTripCalculator* calc, double ior) const;
};

#endif // TABLEFUNCTION_H
