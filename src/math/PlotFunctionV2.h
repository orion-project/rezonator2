#ifndef PLOT_FUNCTION_V2_H
#define PLOT_FUNCTION_V2_H

#include "FunctionBase.h"

/**
    Base class for plotting functions.
    This is a simplifield version of @sa PlotFunction storing results as a plain list.
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

protected:
    QVector<Line> _lines;
    QHash<QString, int> _lineIndex;
    
    virtual void calculateInternal() {}

    void addPoint(const QString &id, double x, double y);
};

#endif // PLOT_FUNCTION_V2_H