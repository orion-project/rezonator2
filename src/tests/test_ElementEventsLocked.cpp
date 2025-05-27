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
        auto p = new Z::Parameter(Z::Dims::linear(), "");
        p->setValue(3.14_mkm);
        addParam(p);
    }
DECLARE_ELEMENT_END
}

#define SCHEMA_AND_LISTENER_AND_ELEM \
    SCHEMA_AND_LISTENER \
    auto elem = new TestElement; \
    schema.insertElements({elem}, 0, Arg::RaiseEvents(false)); \
    ASSERT_SCHEMA_STATE(STATE(New)) \
    ASSERT_IS_FALSE(schema.modified());

TEST_METHOD(schema_modified_when_not_locked)
{
    SCHEMA_AND_LISTENER_AND_ELEM

    elem->params().byIndex(0)->setValue(1_m);
    ASSERT_IS_TRUE(schema.modified());
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER_EVENTS(EVENT(ElemChanged), EVENT(Changed))
}

TEST_METHOD(schema_not_modified_when_locked)
{
    SCHEMA_AND_LISTENER_AND_ELEM

    ElementEventsLocker locker(elem);

    elem->params().byIndex(0)->setValue(1_m);
    ASSERT_IS_FALSE(schema.modified());
    ASSERT_SCHEMA_STATE(STATE(New))
    ASSERT_LISTENER_NO_EVENTS
}

TEST_METHOD(schema_modified_when_link_added_and_not_locked)
{
    SCHEMA_AND_LISTENER_AND_ELEM

    auto p0 = new Z::Parameter(Z::Dims::linear(), "");
    schema.addCustomParam(p0);
    schema.paramLinks()->append(new ParameterLink(p0, elem->params().byIndex(0)));

    ASSERT_IS_TRUE(schema.modified());
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER_EVENTS(EVENT(ElemChanged), EVENT(Changed))
}

TEST_METHOD(schema_not_modified_when_link_added_and_locked)
{
    SCHEMA_AND_LISTENER_AND_ELEM

    ElementEventsLocker locker(elem);
    
    auto p0 = new Z::Parameter(Z::Dims::linear(), "");
    schema.addCustomParam(p0);
    schema.paramLinks()->append(new ParameterLink(p0, elem->params().byIndex(0)));

    ASSERT_IS_FALSE(schema.modified());
    ASSERT_SCHEMA_STATE(STATE(New))
    ASSERT_LISTENER_NO_EVENTS
}

TEST_METHOD(schema_modified_when_linked_parameter_changed_and_not_locked)
{
    SCHEMA_AND_LISTENER_AND_ELEM

    auto p0 = new Z::Parameter(Z::Dims::linear(), "");
    {
        ElementEventsLocker locker(elem);
        schema.addCustomParam(p0);
        schema.paramLinks()->append(new ParameterLink(p0, elem->params().byIndex(0)));
    }

    p0->setValue(1_m);
    ASSERT_IS_TRUE(schema.modified());
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER_EVENTS(EVENT(ElemChanged), EVENT(Changed))
}

TEST_METHOD(schema_not_modified_when_linked_parameter_changed_and_locked)
{
    SCHEMA_AND_LISTENER_AND_ELEM

    auto p0 = new Z::Parameter(Z::Dims::linear(), "");

    {
        ElementEventsLocker locker(elem);
        schema.addCustomParam(p0);
        schema.paramLinks()->append(new ParameterLink(p0, elem->params().byIndex(0)));
    }

    ElementEventsLocker locker(p0);

    p0->setValue(1_m);
    ASSERT_IS_FALSE(schema.modified());
    ASSERT_SCHEMA_STATE(STATE(New))
    ASSERT_LISTENER_NO_EVENTS
}

TEST_METHOD(schema_modified_when_formula_parameter_changed_and_not_locked)
{
    SCHEMA_AND_LISTENER_AND_ELEM

    auto p0 = new Z::Parameter(Z::Dims::linear(), "");
    auto p1 = new Z::Parameter(Z::Dims::linear(), "p1");
    auto p2 = new Z::Parameter(Z::Dims::linear(), "p2");

    {
        ElementEventsLocker locker(elem);
        schema.addCustomParam(p0);
        schema.addCustomParam(p1);
        schema.addCustomParam(p2);
        schema.addParamLink(p0, elem->params().byIndex(0));
        
        auto f = new Z::Formula(p0);
        f->setCode("p1+p2");
        f->addDep(p1);
        f->addDep(p2);
        schema.formulas()->put(f);
    }

    p1->setValue(1_m);
    ASSERT_IS_TRUE(schema.modified());
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER_EVENTS(EVENT(ElemChanged), EVENT(Changed))
}

TEST_METHOD(schema_not_modified_when_formula_parameter_changed_and_locked)
{
    SCHEMA_AND_LISTENER_AND_ELEM

    auto p0 = new Z::Parameter(Z::Dims::linear(), "");
    auto p1 = new Z::Parameter(Z::Dims::linear(), "p1");
    auto p2 = new Z::Parameter(Z::Dims::linear(), "p2");

    {
        ElementEventsLocker locker(elem);
        schema.addCustomParam(p0);
        schema.addCustomParam(p1);
        schema.addCustomParam(p2);
        schema.addParamLink(p0, elem->params().byIndex(0));

        auto f = new Z::Formula(p0);
        f->setCode("p1+p2");
        f->addDep(p1);
        f->addDep(p2);
        schema.formulas()->put(f);
    }
    {
        ElementEventsLocker locker(p1);
        p1->setValue(1_m);
        ASSERT_IS_FALSE(schema.modified());
        ASSERT_SCHEMA_STATE(STATE(New))
        ASSERT_LISTENER_NO_EVENTS
    }
    {
        ElementEventsLocker locker(p2);
        p2->setValue(2_m);
        ASSERT_IS_FALSE(schema.modified());
        ASSERT_SCHEMA_STATE(STATE(New))
        ASSERT_LISTENER_NO_EVENTS
    }
}

//------------------------------------------------------------------------------

TEST_GROUP("ElementEventsLocker",
    ADD_TEST(schema_modified_when_not_locked),
    ADD_TEST(schema_not_modified_when_locked),
    ADD_TEST(schema_modified_when_link_added_and_not_locked),
    ADD_TEST(schema_not_modified_when_link_added_and_locked),
    ADD_TEST(schema_modified_when_linked_parameter_changed_and_not_locked),
    ADD_TEST(schema_not_modified_when_linked_parameter_changed_and_locked),
    ADD_TEST(schema_modified_when_formula_parameter_changed_and_not_locked),
    ADD_TEST(schema_not_modified_when_formula_parameter_changed_and_locked),
)

} // namespace ElementEventsLockerTests
} // namespace Tests
} // namespace Z

