#ifndef MATH_H
#define MATH_H

#include "Complex.h"

#include <QString>

#define SQR(a) ((a)*(a))

namespace Z {

namespace Const {
extern const double Pi;
extern const double LightSpeed;
}

//------------------------------------------------------------------------------

/**
    The ABCD ray matrix.
*/
class Matrix
{
public:
    Complex A, B, C, D;

    Matrix() : A(1), B(0), C(0), D(1) {}
    Matrix(const Matrix& m) : A(m.A), B(m.B), C(m.C), D(m.D) {}
    Matrix(const Matrix* m) : A(m->A), B(m->B), C(m->C), D(m->D) {}
    Matrix(double a, double b, double c, double d) : A(a), B(b), C(c), D(d) {}
    Matrix(const Complex& a, const Complex& b, const Complex& c, const Complex& d) : A(a), B(b), C(c), D(d) {}

    void unity()
    {
        A = {1, 0}; B = {0, 0}; C = {0, 0}; D = {1, 0};
    }

    void assign(double a, double b, double c, double d)
    {
        A = a; B = b; C = c; D = d;
    }

    void assign(const Complex& a, const Complex& b, const Complex& c, const Complex& d)
    {
        A = a; B = b; C = c; D = d;
    }

    bool isReal() const;

    void operator *= (const Matrix &m);
    void operator *= (const Matrix *m);

    void operator = (const Matrix &m)
    {
        A = m.A; B = m.B; C = m.C; D = m.D;
    }

    void operator = (const Matrix *m)
    {
        A = m->A; B = m->B; C = m->C; D = m->D;
    }

    /// Transformation of complex ROC by ray matrix
    Complex multComplexBeam(const Complex& c) const;

    QString str() const;

    Complex det() const;
};

Matrix operator *(const Matrix &m1, const Matrix &m2);

typedef QVector<const Matrix*> MatrixArray;

//------------------------------------------------------------------------------

class RayVector
{
public:
    double Y, V;

    RayVector(): Y(0), V(0) {}
    RayVector(const double& y, const double& v): Y(y), V(v) {}
    RayVector(const RayVector& v): Y(v.Y), V(v.V) {}
    RayVector(const RayVector& in, const Matrix& m);

    void set(const double& y, const double& v) { Y = y; V = v; }

    void operator = (const RayVector& v) { Y = v.Y; V = v.V; }

    QString str() const;
};

} // namespace Z

#endif // MATH_H
