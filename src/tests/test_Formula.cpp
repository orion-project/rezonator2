#include "testing/OriTestBase.h"
#include "../core/Formula.h"

namespace Z {
namespace Tests {
namespace FormulaTests {

TEST_METHOD(isValidVariableName)
{
    ASSERT_IS_TRUE(Z::FormulaUtils::isValidVariableName("a"))
    ASSERT_IS_TRUE(Z::FormulaUtils::isValidVariableName("a1"))
    ASSERT_IS_FALSE(Z::FormulaUtils::isValidVariableName("a+1"))
    ASSERT_IS_FALSE(Z::FormulaUtils::isValidVariableName("a*1"))
    ASSERT_IS_TRUE(Z::FormulaUtils::isValidVariableName("a_1"))
    ASSERT_IS_FALSE(Z::FormulaUtils::isValidVariableName("1"))
    ASSERT_IS_TRUE(Z::FormulaUtils::isValidVariableName("_1"))
}

//------------------------------------------------------------------------------

TEST_GROUP("Formula",
           ADD_TEST(isValidVariableName),
           )

} // namespace FormulaTests
} // namespace Tests
} // namespace Z
