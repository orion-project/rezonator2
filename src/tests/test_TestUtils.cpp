#include "TestUtils.h"

#include "../core/Elements.h"

namespace Z {
namespace Tests {
namespace TestUtilsTests {

//------------------------------------------------------------------------------

TEST_METHOD(Helper_makeElem)
{
    Element* elem = makeElem<ElemEmptyRange>("Label_1", "L = 56.7cm");
    ASSERT_IS_NOT_NULL(elem)
    ASSERT_EQ_STR(elem->type(), "ElemEmptyRange")
    ASSERT_EQ_STR(elem->label(), "Label_1")
    ASSERT_EQ_ZVALUE(elem->params().byAlias("L")->value(), 56.7_cm)
}

//------------------------------------------------------------------------------

TEST_GROUP("TestUtils",
           ADD_TEST(Helper_makeElem),
           )

} // namespace TestUtilsTests
} // namespace Tests
} // namespace Z
