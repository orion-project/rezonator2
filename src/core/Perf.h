#ifndef PERF_H
#define PERF_H

#define Z_PERF_ENABLED

#ifdef Z_PERF_ENABLED

/**

Simple performance tracker.

Below is an example output for plotting of beam variation with 100 points
when changing a global parameter. It can bee seen that calculation is slow
because of lot of calls of parameterChanged handlers and the most of time 
is spent in SchemaParamsTable::parameterChanged so it needs to be optimized.

In square brackets is the number of calls.

BeamVariationFunction: 824_ms [1]"
    setValue: 824_ms [100]"
        ParameterBase::notifyListeners: 824_ms [100]"
            ParameterBase::notifyListeners: 420_ms [100]"
                ParameterLink::apply: 0_ms [100]"
                    ParameterBase::notifyListeners: 0_ms [300]"
                        Element::parameterChanged_1: 0_ms [300]"
                            ElemEmptyRange::calcMatrixInternal: 0_ms [100]"
                        Element::parameterChanged_2: 0_ms [300]"
                SchemaParamsTable::parameterChanged: 419_ms [200]"
            ParameterLink::apply: 0_ms [200]"
                ParameterBase::notifyListeners: 0_ms [300]"
                    Element::parameterChanged_1: 0_ms [300]"
                        ElemEmptyRange::calcMatrixInternal: 0_ms [100]"
                    Element::parameterChanged_2: 0_ms [300]"
            SchemaParamsTable::parameterChanged: 387_ms [200]"
    recalcSubrange: 0_ms [100]"
    multMatrix: 0_ms [100]"
    addResultPoint: 0_ms [100]"

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
