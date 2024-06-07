#ifndef PERF_H
#define PERF_H

#define Z_PERF_ENABLED

#ifdef Z_PERF_ENABLED

/**

Simple performance tracker.

Below is an example output for plotting of beam variation with 100 points
when changing a global parameter. It can bee seen that calculation is slow
because of lot of calls of parameterChanged handlers:

BeamVariationFunction: 847_ms [1]
    setValue: 846_ms [100]
        ParameterBase::notifyListeners: 845_ms [100]
            ParameterLink::apply: 452_ms [100]
                ParameterBase::notifyListeners: 452_ms [100]
                    Element::parameterChanged_1: 0_ms [100]
                        ElemEmptyRange::calcMatrixInternal: 0_ms [100]
                    Element::parameterChanged_2: 452_ms [100]
                        Schema::elementChanged: 452_ms [100]
            SchemaParamsTable::parameterChanged: 393_ms [200]
    recalcSubrange: 0_ms [100]
    multMatrix: 0_ms [100]
    addResultPoint: 1_ms [100]

*/

namespace Z::Perf {

void reset();
void begin(const char *id);
void end();
void print();

} // namespace Z::Perf

#define Z_PERF_RESET Z::Perf::reset();
#define Z_PERF_BEGIN(id) Z::Perf::begin(id);
#define Z_PERF_END Z::Perf::end();
#define Z_PERF_PRINT Z::Perf::print();

#else

#define Z_PERF_RESET
#define Z_PERF_BEGIN(id)
#define Z_PERF_END
#define Z_PERF_PRINT

#endif

#endif // PERF_H
