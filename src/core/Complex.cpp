#include "Complex.h"

#include "core/OriFloatingPoint.h"

#include <qglobal.h>

namespace Z {

bool isReal(const Complex& v)
{
#ifdef Q_OS_MAC
    // While Windows and Linux versions give exact zero here,
    // macOS version gives some small value (not digged why).
    // Probably this epsilon has to be adjusted, it's given from tests.
    return std::isnan(v.imag()) || qAbs(v.imag()) <= 1e-18;
#else
    static Double zero(0);
    return std::isnan(v.imag()) || Double(v.imag()).almostEqual(zero);
#endif
}

bool isImag(const Complex& v)
{
#ifdef Q_OS_MAC
    // While Windows and Linux versions give exact zero here,
    // macOS version gives some small value (not digged why)
    // Probably this epsilon has to be adjusted, it's given from tests.
    return std::isnan(v.real()) || qAbs(v.real()) <= 1e-18;
#else
    static Double zero(0);
    return std::isnan(v.real()) || Double(v.real()).almostEqual(zero);
#endif
}

}
