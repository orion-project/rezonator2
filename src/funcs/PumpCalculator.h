#ifndef PUMP_CALCULATOR_H
#define PUMP_CALCULATOR_H

namespace Z {
class PumpParams;
}

class PumpCalculator final
{
public:
    static PumpCalculator T();
    static PumpCalculator S();
    ~PumpCalculator();

    bool init(Z::PumpParams* pump, double lambdaSI);

private:
    PumpCalculator();

    class PumpCalculatorImpl *_impl;
};

#endif // PUMP_CALCULATOR_H
