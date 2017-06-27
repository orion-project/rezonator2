#include "testing/OriTestBase.h"
#include "../core/Elements.h"
#include "../core/ElementFilter.h"

#include <QSharedPointer>

namespace Z {
namespace Test {
namespace ElementFilterTests {

TEST_METHOD(ElementFilterEnabled_check)
{
    ElementFilterEnabled filter;

    ElemFlatMirror elem;
    ASSERT_IS_TRUE(filter.check(&elem))

    elem.setDisabled(true);
    ASSERT_IS_FALSE(filter.check(&elem))
}

TEST_METHOD(ElementFilterHasParams_check)
{
    ElementFilterHasParams filter;

    ElemFlatMirror elem1;
    ASSERT_IS_FALSE(filter.check(&elem1))

    ElemCurveMirror elem2;
    ASSERT_IS_TRUE(filter.check(&elem2))
}

TEST_METHOD(ElementFilterHasVisibleParams_check)
{
    ElementFilterHasVisibleParams filter;

    ElemEmptyRange elem;
    ASSERT_IS_TRUE(filter.check(&elem))

    elem.params().first()->setVisible(false);
    ASSERT_IS_FALSE(filter.check(&elem))
}

TEST_METHOD(ElementFilterIsRange_check)
{
    ElementFilterIsRange filter;

    ElemFlatMirror elem1;
    ASSERT_IS_FALSE(filter.check(&elem1))

    ElemEmptyRange elem2;
    ASSERT_IS_TRUE(filter.check(&elem2))
}

//------------------------------------------------------------------------------

TEST_GROUP("Element Filter",
   ADD_TEST(ElementFilterEnabled_check),
   ADD_TEST(ElementFilterHasParams_check),
   ADD_TEST(ElementFilterHasVisibleParams_check),
   ADD_TEST(ElementFilterIsRange_check)
)

} // namespace ElementFilterTests
} // namespace Test
} // namespace Z


