#ifndef FORMULA_H
#define FORMULA_H

#include "Parameters.h"

namespace Z {

class Formula
{
public:
    Formula(Parameter* target): _target(target) {}

    void calculate();

private:
    Parameter* _target;
};

} // namespace Z

#endif // FORMULA_H
