/*
    Performance test of matrix multiplication - real vs complex.

    Typical 2D stability map can contain 1000 x 1000 points,
    typical schema can contain 10 elements,
    so we have 10 million matrix multiplications.
    During multiplication, all matrices stay the same,
    while one of them changes every iteration,
    and another changes every

    Build:
    g++ perf_test_mult_matrs.cpp -o ../../bin/perf_test_mult_matrs

    Run:
    ../../bin/perf_test_mult_matrs
    * 
    Some results (ms):
    X=1000, Y=1, E=100: real=0, cplx=46
    X=1000, Y=1000, E=10: real=783, cplx=3888
    X=1000, Y=1000, E=100: real=4198, cplx=38593
*/

#include <complex>
#include <chrono>
#include <random>
#include <iostream>

using namespace std::chrono; 

static const int X_COUNT = 1000;
static const int Y_COUNT = 1000;
static const int E_COUNT = 10;
static const int X_ARG = 3;
static const int Y_ARG = 7;

typedef std::complex<double> Complex;

class Matrix {
public:
    double A, B, C, D;
    Matrix() : A(1), B(0), C(0), D(1) {}
    Matrix(double a, double b, double c, double d) : A(a), B(b), C(c), D(d) {}
    void assign(double a, double b, double c, double d) {
        A = a; B = b; C = c; D = d;
    }
    void operator *= (const Matrix &m);
};

void Matrix::operator *= (const Matrix &m) {
    double a = A * m.A + B * m.C;
    double b = A * m.B + B * m.D;
    double c = C * m.A + D * m.C;
    double d = C * m.B + D * m.D;
    assign(a, b, c, d);
}

class CMatrix {
public:
    Complex A, B, C, D;
    CMatrix() : A(1, 0), B(0, 0), C(0, 0), D(1, 0) {}
    CMatrix(double a, double b, double c, double d) : A(a, a), B(b, b), C(c, c), D(d, d) {}
    void assign(double a, double b, double c, double d) {
        A = {a, a}, B = {b, b}, C = {c, c}, D = {d, d};
    }
    void operator *= (const CMatrix &m);
};

void CMatrix::operator *= (const CMatrix &m) {
    Complex a = A * m.A + B * m.C;
    Complex b = A * m.B + B * m.D;
    Complex c = C * m.A + D * m.C;
    Complex d = C * m.B + D * m.D;
    A = a, B = b, C = c, D = d;
}

double rnd() {
    static std::mt19937 random_gen;
    static std::normal_distribution<double> normal_dist(10, 10);
    return normal_dist(random_gen);
}

template <typename ABCD>
void measure(const char *ident) {
    auto start = high_resolution_clock::now();

    std::vector<ABCD> schema(E_COUNT);
    for (int i = 0; i < E_COUNT; i++) {
        schema.push_back(ABCD(rnd(), rnd(), rnd(), rnd()));
    }
    ABCD m0;
    for (int x = 0; x < X_COUNT; x++) {
        schema.at(X_ARG).assign(rnd(), rnd(), rnd(), rnd());
        for (int y = 0; y < Y_COUNT; y++) {
            schema.at(Y_ARG).assign(rnd(), rnd(), rnd(), rnd());
            for (const ABCD& m : schema) {
                m0 *= m;
            }
        }
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout << ident << duration.count() << " ms" << std::endl;
}

int main() {
    std::cout << "X=" << X_COUNT << ", "
              << "Y=" << Y_COUNT << ", "
              << "E=" << E_COUNT << std::endl;
    measure<Matrix>("real: ");
    measure<CMatrix>("cplx: ");
    return 0;
}
