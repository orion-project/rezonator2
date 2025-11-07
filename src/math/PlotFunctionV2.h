#ifndef PLOT_FUNCTION_V2_H
#define PLOT_FUNCTION_V2_H

#include "FunctionBase.h"
#include "PlotFunctionUtils.h"

/**
    Base class for plotting functions.
    Plotting function is a function presenting its calculation results in graphical form.
    Alternatively, they can be used for calculation of a single result, see @a calculateAt.

    This is a simplifield version of @sa PlotFunction storing results as a plain list.
    
    @sa PlotFuncWindow and @sa PlotFunction should be considered obsolete
    and all new plot functions should be implemented via PlotFunctionV2 and @sa PlotFuncWindowV2.
*/
class PlotFunctionV2 : public FunctionBase
{
public:
    /// Result of calculation of a plot function
    class Line
    {
    public:
        QString id() const { return _id; }
        const QVector<double>& x() const { return _x; }
        const QVector<double>& y() const { return _y; }
        int size() const { return _x.size(); }

    private:
        QString _id;
        QVector<double> _x, _y;

        Line(const QString &id): _id(id) {}
        void append(double x, double y) { _x.append(x); _y.append(y); }

        friend class PlotFunctionV2;
    };

public:
    PlotFunctionV2(Schema *schema);

    void calculate();
    
    virtual PlotFuncDeps dependsOn() const { return {}; }
    
    /// Results of calculation of a plot function.
    /// Functions can provide several results.
    /// For example, the "Beamsize variation" function can have breaks
    /// at such argument values when the system in unstable,
    /// and the function produces several not connected lines.
    /// Line breaks are made automatically by @sa PlotFunctionV2::addPoint()
    /// when Y-value becomes NaN or Infinity,
    /// or can be triggererd manually by @sa PlotFunctionV2::endLine()
    const QVector<Line>& lines() const { return _lines; }

protected:
    virtual bool prepare() { return true; }
    virtual void unprepare() {}
    virtual void calculateInternal() {}

    void addPoint(const QString &id, double x, double y);
    void endLine(const QString &id);
    
private:
    QVector<Line> _lines;
    QHash<QString, int> _lineIndex;
};

#endif // PLOT_FUNCTION_V2_H