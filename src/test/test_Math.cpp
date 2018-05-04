#include "testing/OriTestBase.h"
#include "../core/Math.h"
#include "TestUtils.h"

namespace Z {
namespace Test {
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
    ASSERT_MATRIX_IS_NOT_UNITY(m)
    m.unity();
    ASSERT_MATRIX_IS_UNITY(m)
}

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

TEST_METHOD(Matrix_multiply_static_matrix)
{
    Z::Matrix m1(5, 6, 7, 8);
    Z::Matrix m2(1, 2, 3, 4);

    auto m3 = m2 * m1;
    ASSERT_MATRIX_IS(m3, 19.0, 22.0, 43.0, 50.0)
}

TEST_METHOD(Matrix_multiply_static_complex)
{
    Z::Matrix m1(5, 6, 7, 8);
    Z::Complex c1(1, 2);

    auto c2 = c1 * m1;
    ASSERT_EQ_DBL(c2.real(), 0)
    ASSERT_EQ_DBL(c2.imag(), 0)
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
    v2.set(v1);
    ASSERT_VECTOR(v2, 10, 20)
}

//------------------------------------------------------------------------------

TEST_GROUP("Math",
    ADD_TEST(Matrix_constructors),
    ADD_TEST(Matrix_assign),
    ADD_TEST(Matrix_unity),
    ADD_TEST(Matrix_multiply),
    ADD_TEST(Matrix_multiply_static_matrix),
    ADD_TEST(Matrix_multiply_static_complex),
    ADD_TEST(RayVector_constructors),
    ADD_TEST(RayVector_set)
)

} // namespace MathTests
} // namespace Test
} // namespace Z
