#include "Format.h"
#include "Math.h"

namespace Z {

namespace Const {
extern const double Pi = 3.14159265358979323846;
extern const double LightSpeed = 299792458.0; // m/sec, light speed in vacuum
}

//------------------------------------------------------------------------------
//                                 Matrix
//------------------------------------------------------------------------------

Matrix operator *(const Matrix &m1, const Matrix &m2)
{
    Complex a = m1.A * m2.A + m1.B * m2.C;
    Complex b = m1.A * m2.B + m1.B * m2.D;
    Complex c = m1.C * m2.A + m1.D * m2.C;
    Complex d = m1.C * m2.B + m1.D * m2.D;
    return Matrix(a, b, c, d);
}

void Matrix::operator *= (const Matrix &m)
{

    Complex a = A * m.A + B * m.C;
    Complex b = A * m.B + B * m.D;
    Complex c = C * m.A + D * m.C;
    Complex d = C * m.B + D * m.D;
    assign(a, b, c, d);
}

void Matrix::operator *= (const Matrix *m)
{
    Complex a = A * m->A + B * m->C;
    Complex b = A * m->B + B * m->D;
    Complex c = C * m->A + D * m->C;
    Complex d = C * m->B + D * m->D;
    assign(a, b, c, d);
}

QString Matrix::str() const
{
    return QString("[A=%1; B=%2; C=%3; D=%4]").arg(Z::str(A), Z::str(B), Z::str(C), Z::str(D));
}

Complex Matrix::multComplexBeam(const Complex& c) const
{
    return (c * A + B) / (c * C + D);
}

Complex Matrix::det() const
{
    return A * D - B * C;
}

bool Matrix::isReal() const
{
    return A.imag() == 0 && B.imag() == 0 && C.imag() == 0 && D.imag() == 0;
}

//------------------------------------------------------------------------------
//                                RayVector
//------------------------------------------------------------------------------

RayVector::RayVector(const RayVector& in, const Matrix& m)
{
    // TODO: COMPLEX: what about imaginary part?
    Y = in.Y * m.A.real() + in.V * m.B.real();
    V = in.Y * m.C.real() + in.V * m.D.real();
}

QString RayVector::str() const
{
    return QString("[Y: %1; V: %2]").arg(Z::str(Y), Z::str(V));
}

} // namespace Z

