#include "testing/OriTestBase.h"
#include "../funcs/GrinCalculator.h"

#include <cmath>

// Test values are produced by script $PROJECT/calc/grin_focus_assessment.ipynb

namespace Z {
namespace Tests {
namespace GrinCalculatorTests {

#define ASSERT_GRIN_F(L, n0, n2, F) { \
    auto res = GrinCalculator::calc_focus(L, n0, n2); \
    if (!res.ok()) TEST_LOG(res.error()) \
    ASSERT_IS_TRUE(res.ok()) \
    ASSERT_NEAR_DBL(res.result(), F, 1e-14) \
}

#define ASSERT_GRIN_N2(L, n0, F, n2) { \
    auto res = GrinCalculator::solve_n2(L, n0, F); \
    if (!res.ok()) TEST_LOG(res.error()) \
    ASSERT_IS_TRUE(res.ok()) \
    ASSERT_NEAR_DBL(res.result(), n2, 1e-12) \
}

TEST_METHOD(calc_focus)
{
    ASSERT_GRIN_F(0.01, 1.73, -68, -1.4725145128580228)
    ASSERT_GRIN_F(0.01, 1.73, -6.8, -14.707809084726811)
    ASSERT_GRIN_F(0.01, 1.73, 6.8, 14.703955520175825)
    ASSERT_GRIN_F(0.01, 1.73, 68, 1.468660947932803)

    ASSERT_GRIN_F(0.1, 1.73, -68, -0.16583993377698594)
    ASSERT_GRIN_F(0.1, 1.73, -6.8, -1.4898057564340426)
    ASSERT_GRIN_F(0.1, 1.73, 6.8, 1.451269732942243)
    ASSERT_GRIN_F(0.1, 1.73, 68, 0.12726642783477746)

    ASSERT_GRIN_F(1, 1.73, -68, -0.09219887148906701)
    ASSERT_GRIN_F(1, 1.73, -6.8, -0.30282878628426385)
    ASSERT_GRIN_F(1, 1.73, 6.8, -0.12733932762878003)
    ASSERT_GRIN_F(1, 1.73, 68, -6.725943680342854)
}

TEST_METHOD(solve_n2)
{
    ASSERT_GRIN_N2(0.01, 1.73, -1.45, -69.0572881420023)
    ASSERT_GRIN_N2(0.01, 1.73, -14.5, -6.897468131582403)
    ASSERT_GRIN_N2(0.01, 1.73, 14.5, 6.895639895856058)
    ASSERT_GRIN_N2(0.01, 1.73, 1.45, 68.87395033584585)

    ASSERT_GRIN_N2(0.1, 1.73, -0.145, -79.17933873121946)
    ASSERT_GRIN_N2(0.1, 1.73, -1.45, -6.98917562047311)
    ASSERT_GRIN_N2(0.1, 1.73, 1.45, 6.805876236827568)
    ASSERT_GRIN_N2(0.1, 1.73, 0.145, 60.70386205962681)

    ASSERT_GRIN_N2(1.0, 1.73, -0.0145, -2749.273150042926)
    ASSERT_GRIN_N2(1.0, 1.73, -0.145, -27.530455608631513)
    ASSERT_GRIN_N2(1.0, 1.73, 0.145, 2.7631218863278053)
    ASSERT_GRIN_N2(1.0, 1.73, 0.0145, 36.56807581929833)
}

//------------------------------------------------------------------------------

TEST_GROUP("GrinCalculator",
           ADD_TEST(calc_focus),
           ADD_TEST(solve_n2),
           )

} // namespace GrinCalculatorTests
} // namespace Tests
} // namespace Z
