#ifndef Z_COMPLEX_H
#define Z_COMPLEX_H

#include <complex>

namespace Z {

typedef std::complex<double> Complex;

bool isImag(const Complex& v);
bool isReal(const Complex& v);

} // namespace Z

#endif // Z_COMPLEX_H
