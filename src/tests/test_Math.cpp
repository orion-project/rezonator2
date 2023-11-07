#include "../core/Math.h"
#include "../tests/TestUtils.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {
namespace MathTests {

//------------------------------------------------------------------------------

TEST_METHOD(Matrix_constructors)
{
    Z::Matrix m;
    ASSERT_MATRIX_IS_UNITY(m)

    Z::Matrix m1(1.1, 2.2, 3.3, 4.4);
    ASSERT_MATRIX_IS(m1, 1.1, 2.2, 3.3, 4.4)
}

TEST_METHOD(Matrix_assign)
{
    Z::Matrix m;
    m.assign(1.1, 2.2, 3.3, 4.4);
    ASSERT_MATRIX_IS(m, 1.1, 2.2, 3.3, 4.4)
}

TEST_METHOD(Matrix_unity)
{
    Z::Matrix m(1.1, 2.2, 3.3, 4.4);
    ASSERT_MATRIX_IS(m, 1.1, 2.2, 3.3, 4.4)
    m.unity();
    ASSERT_MATRIX_IS_UNITY(m)
}

// Calculation: $PRJECT/calc/Matrix.py
TEST_METHOD(Matrix_multiply)
{
    Z::Matrix m1(5, 6, 7, 8);

    // by ref
    Z::Matrix m2(1, 2, 3, 4);
    m2 *= m1;
    ASSERT_MATRIX_IS(m2, 19.0, 22.0, 43.0, 50.0)

    // by pointer
    Z::Matrix m3(1, 2, 3, 4);
    m3 *= &m1;
    ASSERT_MATRIX_IS(m3, 19.0, 22.0, 43.0, 50.0)
}

// Calculation: $PROJECT/calc/Matrix.py
TEST_METHOD(Matrix_multiply_static)
{
    Z::Matrix m1(5, 6, 7, 8);
    Z::Matrix m2(1, 2, 3, 4);

    auto m3 = m1 * m2;
    ASSERT_MATRIX_IS(m3, 23, 34, 31, 46)

    auto m4 = m2 * m1;
    ASSERT_MATRIX_IS(m4, 19, 22, 43, 50)
}

// Calculation: $PROJECT/calc/Matrix.py
TEST_METHOD(Matrix_multComplexBeam)
{
    Z::Matrix m1(5, 6, 7, 8);
    Z::Complex c1(1, 2);

    auto c2 = m1.multComplexBeam(c1);
    ASSERT_NEAR_DBL(c2.real(), 0.7244656, 1e-7)
    ASSERT_NEAR_DBL(c2.imag(), -0.0095012, 1e-7)
}

//------------------------------------------------------------------------------

#define ASSERT_VECTOR(vector, y, v)\
    ASSERT_EQ_DBL(vector.Y, y)\
    ASSERT_EQ_DBL(vector.V, v)

TEST_METHOD(RayVector_constructors)
{
    Z::RayVector v0;
    ASSERT_VECTOR(v0, 0, 0)

    Z::RayVector v1(10, 20);
    ASSERT_VECTOR(v1, 10, 20)

    Z::RayVector v2(v1);
    ASSERT_VECTOR(v2, 10, 20)

    Z::Matrix m(2, 3, 4, 5);
    Z::RayVector v3(v1, m);
    ASSERT_VECTOR(v3, 10*2 + 20*3, 10*4 + 20*5)
}

TEST_METHOD(RayVector_set)
{
    Z::RayVector v1;
    v1.set(10, 20);
    ASSERT_VECTOR(v1, 10, 20)

    Z::RayVector v2;
    v2 = v1;
    ASSERT_VECTOR(v2, 10, 20)
}

//------------------------------------------------------------------------------

TEST_GROUP("Math",
    ADD_TEST(Matrix_constructors),
    ADD_TEST(Matrix_assign),
    ADD_TEST(Matrix_unity),
    ADD_TEST(Matrix_multiply),
    ADD_TEST(Matrix_multiply_static),
    ADD_TEST(Matrix_multComplexBeam),
    ADD_TEST(RayVector_constructors),
    ADD_TEST(RayVector_set)
)

} // namespace MathTests
} // namespace Tests
} // namespace Z
