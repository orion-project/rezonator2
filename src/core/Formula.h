#ifndef FORMULA_H
#define FORMULA_H

#include "Parameters.h"

#include <QMap>

namespace Z {

class Formula
{
public:
    Formula(Parameter* target): _target(target) {}

    void calculate();

    Parameter* target() { return _target; }

private:
    Parameter* _target;
};

//------------------------------------------------------------------------------

class Formulas
{
public:
    Formula* get(Parameter*);
    void put(Formula*);
    void free(Parameter*);
    void clear();

private:
    QMap<Parameter*, Formula*> _items;
};

} // namespace Z

#endif // FORMULA_H
