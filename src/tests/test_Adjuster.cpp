#include "../tests/TestSchemaListener.h"
#include "../windows/AdjustmentWindow.h"

#include "testing/OriTestBase.h"

// Access to private members of AdjusterWidget
class AdjusterTester
{
public:
    AdjusterTester(Schema* schema, Z::Parameter* param) {
        target.reset(new AdjusterWidget(schema, param));
    }
    
    void setValue(double value) {
        target->_currentValue = value;
        target->changeValue();
    }
    
    std::shared_ptr<AdjusterWidget> target;
};

namespace Z {
namespace Tests {
namespace AdjusterTests {

namespace {
DECLARE_ELEMENT(TestElement, Element)
    TestElement() : Element() {
        addParam(new Z::Parameter(Z::Dims::linear(), "p1"));
    }
DECLARE_ELEMENT_END
}

TEST_METHOD(must_raise_events_when_elem_param_chenged)
{
    SCHEMA_AND_LISTENER
    auto e1 = new TestElement;
    schema.insertElements({ e1 }, 0, Arg::RaiseEvents(false));
    AdjusterTester adjuster(&schema, e1->param("p1"));
    ASSERT_SCHEMA_STATE(STATE(New))
    adjuster.setValue(10);
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER_EVENTS(
        EVENT(ElemChanged),
        EVENT(Changed),
        EVENT(RecalRequred)
        )
}

TEST_METHOD(must_raise_events_when_custom_param_changed_1)
{
    SCHEMA_AND_LISTENER
    
    auto e1 = new TestElement;
    auto p0 = new Z::Parameter(Z::Dims::linear(), "");
    schema.insertElements({ e1 }, 0, Arg::RaiseEvents(false));
    schema.addCustomParam(p0);
    {
        ElementEventsLocker locker(e1);
        schema.addParamLink(p0, e1->param("p1"));
    }
    
    AdjusterTester adjuster(&schema, p0);
    ASSERT_SCHEMA_STATE(STATE(New))
    
    adjuster.setValue(10);
    
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER_EVENTS(
        EVENT(ElemChanged),
        EVENT(Changed),

        EVENT(CustomParamChanged),
        EVENT(Changed),

        EVENT(RecalRequred)
        )
}

TEST_METHOD(must_raise_events_when_custom_param_changed_2)
{
    SCHEMA_AND_LISTENER

    auto e1 = new TestElement;
    auto e2 = new TestElement;
    auto p0 = new Z::Parameter(Z::Dims::linear(), "");
    schema.insertElements({ e1, e2 }, 0, Arg::RaiseEvents(false));
    schema.addCustomParam(p0);
    {
        ElementEventsLocker locker1(e1);
        ElementEventsLocker locker2(e2);
        schema.addParamLink(p0, e1->param("p1"));
        schema.addParamLink(p0, e2->param("p1"));
    }

    AdjusterTester adjuster(&schema, p0);
    ASSERT_SCHEMA_STATE(STATE(New))

    adjuster.setValue(10);

    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER_EVENTS(
        EVENT(ElemChanged), // e1 changed
        EVENT(Changed),

        EVENT(ElemChanged), // e2 changed
        EVENT(Changed),

        EVENT(CustomParamChanged),
        EVENT(Changed),

        EVENT(RecalRequred)
        )
}

//------------------------------------------------------------------------------

TEST_GROUP("Adjuster",
    ADD_GUI_TEST(must_raise_events_when_elem_param_chenged),
    ADD_GUI_TEST(must_raise_events_when_custom_param_changed_1),
    ADD_GUI_TEST(must_raise_events_when_custom_param_changed_2),
)

} // namespace AdjusterTests
} // namespace Tests
} // namespace Z
