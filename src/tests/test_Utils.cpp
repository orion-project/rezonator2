#include "testing/OriTestBase.h"
#include "../core/Utils.h"

namespace Z {
namespace Tests {
namespace UtilsTests {


TEST_METHOD(generateLabel)
{
    ASSERT_EQ_STR(Z::Utils::generateLabel("a", {}), "a1");
    ASSERT_EQ_STR(Z::Utils::generateLabel("a", {"a"}), "a1");
    ASSERT_EQ_STR(Z::Utils::generateLabel("a", {"a1"}), "a2");
    ASSERT_EQ_STR(Z::Utils::generateLabel("a", {"a1", "b1"}), "a2");
    ASSERT_EQ_STR(Z::Utils::generateLabel("a", {"a1", "a2"}), "a3");
    ASSERT_EQ_STR(Z::Utils::generateLabel("a", {"a100", "a2"}), "a101");
}

//------------------------------------------------------------------------------

TEST_GROUP("Utils",
           ADD_TEST(generateLabel),
           )

} // namespace FormulaTests
} // namespace Tests
} // namespace Z
