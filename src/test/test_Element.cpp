#include "testing/OriTestBase.h"
#include "../core/Element.h"
#include "TestUtils.h"

namespace Z {
namespace Test {
namespace ElementTests {

DECLARE_ELEMENT(TestElement, Element)
    TestElement()
    {
        addParam(new Z::Parameter(Z::Dims::linear(), "", "",  ""), 3.14, Z::Units::mkm());
    }
    bool matrixCalculated = false;
    void calcMatrixInternal() override {
        _mt.assign(11, 12, 13, 14);
        _ms.assign(21, 22, 23, 24);
        matrixCalculated = true;
    }
DECLARE_ELEMENT_END

#define RESET_MATRIX(e) e.matrixCalculated = false;
#define ASSERT_MATRIX_CALCULATED(e) ASSERT_IS_TRUE(e.matrixCalculated)
#define ASSERT_MATRIX_NOT_CALCULATED(e) ASSERT_IS_FALSE(e.matrixCalculated)

//------------------------------------------------------------------------------

class TestElemOwner : public ElementOwner
{
public:
    bool changed = false;
    Element *element = nullptr;
    void elementChanged(Element *e) override { element = e, changed = true; }
};

#define ELEMENT_AND_OWNER\
    TestElemOwner owner;\
    TestElement elem;\
    elem.setOwner(&owner);

#define ASSERT_OWNER_NOTIFYED\
    ASSERT_IS_TRUE(owner.changed)\
    ASSERT_IS_TRUE(owner.element == &elem)

#define ASSERT_OWNER_NOT_NOTIFYED\
    ASSERT_IS_FALSE(owner.changed)\
    ASSERT_IS_NULL(owner.element)

//------------------------------------------------------------------------------

TEST_METHOD(Element_ctor_generates_id)
{
    DECLARE_ELEMENT(TestElemWithId, Element) DECLARE_ELEMENT_END

    TestElemWithId e1, e2, e3;
    ASSERT_IS_TRUE(e1.id() > 0)
    ASSERT_IS_TRUE(e2.id() > e1.id())
    ASSERT_IS_TRUE(e3.id() > e2.id())
}

//------------------------------------------------------------------------------

TEST_METHOD(ElementsNamer_generateLabel)
{
    ElementsNamer::instance().reset();
    ASSERT_EQ_STR(ElementsNamer::instance().generateLabel("L"), "L1");
    ASSERT_EQ_STR(ElementsNamer::instance().generateLabel("M"), "M1");
    ASSERT_EQ_STR(ElementsNamer::instance().generateLabel("L"), "L2");
    ASSERT_EQ_STR(ElementsNamer::instance().generateLabel("M"), "M2");
}

TEST_METHOD(ElementsNamer_generateLabel_via_helper)
{
    DECLARE_ELEMENT(TestElementWithLabel, Element)
        DEFAULT_LABEL("TestElement")
    DECLARE_ELEMENT_END

    TestElementWithLabel e1, e2;
    ASSERT_IS_TRUE(e1.label().isEmpty())
    ASSERT_IS_TRUE(e2.label().isEmpty())
    ElementsNamer::instance().reset();
    Z::Utils::generateLabel(&e1);
    Z::Utils::generateLabel(&e2);
    ASSERT_EQ_STR(e1.label(), "TestElement1");
    ASSERT_EQ_STR(e2.label(), "TestElement2");
}

//------------------------------------------------------------------------------

TEST_METHOD(Element_addParam)
{
    TestElement el;
    ASSERT_EQ_INT(el.params().size(), 1)

    Z::Parameter* p = el.params().at(0);
    ASSERT_EQ_PTR(p->dim(), Z::Dims::linear())
    ASSERT_EQ_PTR(p->owner(), &el)
    ASSERT_Z_VALUE(p->value(), 3.14, Z::Units::mkm())
}

TEST_METHOD(Element_hasParams)
{
    DECLARE_ELEMENT(TestElementWithoutParams, Element) DECLARE_ELEMENT_END

    TestElementWithoutParams el1;
    ASSERT_IS_FALSE(el1.hasParams())

    TestElement el;
    ASSERT_IS_TRUE(el.hasParams())
}

//------------------------------------------------------------------------------

TEST_METHOD(Element_unlock_calculates_matrix)
{
    TestElement elem;

    elem.lock();
    ASSERT_MATRIX_NOT_CALCULATED(elem)

    elem.unlock();
    ASSERT_MATRIX_CALCULATED(elem)
}

TEST_METHOD(Element_unlock_does_not_raise_events)
{
    ELEMENT_AND_OWNER

    elem.lock();
    ASSERT_OWNER_NOT_NOTIFYED

    elem.unlock();
    ASSERT_OWNER_NOT_NOTIFYED
}

//------------------------------------------------------------------------------

TEST_METHOD(Element_setDisabled_must_set_flag)
{
    TestElement e;
    ASSERT_IS_FALSE(e.disabled());

    e.setDisabled(true);
    ASSERT_IS_TRUE(e.disabled());

    e.setDisabled(false);
    ASSERT_IS_FALSE(e.disabled());
}

TEST_METHOD(Element_setDisabled_must_be_unity_matrix)
{
    TestElement e;
    e.calcMatrix();
    ASSERT_MATRIX_CALCULATED(e)
    ASSERT_MATRIX_IS_NOT_UNITY(e.Mt())
    ASSERT_MATRIX_IS_NOT_UNITY(e.Ms())

    RESET_MATRIX(e)
    e.setDisabled(true);
    e.calcMatrix();
    ASSERT_MATRIX_NOT_CALCULATED(e)
    ASSERT_MATRIX_IS_UNITY(e.Mt())
    ASSERT_MATRIX_IS_UNITY(e.Ms())

    e.setDisabled(false);
    e.calcMatrix();
    ASSERT_MATRIX_CALCULATED(e)
    ASSERT_MATRIX_IS_NOT_UNITY(e.Mt())
    ASSERT_MATRIX_IS_NOT_UNITY(e.Ms())
}

TEST_METHOD(Element_setDisabled_must_raise_event)
{
    ELEMENT_AND_OWNER

    elem.lock();
    elem.setDisabled(true);
    ASSERT_OWNER_NOT_NOTIFYED

    elem.unlock();
    elem.setDisabled(true);
    ASSERT_OWNER_NOTIFYED
}

//------------------------------------------------------------------------------

TEST_METHOD(Element_setTitle_must_assign_value)
{
    TestElement elem;
    ASSERT_IS_TRUE(elem.title().isEmpty())

    elem.setTitle("test");
    ASSERT_EQ_STR(elem.title(), "test")
}

TEST_METHOD(Element_setTitle_must_raise_event)
{
    ELEMENT_AND_OWNER

    elem.lock();
    elem.setTitle("test");
    ASSERT_OWNER_NOT_NOTIFYED

    elem.unlock();
    elem.setTitle("test");
    ASSERT_OWNER_NOTIFYED
}

//------------------------------------------------------------------------------

TEST_METHOD(Element_setLabel_must_assign_value)
{
    TestElement elem;
    ASSERT_IS_TRUE(elem.label().isEmpty())

    elem.setLabel("test");
    ASSERT_EQ_STR(elem.label(), "test")
}

TEST_METHOD(Element_setLabel_must_raise_event)
{
    ELEMENT_AND_OWNER

    elem.lock();
    elem.setLabel("test");
    ASSERT_OWNER_NOT_NOTIFYED

    elem.unlock();
    elem.setLabel("test");
    ASSERT_OWNER_NOTIFYED
}

//------------------------------------------------------------------------------

TEST_METHOD(ElementOwner_setParam_must_recalculate_matrix)
{
    TestElement elem;
    elem.params()[0]->setValue(Z::Value(100, Z::Units::mkm()));
    ASSERT_MATRIX_CALCULATED(elem)
}

TEST_METHOD(ElementOwner_setParam_must_recalculate_matrix_when_locked)
{
    TestElement elem;
    elem.lock();
    elem.params()[0]->setValue(Z::Value(100, Z::Units::mkm()));
    ASSERT_MATRIX_CALCULATED(elem)
}

TEST_METHOD(ElementOwner_setParam_must_raise_event)
{
    ELEMENT_AND_OWNER

    elem.lock();
    elem.params()[0]->setValue(Z::Value(100, Z::Units::mkm()));
    ASSERT_OWNER_NOT_NOTIFYED

    elem.unlock();
    elem.params()[0]->setValue(Z::Value(150, Z::Units::mm()));
    ASSERT_OWNER_NOTIFYED
}

//------------------------------------------------------------------------------

TEST_GROUP("Element",
    ADD_TEST(Element_ctor_generates_id),

    ADD_TEST(ElementsNamer_generateLabel),
    ADD_TEST(ElementsNamer_generateLabel_via_helper),

    ADD_TEST(Element_addParam),
    ADD_TEST(Element_hasParams),

    ADD_TEST(Element_unlock_calculates_matrix),
    ADD_TEST(Element_unlock_does_not_raise_events),

    ADD_TEST(Element_setDisabled_must_set_flag),
    ADD_TEST(Element_setDisabled_must_be_unity_matrix),
    ADD_TEST(Element_setDisabled_must_raise_event),

    ADD_TEST(Element_setTitle_must_assign_value),
    ADD_TEST(Element_setTitle_must_raise_event),

    ADD_TEST(Element_setLabel_must_assign_value),
    ADD_TEST(Element_setLabel_must_raise_event),

    ADD_TEST(ElementOwner_setParam_must_recalculate_matrix),
    ADD_TEST(ElementOwner_setParam_must_recalculate_matrix_when_locked),
    ADD_TEST(ElementOwner_setParam_must_raise_event),
)

} // namespace ElementTests
} // namespace Test
} // namespace Z

