#include "../tests/TestSchemaListener.h"
#include "../tests/TestUtils.h"

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

#define ASSERT_ELEM_DESTRUCTED(elem_label) \
    ASSERT_IS_TRUE(test->data()[QString("elem destructor %1").arg(elem_label)].toBool())
#define ASSERT_ELEM_NOT_DESTRUCTED(elem_label) \
    ASSERT_IS_FALSE(test->data()[QString("elem destructor %1").arg(elem_label)].toBool())

//------------------------------------------------------------------------------

namespace {
DECLARE_ELEMENT(TestElement, Element)
    Ori::Testing::TestBase *test = nullptr;
    ~TestElement() override
    {
        if (!test) return;
        TEST_LOG("~TestElement() " + label())
        SET_TEST_DATA(QString("elem destructor %1").arg(label()), true);
    }
    DEFAULT_LABEL("hhh")
    void addParamPublic(Z::Parameter* p) { addParam(p); }
DECLARE_ELEMENT_END
}

#define PREPARE_SCHEMA_ELEMS(elem_count)\
    SCHEMA_AND_LISTENER \
    Elements elems;\
    for (int i = 0; i < elem_count; i++ ) {\
        auto elem = new TestElement;\
        elem->test = test;\
        elem->setLabel(QString("test_elem_%1").arg(i));\
        elems << elem;\
    }\
    schema.insertElements(elems, -1, Arg::RaiseEvents(false));

//------------------------------------------------------------------------------

TEST_METHOD(constructor)
{
    Schema schema;
    ASSERT_SCHEMA_STATE(STATE(New))
    ASSERT_IS_FALSE(schema.modified())
}

TEST_METHOD(destructor__must_delete_elements)
{
    auto schema = new Schema;
    for (int i = 0; i < 10; i++)
    {
        auto elem = new TestElement;
        elem->setLabel(QString::number(i));
        elem->test = test;
        schema->insertElements({elem}, -1, Arg::RaiseEvents(false));
    }
    delete schema;
    for (int i = 0; i < 10; i++)
        ASSERT_ELEM_DESTRUCTED(QString::number(i))
}

TEST_METHOD(destructor__must_delete_custom_params)
{
    class TestParam : public Z::Parameter
    {
    public:
        ~TestParam() { SET_TEST_DATA("custom param was deleted", true); }
        Ori::Testing::TestBase* test = nullptr;
    };

    auto param = new TestParam;
    param->test = test;

    auto schema = new Schema;
    schema->addGlobalParam(param);
    delete schema;

    ASSERT_EQ_DATA("custom param was deleted", true)
}

TEST_METHOD(destructor__must_delete_pumps)
{
    class TestPumpParams : public PumpParams
    {
    public:
        ~TestPumpParams() { SET_TEST_DATA("pump was deleted", true); }
        Ori::Testing::TestBase* test = nullptr;
    };

    auto pump = new TestPumpParams;
    pump->test = test;

    auto schema = new Schema;
    schema->pumps()->append(pump);
    delete schema;

    ASSERT_EQ_DATA("pump was deleted", true)
}

TEST_METHOD(destructor__must_delete_formulas)
{
    Z::Parameter param;

    class TestFormula : public Z::Formula
    {
    public:
        TestFormula(Z::Parameter* p) : Z::Formula(p) {}
        ~TestFormula() { SET_TEST_DATA("formula was deleted", true); }
        Ori::Testing::TestBase* test = nullptr;
    };

    auto formula = new TestFormula(&param);
    formula->test = test;

    auto schema = new Schema;
    schema->formulas()->put(formula);
    delete schema;

    ASSERT_EQ_DATA("formula was deleted", true)
}

TEST_METHOD(destructor__must_raise_event)
{
    TestSchemaListener listener;
    auto schema = new Schema;
    schema->registerListener(&listener);
    delete schema;
    ASSERT_IS_TRUE(listener.schema == schema)
    ASSERT_IS_TRUE(listener.checkEvents({EVENT(Deleted)}))
}

TEST_METHOD(destructor__can_do_without_events)
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

namespace {
DECLARE_ELEMENT(LabeledElement1, Element) DEFAULT_LABEL("hhh") DECLARE_ELEMENT_END
DECLARE_ELEMENT(LabeledElement2, Element) DEFAULT_LABEL("ggg") DECLARE_ELEMENT_END
}

TEST_METHOD(generateLabel__first_elem)
{
    Schema s;
    auto e1 = new LabeledElement1;
    s.insertElements({e1}, -1, Arg::RaiseEvents(false));
    Z::Utils::generateLabel(s.elements(), e1);
    ASSERT_EQ_STR(e1->label(), "hhh1")

    auto e2 = new LabeledElement2;
    s.insertElements({e2}, -1, Arg::RaiseEvents(false));
    Z::Utils::generateLabel(s.elements(), e2);
    ASSERT_EQ_STR(e2->label(), "ggg1")
}

TEST_METHOD(generateLabel__next_elem)
{
    Schema s;
    s.insertElements({new LabeledElement1, new LabeledElement2}, -1, Arg::RaiseEvents(false));
    Z::Utils::generateLabel(s.elements(), s.element(0));
    Z::Utils::generateLabel(s.elements(), s.element(1));

    LabeledElement1 e1;
    Z::Utils::generateLabel(s.elements(), &e1);
    ASSERT_EQ_STR(e1.label(), "hhh2")

    LabeledElement2 e2;
    Z::Utils::generateLabel(s.elements(), &e2);
    ASSERT_EQ_STR(e2.label(), "ggg2")
}

//------------------------------------------------------------------------------

#define RAISE_EVENT(event, elem)                                             \
    TEST_LOG(#event)                                                         \
    listener.reset();                                                        \
    schema.events().disable();                                               \
    schema.events().raise(SchemaEvents::event, elem, "");                    \
    ASSERT_IS_TRUE(listener.events.isEmpty())                                \
    schema.events().enable();                                                \
    schema.events().raise(SchemaEvents::event, elem, "");

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

TEST_METHOD(set_wavelength__must_raise_event)
{
    SCHEMA_AND_LISTENER
    schema.wavelength().setValue(Z::Value(10, Z::Units::m()));
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(nullptr, EVENT(LambdaChanged), EVENT(Changed), EVENT(RecalRequred))
}

//------------------------------------------------------------------------------

TEST_METHOD(Element_RequiresWavelength__must_be_respected)
{
    Schema schema;
    auto oldLambda = 5_m;
    schema.wavelength().setValue(oldLambda);

    // Good element, it requires wavelength and provides Lambda
    auto e1 = new TestElement;
    e1->setOption(Element_RequiresWavelength);
    e1->addParamPublic(new Z::Parameter(Z::Dims::linear(), "Lambda"));
    SET_PARAM_VALUE(e1, Lambda, 1_mm)

    // Good element, it requires wavelength and provides Lambda
    auto e2 = new TestElement;
    e2->setOption(Element_RequiresWavelength);
    e2->addParamPublic(new Z::Parameter(Z::Dims::linear(), "Lambda"));
    SET_PARAM_VALUE(e2, Lambda, 2_mm)

    // Provides Lambda but doesn't require wavelength
    auto e3 = new TestElement;
    e3->addParamPublic(new Z::Parameter(Z::Dims::linear(), "Lambda"));
    SET_PARAM_VALUE(e3, Lambda, 3_mm)

    // Requires wavelength but doesn't provide Lambda
    auto e4 = new TestElement;
    e4->setOption(Element_RequiresWavelength);
    e4->addParamPublic(new Z::Parameter(Z::Dims::linear(), "Lambda1"));
    SET_PARAM_VALUE(e4, Lambda1, 4_mm)

    // Requires wavelengh but Lambda is of invalid dim
    auto e5 = new TestElement;
    e5->setOption(Element_RequiresWavelength);
    e5->addParamPublic(new Z::Parameter(Z::Dims::angular(), "Lambda"));
    SET_PARAM_VALUE(e5, Lambda, 5_deg)

    ASSERT_PARAM_VALUE(e1, Lambda, 1_mm)
    ASSERT_PARAM_VALUE(e2, Lambda, 2_mm)
    ASSERT_PARAM_VALUE(e3, Lambda, 3_mm)
    ASSERT_PARAM_VALUE(e4, Lambda1, 4_mm)
    ASSERT_PARAM_VALUE(e5, Lambda, 5_deg)

    schema.insertElements({e1, e2, e3, e4, e5}, -1, Arg::RaiseEvents(false));
    ASSERT_PARAM_VALUE(e1, Lambda, oldLambda)
    ASSERT_PARAM_VALUE(e2, Lambda, oldLambda)
    ASSERT_PARAM_VALUE(e3, Lambda, 3_mm)
    ASSERT_PARAM_VALUE(e4, Lambda1, 4_mm)
    ASSERT_PARAM_VALUE(e5, Lambda, 5_deg)

    auto newLambda = 10_m;
    schema.wavelength().setValue(newLambda);
    ASSERT_PARAM_VALUE(e1, Lambda, newLambda)
    ASSERT_PARAM_VALUE(e2, Lambda, newLambda)
    ASSERT_PARAM_VALUE(e3, Lambda, 3_mm)
    ASSERT_PARAM_VALUE(e4, Lambda1, 4_mm)
    ASSERT_PARAM_VALUE(e5, Lambda, 5_deg)
}

//------------------------------------------------------------------------------

TEST_METHOD(activeCount)
{
    PREPARE_SCHEMA_ELEMS(10)
    ASSERT_EQ_INT(schema.activeCount(), 10)
    schema.element(3)->setDisabled(true);
    schema.element(7)->setDisabled(true);
    ASSERT_EQ_INT(schema.activeCount(), 8)
}

TEST_METHOD(activeElements)
{
    PREPARE_SCHEMA_ELEMS(3)
    auto elems1 = schema.activeElements();
    ASSERT_EQ_INT(elems1.size(), schema.count());
    ASSERT_EQ_PTR(elems1.at(0), schema.element(0))
    ASSERT_EQ_PTR(elems1.at(1), schema.element(1))
    ASSERT_EQ_PTR(elems1.at(2), schema.element(2))

    schema.element(1)->setDisabled(true);
    auto elems2 = schema.activeElements();
    ASSERT_EQ_INT(elems2.size(), schema.count()-1);
    ASSERT_EQ_PTR(elems2.at(0), schema.element(0))
    ASSERT_EQ_PTR(elems2.at(1), schema.element(2))
}

//------------------------------------------------------------------------------

TEST_METHOD(elementById)
{
    PREPARE_SCHEMA_ELEMS(2)
    for (auto e : schema.elements())
        ASSERT_EQ_PTR(schema.elementById(e->id()), e)
}

TEST_METHOD(elementByLabel)
{
    PREPARE_SCHEMA_ELEMS(2)
    for (auto e : schema.elements())
        ASSERT_EQ_PTR(schema.elementByLabel(e->label()), e)
}

//------------------------------------------------------------------------------

TEST_METHOD(insertElements__must_increase_count)
{
    Schema schema;
    ASSERT_ELEM_COUNT(0)
    schema.insertElements({new TestElement}, -1, Arg::RaiseEvents(false));
    ASSERT_ELEM_COUNT(1)
}

TEST_METHOD(insertElements__must_assign_element_owner)
{
    Schema schema;
    auto el = new TestElement;
    schema.insertElements({el}, -1, Arg::RaiseEvents(false));
    ASSERT_EQ_PTR(el->owner(), &schema)
}

TEST_METHOD(insertElements__must_raise_events_and_change_state)
{
    SCHEMA_AND_LISTENER
    auto el1 = new TestElement;
    schema.insertElements({el1}, -1, Arg::RaiseEvents(true));
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(el1, EVENT(ElemCreated), EVENT(Changed), EVENT(RecalRequred))
}

TEST_METHOD(insertElements__must_not_raise_events_when_they_disabled_by_param)
{
    SCHEMA_AND_LISTENER
    auto el1 = new TestElement;
    schema.insertElements({el1}, -1, Arg::RaiseEvents(false));
    ASSERT_SCHEMA_STATE(STATE(New))
    ASSERT_LISTENER_NO_EVENTS
}

TEST_METHOD(insertElements__must_raise_events_for_all_elems)
{
    SCHEMA_AND_LISTENER
    Elements elems({new TestElement, new TestElement, new TestElement});
    schema.insertElements(elems, -1, Arg::RaiseEvents(true));
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(elems[2], EVENT(ElemCreated), EVENT(Changed),
                              EVENT(ElemCreated), EVENT(Changed),
                              EVENT(ElemCreated), EVENT(Changed),
                              EVENT(RecalRequred))
    ASSERT_LISTENER_EVENT_PARAMS(elems[0], nullptr, elems[1], nullptr, elems[2], nullptr, nullptr)
}

//------------------------------------------------------------------------------

TEST_METHOD(deleteElements__must_not_fail_when_invalid_elem)
{
    PREPARE_SCHEMA_ELEMS(1)

    // invalid pointer
    TestElement nonSchemaElem;
    schema.deleteElements({&nonSchemaElem}, Arg::RaiseEvents(false), Arg::FreeElem(true));
    ASSERT_ELEM_COUNT(1)
}

TEST_METHOD(deleteElements__must_remove_element_from_schema)
{
    PREPARE_SCHEMA_ELEMS(2)
    ASSERT_EQ_INT(schema.count(), 2)

    schema.deleteElements({elems[0]}, Arg::RaiseEvents(false), Arg::FreeElem(true));
    ASSERT_ELEM_COUNT(1)
    ASSERT_EQ_INT(schema.indexOf(elems[0]), -1)
}

TEST_METHOD(deleteElements__must_reset_element_owner)
{
    PREPARE_SCHEMA_ELEMS(2)
    for (auto el: schema.elements())
        ASSERT_EQ_PTR(el->owner(), &schema)

    schema.deleteElements({elems[0]}, Arg::RaiseEvents(false), Arg::FreeElem(false));
    ASSERT_IS_NULL(elems[0]->owner())
    delete elems[0];
}

TEST_METHOD(deleteElements__must_free_element)
{
    PREPARE_SCHEMA_ELEMS(2)

    schema.deleteElements({elems[0], elems[1]}, Arg::RaiseEvents(false), Arg::FreeElem(true));
    ASSERT_ELEM_DESTRUCTED("test_elem_0")
    ASSERT_ELEM_DESTRUCTED("test_elem_1")
}

TEST_METHOD(deleteElements__must_not_free_element_when_not_required)
{
    PREPARE_SCHEMA_ELEMS(2)

    schema.deleteElements({elems[0], elems[1]}, Arg::RaiseEvents(false), Arg::FreeElem(false));
    ASSERT_ELEM_NOT_DESTRUCTED("test_elem_0")
    ASSERT_ELEM_NOT_DESTRUCTED("test_elem_1")
    delete elems[0];
    delete elems[1];
}

TEST_METHOD(deleteElements__must_raise_events)
{
    PREPARE_SCHEMA_ELEMS(3)

    schema.deleteElements({elems}, Arg::RaiseEvents(true), Arg::FreeElem(true));
    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(elems[2],
        EVENT(ElemsDeleting),
        EVENT(ElemDeleting),
        EVENT(ElemDeleting),
        EVENT(ElemDeleting),
        EVENT(ElemDeleted), EVENT(Changed),
        EVENT(ElemDeleted), EVENT(Changed),
        EVENT(ElemDeleted), EVENT(Changed),
        EVENT(ElemsDeleted),
        EVENT(RecalRequred)
        )
    ASSERT_LISTENER_EVENT_PARAMS(
        nullptr,
        elems[0],
        elems[1],
        elems[2],
        elems[0], nullptr,
        elems[1], nullptr,
        elems[2], nullptr,
        nullptr,
        nullptr
        )
}

TEST_METHOD(deleteElements__must_not_raise_events_when_they_disabled_by_param)
{
    PREPARE_SCHEMA_ELEMS(2)

    schema.deleteElements({elems}, Arg::RaiseEvents(false), Arg::FreeElem(true));
    ASSERT_LISTENER_NO_EVENTS
    ASSERT_SCHEMA_STATE(STATE(New))
}

TEST_METHOD(deleteElements__must_not_raise_events_when_they_disabled_in_schema)
{
    PREPARE_SCHEMA_ELEMS(2)
    schema.events().disable();

    schema.deleteElements({elems}, Arg::RaiseEvents(true), Arg::FreeElem(true));
    ASSERT_LISTENER_NO_EVENTS
    ASSERT_SCHEMA_STATE(STATE(New))
}

//------------------------------------------------------------------------------

namespace {
DECLARE_ELEMENT(TestRange, ElementRange)
DECLARE_ELEMENT_END

DECLARE_ELEMENT(TestInterface, Element)
    TestInterface() : Element()
    {
        ELEM_PROLOG_INTERFACE
    }
    double ior1() const { return _ior1->value().value(); }
    double ior2() const { return _ior2->value().value(); }
DECLARE_ELEMENT_END
}

TEST_METHOD(ElementInterface__must_be_linked_to_neighbours)
{
    Schema s;

    // [0:intf1]
    auto intf1 = new TestInterface;
    s.insertElements({intf1}, -1, Arg::RaiseEvents(false));
    ASSERT_EQ_DBL(intf1->ior1(), 1)
    ASSERT_EQ_DBL(intf1->ior2(), 1)

    // Left range must be linked to `n1`
    // [0:medium1][1:intf1]
    auto medium1 = new TestRange;
    medium1->paramIor()->setValue(2);
    s.insertElements({medium1}, 0, Arg::RaiseEvents(false));
    ASSERT_EQ_INT(s.paramLinks()->size(), 1)
    ASSERT_EQ_DBL(intf1->ior1(), 2)
    ASSERT_EQ_DBL(intf1->ior2(), 1)

    // Right range must be linked to `n2`
    // [0:medium1][1:intf1][2:medium2]
    auto medium2 = new TestRange;
    medium2->paramIor()->setValue(3);
    s.insertElements({medium2}, -1, Arg::RaiseEvents(false));
    ASSERT_EQ_INT(s.paramLinks()->size(), 2)
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
    s.insertElements({elem1}, 1, Arg::RaiseEvents(false));
    ASSERT_EQ_INT(s.paramLinks()->size(), 1)
    ASSERT_EQ_DBL(intf1->ior1(), 1)
    medium1->paramIor()->setValue(2);
    ASSERT_EQ_DBL(intf1->ior1(), 1)
    ASSERT_EQ_DBL(intf1->ior2(), 3.5)

    // Insertion of non-range element must break a link to the interface,
    // reset `n2`, and change of right range must not change `n2` anymore
    // [0:medium1][1:elem1][2:intf1][3:elem2][4:medium2]
    auto elem2 = new TestElement;
    s.insertElements({elem2}, 3, Arg::RaiseEvents(false));
    ASSERT_EQ_INT(s.paramLinks()->size(), 0)
    ASSERT_EQ_DBL(intf1->ior2(), 1)
    medium2->paramIor()->setValue(3);
    ASSERT_EQ_DBL(intf1->ior1(), 1)
    ASSERT_EQ_DBL(intf1->ior2(), 1)
}

TEST_METHOD(ElementInterface__must_be_unlinked_after_deletion_of_itself)
{
    Schema s;

    auto intf1 = new TestInterface;
    s.insertElements({intf1}, -1, Arg::RaiseEvents(false));

    auto medium1 = new TestRange;
    s.insertElements({medium1}, 0, Arg::RaiseEvents(false));
    ASSERT_EQ_INT(s.paramLinks()->size(), 1)

    s.deleteElements({intf1}, Arg::RaiseEvents(false), Arg::FreeElem(true));
    ASSERT_EQ_INT(s.paramLinks()->size(), 0)
}

TEST_METHOD(ElementInterface__must_be_unlinked_after_deletion_of_neighbour)
{
    Schema s;

    auto intf1 = new TestInterface;
    s.insertElements({intf1}, -1, Arg::RaiseEvents(false));

    auto medium1 = new TestRange;
    s.insertElements({medium1}, 0, Arg::RaiseEvents(false));
    ASSERT_EQ_INT(s.paramLinks()->size(), 1)

    s.deleteElements({medium1}, Arg::RaiseEvents(false), Arg::FreeElem(true));
    ASSERT_EQ_INT(s.paramLinks()->size(), 0)
}

//------------------------------------------------------------------------------

TEST_METHOD(activePump)
{
    auto p1 = PumpMode_Waist::instance()->makePump();
    auto p2 = PumpMode_Waist::instance()->makePump();

    Schema s;
    s.pumps()->append(p1);
    s.pumps()->append(p2);

    ASSERT_IS_NULL(s.activePump())

    p2->activate(true);
    ASSERT_EQ_PTR(s.activePump(), p2)
}

//------------------------------------------------------------------------------

TEST_GROUP("Schema",
    ADD_TEST(constructor),
    ADD_TEST(destructor__must_delete_elements),
    ADD_TEST(destructor__must_delete_custom_params),
    ADD_TEST(destructor__must_delete_pumps),
    ADD_TEST(destructor__must_delete_formulas),
    ADD_TEST(destructor__must_raise_event),
    ADD_TEST(destructor__can_do_without_events),
    ADD_TEST(generateLabel__first_elem),
    ADD_TEST(generateLabel__next_elem),
    ADD_TEST(raise_all_events),
    ADD_TEST(set_wavelength__must_raise_event),
    ADD_TEST(Element_RequiresWavelength__must_be_respected),
    ADD_TEST(activeCount),
    ADD_TEST(activeElements),
    ADD_TEST(elementById),
    ADD_TEST(elementByLabel),
    ADD_TEST(insertElements__must_increase_count),
    ADD_TEST(insertElements__must_assign_element_owner),
    ADD_TEST(insertElements__must_raise_events_and_change_state),
    ADD_TEST(insertElements__must_not_raise_events_when_they_disabled_by_param),
    ADD_TEST(insertElements__must_raise_events_for_all_elems),
    ADD_TEST(deleteElements__must_not_fail_when_invalid_elem),
    ADD_TEST(deleteElements__must_remove_element_from_schema),
    ADD_TEST(deleteElements__must_reset_element_owner),
    ADD_TEST(deleteElements__must_free_element),
    ADD_TEST(deleteElements__must_not_free_element_when_not_required),
    ADD_TEST(deleteElements__must_raise_events),
    ADD_TEST(deleteElements__must_not_raise_events_when_they_disabled_by_param),
    ADD_TEST(deleteElements__must_not_raise_events_when_they_disabled_in_schema),
    ADD_TEST(ElementInterface__must_be_linked_to_neighbours),
    ADD_TEST(ElementInterface__must_be_unlinked_after_deletion_of_itself),
    ADD_TEST(ElementInterface__must_be_unlinked_after_deletion_of_neighbour),
    ADD_TEST(activePump),
)

} // namespace SchemaTests
} // namespace Tests
} // namespace Z
