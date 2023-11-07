#include "MultirangeCausticFunction.h"

MultirangeCausticFunction::~MultirangeCausticFunction()
{
    qDeleteAll(_funcs);
}

QVector<Z::Variable> MultirangeCausticFunction::args() const
{
    QVector<Z::Variable> args;
    foreach (auto func, _funcs)
        args.append(*(func->arg()));
    return args;
}

CausticFunction::Mode MultirangeCausticFunction::mode() const
{
    return _funcs.first()->mode();
}

void MultirangeCausticFunction::setMode(CausticFunction::Mode mode)
{
    foreach (CausticFunction *func, _funcs)
        func->setMode(mode);
}

// TODO ensure if all subfunctions have the same _argumentUnit
void MultirangeCausticFunction::setArgs(const QVector<Z::Variable>& args)
{
    QList<CausticFunction*> funcs;
    foreach (const Z::Variable& arg, args)
    {
        bool funcUpdated = false;
        foreach (CausticFunction *func, _funcs)
            if (func->arg()->element == arg.element)
            {
                *(func->arg()) = arg;
                _funcs.removeOne(func);
                funcs.append(func);
                funcUpdated = true;
                break;
            }
        if (!funcUpdated)
        {
            auto func = new CausticFunction(schema());
            *(func->arg()) = arg;
            funcs.append(func);
        }
    }
    qDeleteAll(_funcs);
    _funcs = funcs;
}

void MultirangeCausticFunction::calculate()
{
    setError(QString());
    int disabledCount = 0;
    foreach (CausticFunction *func, _funcs)
    {
        if (func->arg()->element->disabled())
        {
            disabledCount++;
            continue;
        }
        func->calculate();
        if (!func->ok())
        {
            setError(func->errorText());
            foreach (auto f, _funcs)
                f->clearResults();
            break;
        }
    }
    if (disabledCount == _funcs.size())
        setError("All elements are disabled");
}

int MultirangeCausticFunction::resultCount(Z::WorkPlane plane) const
{
    int count = 0;
    for (CausticFunction *func : _funcs)
        if (!func->arg()->element->disabled())
            count += func->resultCount(plane);
    return count;
}

const PlotFuncResult& MultirangeCausticFunction::result(Z::WorkPlane plane, int index) const
{
    int index1 = 0;
    for (CausticFunction *func : _funcs)
    {
        if (func->arg()->element->disabled())
            continue;
        int index2 = index1 + func->resultCount(plane);
        if (index >= index1 && index < index2)
            return func->result(plane, index - index1);
        index1 = index2;
    }

    QString errorMsg = QString("MultiCausticFunction::result(): "
        "%1 is invalid result index for plane %2").arg(index).arg(int(plane));
    qCritical() << errorMsg;
    throw std::runtime_error(errorMsg.toStdString()); // let it crash
}

void MultirangeCausticFunction::setPump(PumpParams* pump)
{
    foreach (CausticFunction *func, _funcs)
        func->setPump(pump);
}

Z::PointTS MultirangeCausticFunction::calculateAt(const Z::Value &arg)
{
    double remainingL = arg.toSi();
    foreach (CausticFunction *func, _funcs)
    {
        if (func->arg()->element->disabled())
            continue;
        auto elem = Z::Utils::asRange(func->arg()->element);
        double L = elem->axisLengthSI();
        double newRemainingL = remainingL - L;
        if (newRemainingL <= 0)
            return func->calculateAt(Z::Value(remainingL, Z::Units::m()));
        remainingL = newRemainingL;
    }
    return { Double::nan(), Double::nan() };
}
