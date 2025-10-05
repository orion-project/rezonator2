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
    schema.addGlobalParam(p0);
    {
        ElementEventsLocker locker(e1, "");
        schema.addParamLink(p0, e1->param("p1"));
    }
    
    AdjusterTester adjuster(&schema, p0);
    ASSERT_SCHEMA_STATE(STATE(New))
    
    adjuster.setValue(10);
    
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER_EVENTS(
        EVENT(GlobalParamChanged),
        EVENT(Changed),

        EVENT(ElemChanged),
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
    schema.addGlobalParam(p0);
    {
        ElementEventsLocker locker1(e1, "");
        ElementEventsLocker locker2(e2, "");
        schema.addParamLink(p0, e1->param("p1"));
        schema.addParamLink(p0, e2->param("p1"));
    }

    AdjusterTester adjuster(&schema, p0);
    ASSERT_SCHEMA_STATE(STATE(New))

    adjuster.setValue(10);

    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER_EVENTS(
        EVENT(GlobalParamChanged),
        EVENT(Changed),

        EVENT(ElemChanged), // e1 changed
        EVENT(Changed),

        EVENT(ElemChanged), // e2 changed
        EVENT(Changed),

        EVENT(RecalRequred)
        )
}

TEST_METHOD(must_be_disabled_when_param_is_linked_to_custom)
{
    Schema schema;
    auto e1 = new TestElement;
    schema.insertElements({ e1 }, 0, Arg::RaiseEvents(false));
    
    AdjusterTester adjuster(&schema, e1->param("p1"));
    ASSERT_IS_FALSE(adjuster.target->isReadOnly());
    
    auto p0 = new Z::Parameter(Z::Dims::linear(), "");
    schema.addGlobalParam(p0);
    schema.addParamLink(p0, e1->param("p1"));
    ASSERT_IS_TRUE(adjuster.target->isReadOnly());
}

TEST_METHOD(must_be_disabled_when_custom_param_is_driven_by_formula)
{
    Schema schema;
    auto p0 = new Z::Parameter(Z::Dims::linear(), "");
    schema.addGlobalParam(p0);

    AdjusterTester adjuster(&schema, p0);
    ASSERT_IS_FALSE(adjuster.target->isReadOnly());
    
    auto f = new Z::Formula(p0);
    schema.formulas()->put(f);
    f->setCode("2+2");
    f->calculate();
    ASSERT_IS_TRUE(f->ok());
    
    ASSERT_IS_TRUE(adjuster.target->isReadOnly());
}

//------------------------------------------------------------------------------

TEST_GROUP("Adjuster",
    ADD_GUI_TEST(must_raise_events_when_elem_param_chenged),
    ADD_GUI_TEST(must_raise_events_when_custom_param_changed_1),
    ADD_GUI_TEST(must_raise_events_when_custom_param_changed_2),
    ADD_GUI_TEST(must_be_disabled_when_param_is_linked_to_custom),
    ADD_GUI_TEST(must_be_disabled_when_custom_param_is_driven_by_formula),
)

} // namespace AdjusterTests
} // namespace Tests
} // namespace Z
