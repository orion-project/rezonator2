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
    struct Line
    {
    public:
        Line(const QString &id): _id(id) {}
        QString id() const { return _id; }
        const QVector<double>& x() const { return _x; }
        const QVector<double>& y() const { return _y; }
        int size() const { return _x.size(); }
        void append(double x, double y) { _x.append(x); _y.append(y); }
    private:
        QString _id;
        QVector<double> _x, _y;
    };

public:
    PlotFunctionV2(Schema *schema);

    void calculate();
    
    virtual PlotFuncDeps dependsOn() const { return {}; }

protected:
    QVector<Line> _lines;
    QHash<QString, int> _lineIndex;
    
    virtual void calculateInternal() {}

    void addPoint(const QString &id, double x, double y);
};

#endif // PLOT_FUNCTION_V2_H