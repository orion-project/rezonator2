#include "TestUtils.h"
#include "TestSchemaListener.h"

#include <memory>

#pragma GCC diagnostic ignored "-Wweak-vtables"
#pragma GCC diagnostic ignored "-Wpadded"

namespace Z {
namespace Tests {
namespace SchemaTests {

#define ASSERT_LISTENER(expected_elem, ...) { \
    ASSERT_IS_TRUE(listener.schema == &schema) \
    ASSERT_IS_TRUE(listener.element == expected_elem) \
    TEST_LOG(listener.eventsStr())\
    ASSERT_IS_TRUE(listener.checkEvents({__VA_ARGS__})) \
}

#define ASSERT_ELEM_COUNT(expected_count)\
    ASSERT_EQ_INT(schema.count(), expected_count)

#define TAKE_ELEM_PTR(elem_var, index)\
    std::unique_ptr<Element> auto_##elem_var(schema.element(index));\
    auto elem_var = auto_##elem_var.get();\
    Q_UNUSED(elem_var)

//------------------------------------------------------------------------------

DECLARE_ELEMENT(TestElement, Element)
    Ori::Testing::TestBase *test = nullptr;
    ~TestElement() override
    {
        if (!test) return;
        TEST_LOG("~TestElement() " + label())
        test->data().insert(label(), true);
    }
    DEFAULT_LABEL("hhh")
DECLARE_ELEMENT_END

#define PREPARE_SCHEMA_ELEMS(elem_count)\
    SCHEMA_AND_LISTENER \
    for (int i = 0; i < elem_count; i++ )\
        schema.insertElement(new TestElement, -1, false);

//------------------------------------------------------------------------------

TEST_METHOD(constructor)
{
    Schema schema;
    ASSERT_SCHEMA_STATE(STATE(New))
    ASSERT_IS_FALSE(schema.modified())
}

TEST_METHOD(destructor_must_delete_elements)
{
    auto schema = new Schema;
    for (int i = 0; i < 10; i++)
    {
        auto elem = new TestElement;
        elem->setLabel(QString::number(i));
        elem->test = test;
        schema->insertElement(elem);
    }
    delete schema;
    for (int i = 0; i < 10; i++)
        ASSERT_IS_TRUE(test->data()[QString::number(i)].toBool()) // element was deleted
}

TEST_METHOD(destructor_must_raise_event)
{
    TestSchemaListener listener;
    auto schema = new Schema;
    schema->registerListener(&listener);
    delete schema;
    ASSERT_IS_TRUE(listener.schema == schema)
    ASSERT_IS_TRUE(listener.checkEvents({EVENT(Deleted)}))
}

TEST_METHOD(destructor_with_no_events)
{
    TestSchemaListener listener;
    auto schema = new Schema;
    schema->registerListener(&listener);
    schema->events().disable();
    delete schema;
    ASSERT_IS_NULL(listener.schema)
    ASSERT_LISTENER_NO_EVENTS
}

//------------------------------------------------------------------------------

DECLARE_ELEMENT(LabeledElement1, Element) DEFAULT_LABEL("hhh") DECLARE_ELEMENT_END
DECLARE_ELEMENT(LabeledElement2, Element) DEFAULT_LABEL("ggg") DECLARE_ELEMENT_END

TEST_METHOD(generateLabel_first_elem)
{
    Schema s;
    auto e1 = new LabeledElement1;
    s.insertElement(e1);
    s.generateLabel(e1);
    ASSERT_EQ_STR(e1->label(), "hhh1");

    auto e2 = new LabeledElement2;
    s.insertElement(e2);
    s.generateLabel(e2);
    ASSERT_EQ_STR(e2->label(), "ggg1");
}

TEST_METHOD(generateLabel_next_elem)
{
    Schema s;
    s.insertElement(new LabeledElement1);
    s.insertElement(new LabeledElement2);
    s.generateLabel(s.element(0));
    s.generateLabel(s.element(1));

    LabeledElement1 e1;
    s.generateLabel(&e1);
    ASSERT_EQ_STR(e1.label(), "hhh2");

    LabeledElement2 e2;
    s.generateLabel(&e2);
    ASSERT_EQ_STR(e2.label(), "ggg2");
}

//------------------------------------------------------------------------------

#define RAISE_EVENT(event, elem)                                             \
    TEST_LOG(#event)                                                         \
    listener.reset();                                                        \
    schema.events().disable();                                               \
    schema.events().raise(SchemaEvents::event, elem);                        \
    ASSERT_IS_TRUE(listener.events.isEmpty())                                \
    schema.events().enable();                                                \
    schema.events().raise(SchemaEvents::event, elem);

TEST_METHOD(raise_all_events)
{
    TestElement e;
    SCHEMA_AND_LISTENER

    RAISE_EVENT(Deleted, nullptr)
    ASSERT_SCHEMA_STATE(STATE(New))
    ASSERT_LISTENER(nullptr, EVENT(Deleted))

    RAISE_EVENT(Changed, nullptr)
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(nullptr, EVENT(Changed))

    RAISE_EVENT(Saved, nullptr)
    ASSERT_SCHEMA_STATE(STATE(None))
    ASSERT_LISTENER(nullptr, EVENT(Saved), EVENT(Changed))

    RAISE_EVENT(Loading, nullptr)
    ASSERT_SCHEMA_STATE(STATE(Loading))
    ASSERT_LISTENER(nullptr, EVENT(Loading))

    RAISE_EVENT(Loaded, nullptr)
    ASSERT_SCHEMA_STATE(STATE(None))
    ASSERT_LISTENER(nullptr, EVENT(Loaded), EVENT(Changed))

    RAISE_EVENT(ElemCreated, &e)
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(&e, EVENT(ElemCreated), EVENT(Changed))

    SCHEMA_RESET_STATE
    RAISE_EVENT(ElemChanged, &e)
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(&e, EVENT(ElemChanged), EVENT(Changed))

    SCHEMA_RESET_STATE
    RAISE_EVENT(ElemDeleting, &e)
    ASSERT_SCHEMA_STATE(STATE(None))
    ASSERT_LISTENER(&e, EVENT(ElemDeleting))

    RAISE_EVENT(ElemDeleted, &e)
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(&e, EVENT(ElemDeleted), EVENT(Changed))

    SCHEMA_RESET_STATE
    RAISE_EVENT(ParamsChanged, nullptr)
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(nullptr, EVENT(ParamsChanged), EVENT(Changed))

    SCHEMA_RESET_STATE
    RAISE_EVENT(LambdaChanged, nullptr)
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(nullptr, EVENT(LambdaChanged), EVENT(Changed))
}

//------------------------------------------------------------------------------

TEST_METHOD(set_wavelength_must_raise_event)
{
    SCHEMA_AND_LISTENER
    schema.wavelength().setValue(Z::Value(10, Z::Units::m()));
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(nullptr, EVENT(LambdaChanged), EVENT(Changed))
}

TEST_METHOD(enabledCount)
{
    PREPARE_SCHEMA_ELEMS(10)
    ASSERT_EQ_INT(schema.enabledCount(), 10)
    schema.element(3)->setDisabled(true);
    schema.element(7)->setDisabled(true);
    ASSERT_EQ_INT(schema.enabledCount(), 8)
}

TEST_METHOD(elementById)
{
    PREPARE_SCHEMA_ELEMS(2)
    for (auto e : schema.elements())
        ASSERT_EQ_PTR(schema.elementById(e->id()), e);
}

//------------------------------------------------------------------------------

TEST_METHOD(insertElement_must_increase_count)
{
    Schema schema;
    ASSERT_ELEM_COUNT(0)
    schema.insertElement(new TestElement);
    ASSERT_ELEM_COUNT(1)
}

TEST_METHOD(insertElement_must_assign_element_owner)
{
    Schema schema;
    auto el = new TestElement;
    schema.insertElement(el);
    ASSERT_EQ_PTR(el->owner(), &schema);
}

TEST_METHOD(insertElement_must_raise_events_and_change_state)
{
    SCHEMA_AND_LISTENER
    auto el1 = new TestElement;
    schema.insertElement(el1);
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(el1, EVENT(ElemCreated), EVENT(Changed))
}

//------------------------------------------------------------------------------

TEST_METHOD(deleteElement_must_not_fail_when_invalid_elem)
{
    PREPARE_SCHEMA_ELEMS(1)

    // invalid index
    schema.deleteElement(-1);
    ASSERT_ELEM_COUNT(1)

    // invalid index
    schema.deleteElement(1);
    ASSERT_ELEM_COUNT(1)

    // invalid pointer
    TestElement nonSchemaElem;
    schema.deleteElement(&nonSchemaElem);
    ASSERT_ELEM_COUNT(1)
}

TEST_METHOD(deleteElement_must_decrease_count)
{
    PREPARE_SCHEMA_ELEMS(2)
    ASSERT_EQ_INT(schema.count(), 2)

    // by index
    TAKE_ELEM_PTR(el1, 0)
    schema.deleteElement(0, false, false);
    ASSERT_ELEM_COUNT(1)
    ASSERT_EQ_INT(schema.indexOf(el1), -1)

    // by pointer
    TAKE_ELEM_PTR(el2, 0)
    schema.deleteElement(el2, false, false);
    ASSERT_ELEM_COUNT(0)
    ASSERT_EQ_INT(schema.indexOf(el2), -1)
}

TEST_METHOD(deleteElement_must_reset_element_owner)
{
    PREPARE_SCHEMA_ELEMS(2)
    for (auto el: schema.elements())
        ASSERT_EQ_PTR(el->owner(), &schema)

    // by index
    TAKE_ELEM_PTR(el1, 0)
    schema.deleteElement(0, false, false);
    ASSERT_IS_NULL(el1->owner())

    // by pointer
    TAKE_ELEM_PTR(el2, 0)
    schema.deleteElement(el2, false, false);
    ASSERT_IS_NULL(el2->owner())
}

TEST_METHOD(deleteElement_with_lockEvents)
{
    PREPARE_SCHEMA_ELEMS(2)
    schema.events().disable();

    // by index
    TAKE_ELEM_PTR(el1, 0)
    schema.deleteElement(0, true, false);
    ASSERT_LISTENER_NO_EVENTS
    ASSERT_SCHEMA_STATE(STATE(New))

    // by pointer
    TAKE_ELEM_PTR(el2, 0)
    schema.deleteElement(el2, true, false);
    ASSERT_LISTENER_NO_EVENTS
    ASSERT_SCHEMA_STATE(STATE(New))
}

TEST_METHOD(deleteElement_with_events_false)
{
    PREPARE_SCHEMA_ELEMS(2)
    schema.events().enable();

    // by index
    TAKE_ELEM_PTR(el1, 0)
    schema.deleteElement(0, false, false);
    ASSERT_LISTENER_NO_EVENTS
    ASSERT_SCHEMA_STATE(STATE(New))

    // by pointer
    TAKE_ELEM_PTR(el2, 0)
    schema.deleteElement(el2, false, false);
    ASSERT_LISTENER_NO_EVENTS
    ASSERT_SCHEMA_STATE(STATE(New))
}

TEST_METHOD(deleteElement_by_index_with_events)
{
    PREPARE_SCHEMA_ELEMS(1)
    TAKE_ELEM_PTR(el, 0)
    schema.deleteElement(0, true, false);
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(el, EVENT(ElemDeleting), EVENT(ElemDeleted), EVENT(Changed))
}

TEST_METHOD(deleteElement_by_pointer_with_events)
{
    PREPARE_SCHEMA_ELEMS(1)
    TAKE_ELEM_PTR(el, 0)
    schema.deleteElement(el, true, false);
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(el, EVENT(ElemDeleting), EVENT(ElemDeleted), EVENT(Changed))
}

//------------------------------------------------------------------------------

DECLARE_ELEMENT(TestRange, ElementRange)
DECLARE_ELEMENT_END

DECLARE_ELEMENT(TestInterface, ElementInterface)
DECLARE_ELEMENT_END

TEST_METHOD(ElementInterface_must_be_linked_to_neighbours)
{
    Schema s;

    // [0:intf1]
    auto intf1 = new TestInterface;
    s.insertElement(intf1);
    ASSERT_EQ_DBL(intf1->ior1(), 1)
    ASSERT_EQ_DBL(intf1->ior2(), 1)

    // Left range must be linked to `n1`
    // [0:medium1][1:intf1]
    auto medium1 = new TestRange;
    medium1->paramIor()->setValue(2);
    s.insertElement(medium1, 0);
    ASSERT_EQ_INT(s.paramLinks()->size(), 1);
    ASSERT_EQ_DBL(intf1->ior1(), 2)
    ASSERT_EQ_DBL(intf1->ior2(), 1)

    // Right range must be linked to `n2`
    // [0:medium1][1:intf1][2:medium2]
    auto medium2 = new TestRange;
    medium2->paramIor()->setValue(3);
    s.insertElement(medium2);
    ASSERT_EQ_INT(s.paramLinks()->size(), 2);
    ASSERT_EQ_DBL(intf1->ior1(), 2)
    ASSERT_EQ_DBL(intf1->ior2(), 3)

    // Change of left range must change `n1`
    medium1->paramIor()->setValue(2.5);
    ASSERT_EQ_DBL(intf1->ior1(), 2.5)
    ASSERT_EQ_DBL(intf1->ior2(), 3)

    // Change of right range must change `n2`
    medium2->paramIor()->setValue(3.5);
    ASSERT_EQ_DBL(intf1->ior1(), 2.5)
    ASSERT_EQ_DBL(intf1->ior2(), 3.5)

    // Insertion of non-range element must break a link to the interface,
    // reset `n1`, and change of left range must not change `n1` anymore
    // [0:medium1][1:elem1][2:intf1][3:medium2]
    auto elem1 = new TestElement;
    s.insertElement(elem1, 1);
    ASSERT_EQ_INT(s.paramLinks()->size(), 1);
    ASSERT_EQ_DBL(intf1->ior1(), 1)
    medium1->paramIor()->setValue(2);
    ASSERT_EQ_DBL(intf1->ior1(), 1)
    ASSERT_EQ_DBL(intf1->ior2(), 3.5)

    // Insertion of non-range element must break a link to the interface,
    // reset `n2`, and change of right range must not change `n2` anymore
    // [0:medium1][1:elem1][2:intf1][3:elem2][4:medium2]
    auto elem2 = new TestElement;
    s.insertElement(elem2, 3);
    ASSERT_EQ_INT(s.paramLinks()->size(), 0);
    ASSERT_EQ_DBL(intf1->ior2(), 1)
    medium2->paramIor()->setValue(3);
    ASSERT_EQ_DBL(intf1->ior1(), 1)
    ASSERT_EQ_DBL(intf1->ior2(), 1)
}

TEST_METHOD(ElementInterface_must_be_unlinked_after_deletion_of_itself)
{
    Schema s;

    auto intf1 = new TestInterface;
    s.insertElement(intf1);

    auto medium1 = new TestRange;
    s.insertElement(medium1, 0);
    ASSERT_EQ_INT(s.paramLinks()->size(), 1);

    s.deleteElement(intf1);
    ASSERT_EQ_INT(s.paramLinks()->size(), 0);
}

TEST_METHOD(ElementInterface_must_be_unlinked_after_deletion_of_neighbour)
{
    Schema s;

    auto intf1 = new TestInterface;
    s.insertElement(intf1);

    auto medium1 = new TestRange;
    s.insertElement(medium1, 0);
    ASSERT_EQ_INT(s.paramLinks()->size(), 1);

    s.deleteElement(medium1);
    ASSERT_EQ_INT(s.paramLinks()->size(), 0);
}

//------------------------------------------------------------------------------

TEST_GROUP("Schema",
    ADD_TEST(constructor),
    ADD_TEST(destructor_must_raise_event),
    ADD_TEST(destructor_with_no_events),
    ADD_TEST(destructor_must_delete_elements),
    ADD_TEST(generateLabel_first_elem),
    ADD_TEST(generateLabel_next_elem),
    ADD_TEST(raise_all_events),
    ADD_TEST(set_wavelength_must_raise_event),
    ADD_TEST(enabledCount),
    ADD_TEST(elementById),
    ADD_TEST(insertElement_must_increase_count),
    ADD_TEST(insertElement_must_assign_element_owner),
    ADD_TEST(insertElement_must_raise_events_and_change_state),
    ADD_TEST(deleteElement_must_decrease_count),
    ADD_TEST(deleteElement_must_not_fail_when_invalid_elem),
    ADD_TEST(deleteElement_must_reset_element_owner),
    ADD_TEST(deleteElement_with_lockEvents),
    ADD_TEST(deleteElement_with_events_false),
    ADD_TEST(deleteElement_by_index_with_events),
    ADD_TEST(deleteElement_by_pointer_with_events),
    ADD_TEST(ElementInterface_must_be_linked_to_neighbours),
    ADD_TEST(ElementInterface_must_be_unlinked_after_deletion_of_itself),
    ADD_TEST(ElementInterface_must_be_unlinked_after_deletion_of_neighbour),
)

} // namespace SchemaTests
} // namespace Tests
} // namespace Z
