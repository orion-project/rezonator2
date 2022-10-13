#include "testing/OriTestBase.h"
#include "../funcs/GrinCalculator.h"

#include <cmath>

// Test values are produced by script $PROJECT/calc/grin_focus_assessment.ipynb

namespace Z {
namespace Tests {
namespace GrinCalculatorTests {

TEST_METHOD(calc_focus)
{
//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(0.01, 1.73, 680.587611522994), 0.145, 1e-16)
//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(0.01, 1.73, 68.8739721494325), 1.45, 1e-15)
//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(0.01, 1.73, 6.89563539716427), 14.5, 1e-14)
//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(0.01, 1.73, 0.689646008267344), 145.0, 1e-12)

//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(0.1, 1.73, 60.7038644668408), 0.145, 1e-16)
//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(0.1, 1.73, 6.80587611522994), 1.45, 1e-15)
//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(0.1, 1.73, 0.688739721494325), 14.5, 1e-14)
//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(0.1, 1.73, 0.0689563539716427), 145.0, 1e-13)

//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(1.0, 1.73, 2.76312188867699), 0.145, 1e-15)
//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(1.0, 1.73, 0.607038644668408), 1.45, 1e-15)
//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(1.0, 1.73, 0.0680587611522994), 14.5, 1e-14)
//    ASSERT_NEAR_DBL(GrinCalculator::calc_focus(1.0, 1.73, 0.00688739721494325), 145.0, 1e-15)
}

TEST_METHOD(solve_n2)
{
//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(0.01, 0.145, 1.73), 680.587611522994, 1e-2)
//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(0.01, 1.45, 1.73), 68.8739721494325, 1e-4)
//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(0.01, 14.5, 1.73), 6.89563539716427, 1e-5)
//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(0.01, 145.0, 1.73), 0.689646008267344, 1e-6)

//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(0.1, 0.145, 1.73), 60.7038644668408, 1e-5)
//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(0.1, 1.45, 1.73), 6.80587611522994, 1e-6)
//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(0.1, 14.5, 1.73), 0.688739721494325, 1e-7)
//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(0.1, 145.0, 1.73), 0.0689563539716427, 1e-8)

//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(1.0, 0.145, 1.73), 2.76312188867699, 1e-8)
//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(1.0, 1.45, 1.73), 0.607038644668408, 1e-9)
//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(1.0, 14.5, 1.73), 0.0680587611522994, 1e-9)
//    ASSERT_NEAR_DBL(GrinCalculator::solve_n2(1.0, 145.0, 1.73), 0.00688739721494325, 1e-9)
}

//------------------------------------------------------------------------------

TEST_GROUP("GrinCalculator",
           ADD_TEST(calc_focus),
           ADD_TEST(solve_n2),
           )

} // namespace GrinCalculatorTests
} // namespace Tests
} // namespace Z
