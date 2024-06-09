#include "../core/Element.h"
#include "../core/Schema.h"
#include "../tests/TestSchemaListener.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {
namespace ElementEventsLockerTests {

namespace {
DECLARE_ELEMENT(TestElement, Element)
    TestElement()
    {
        auto p = new Z::Parameter(Z::Dims::linear(), "", "",  "");
        p->setValue(3.14_mkm);
        addParam(p);
    }
DECLARE_ELEMENT_END
}

TEST_METHOD(schema_modified_when_not_locked)
{
    SCHEMA_AND_LISTENER

    auto elem = new TestElement;
    schema.insertElements({elem}, 0, Arg::RaiseEvents(false));

    ASSERT_IS_FALSE(schema.modified());

    elem->params().byIndex(0)->setValue(1_m);
    ASSERT_IS_TRUE(schema.modified());
}

TEST_METHOD(schema_not_modified_when_locked)
{
    SCHEMA_AND_LISTENER

    auto elem = new TestElement;
    schema.insertElements({elem}, 0, Arg::RaiseEvents(false));

    ASSERT_IS_FALSE(schema.modified());

    ElementEventsLocker locker(elem);

    elem->params().byIndex(0)->setValue(1_m);
    ASSERT_IS_FALSE(schema.modified());
}

//------------------------------------------------------------------------------

TEST_GROUP("ElementEventsLocker",
    ADD_TEST(schema_modified_when_not_locked),
    ADD_TEST(schema_not_modified_when_locked),
)

} // namespace ElementEventsLockerTests
} // namespace Tests
} // namespace Z

