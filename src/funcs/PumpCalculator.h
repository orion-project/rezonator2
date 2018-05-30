#ifndef PUMP_CALCULATOR_H
#define PUMP_CALCULATOR_H

namespace Z {
class Matrix;
class PumpParams;
}

struct BeamResult
{
    double beamRadius; ///< Beam rdius.
    double frontRadius; ///< Wavefront ROC.
    double halfAngle; ///< Half of divergence angle.
};

class PumpCalculator final
{
public:
    static PumpCalculator* T();
    static PumpCalculator* S();

    ~PumpCalculator();

    bool init(Z::PumpParams* pump, double lambdaSI);

    BeamResult calc(const Z::Matrix& matrix);

private:
    PumpCalculator() = default;
    PumpCalculator(const PumpCalculator& other) = delete;
    PumpCalculator(PumpCalculator&& other) = delete;
    PumpCalculator& operator =(const PumpCalculator& other) = delete;
    PumpCalculator& operator =(PumpCalculator&& other) = delete;

    class PumpCalculatorImpl *_impl = nullptr;
};

#endif // PUMP_CALCULATOR_H
