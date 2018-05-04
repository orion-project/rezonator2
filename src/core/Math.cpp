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

Complex operator *(const Complex &c, const Matrix &m)
{
    return Complex(0, 0); // TODO
}

Matrix operator *(const Matrix &m1, const Matrix &m2)
{
    double a = m1.A * m2.A + m1.B * m2.C;
    double b = m1.A * m2.B + m1.B * m2.D;
    double c = m1.C * m2.A + m1.D * m2.C;
    double d = m1.C * m2.B + m1.D * m2.D;
    return Matrix(a, b, c, d);
}

void Matrix::operator *= (const Matrix &m)
{

    double a = A * m.A + B * m.C;
    double b = A * m.B + B * m.D;
    double c = C * m.A + D * m.C;
    double d = C * m.B + D * m.D;
    assign(a, b, c, d);
}

void Matrix::operator *= (const Matrix *m)
{
    double a = A * m->A + B * m->C;
    double b = A * m->B + B * m->D;
    double c = C * m->A + D * m->C;
    double d = C * m->B + D * m->D;
    assign(a, b, c, d);
}

QString Matrix::str() const
{
    return QString("[A=%1; B=%2; C=%3; D=%4]").arg(Z::str(A), Z::str(B), Z::str(C), Z::str(D));
}

//------------------------------------------------------------------------------
//                                RayVector
//------------------------------------------------------------------------------

RayVector::RayVector(const RayVector& in, const Matrix& m)
{
    Y = in.Y * m.A + in.V * m.B;
    V = in.Y * m.C + in.V * m.D;
}

QString RayVector::str() const
{
    return QString("[Y: %1; V: %2]").arg(Z::str(Y), Z::str(V));
}

} // namespace Z

