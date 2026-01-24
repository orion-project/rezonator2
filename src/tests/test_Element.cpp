#include "../core/Element.h"
#include "../tests/TestUtils.h"

#include "testing/OriTestBase.h"

#ifndef Q_CC_MSVC
#pragma GCC diagnostic ignored "-Wpadded"
#endif

namespace Z {
namespace Tests {
namespace ElementTests {

namespace {
DECLARE_ELEMENT(TestElement, Element)
    TestElement(bool empty = false)
    {
        if (!empty) 
        {
            auto p = new Z::Parameter(Z::Dims::linear(), "", "",  "");
            p->setValue(3.14_mkm);
            addParam(p);
        }
    }

    bool matrixCalculated = false;
    void calcMatrixInternal() override {
        _matrs[MatrixKind::T].assign(11, 12, 13, 14);
        _matrs[MatrixKind::S].assign(21, 22, 23, 24);
        matrixCalculated = true;
    }

    Z::ParameterBase* changedParam = nullptr;
    void parameterChanged(ParameterBase *p) override {
        Element::parameterChanged(p);
        changedParam = p;
    }
DECLARE_ELEMENT_END
}

#define RESET_MATRIX(e) e.matrixCalculated = false;
#define ASSERT_MATRIX_CALCULATED(e) ASSERT_IS_TRUE(e.matrixCalculated)
#define ASSERT_MATRIX_NOT_CALCULATED(e) ASSERT_IS_FALSE(e.matrixCalculated)

//------------------------------------------------------------------------------

namespace {
class TestElemOwner : public ElementOwner
{
public:
    bool changed = false;
    Element *element = nullptr;
    void elementChanged(Element *e, Z::ParameterBase *p, const QString&) override { element = e; changed = true; }
};
}

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

TEST_METHOD(Element_must_add_param_in_ctor)
{
    TestElement el;
    ASSERT_EQ_INT(el.params().size(), 1)

    Z::Parameter* p = el.params().at(0);
    ASSERT_EQ_PTR(p->dim(), Z::Dims::linear())
    ASSERT_EQ_ZVALUE(p->value(), 3.14_mkm)
}

TEST_METHOD(Element_must_listen_its_params)
{
    TestElement el;
    ASSERT_IS_NULL(el.changedParam)

    el.params().at(0)->setValue(1_mm);
    ASSERT_EQ_PTR(el.changedParam, el.params().at(0))
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

TEST_METHOD(Element_unlock_does_not_calculates_matrix)
{
    TestElement elem;

    {
        ElementEventsLocker locker(&elem, "");
        ASSERT_MATRIX_NOT_CALCULATED(elem)
    }

    ASSERT_MATRIX_NOT_CALCULATED(elem)
}

TEST_METHOD(Element_unlock_does_not_raise_events)
{
    ELEMENT_AND_OWNER

    {
        ElementEventsLocker locker(&elem, "");
        ASSERT_OWNER_NOT_NOTIFYED
    }

    ASSERT_OWNER_NOT_NOTIFYED
}

//------------------------------------------------------------------------------

TEST_METHOD(Element_setDisabled_must_set_flag)
{
    TestElement e;
    ASSERT_IS_FALSE(e.disabled())

    e.setDisabled(true);
    ASSERT_IS_TRUE(e.disabled())

    e.setDisabled(false);
    ASSERT_IS_FALSE(e.disabled())
}

TEST_METHOD(Element_setDisabled_must_raise_event)
{
    ELEMENT_AND_OWNER

    {
        ElementEventsLocker locker(&elem, "");
        elem.setDisabled(true);
        ASSERT_OWNER_NOT_NOTIFYED
    }

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

    {
        ElementEventsLocker locker(&elem, "");
        elem.setTitle("test");
        ASSERT_OWNER_NOT_NOTIFYED
    }

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

    {
        ElementEventsLocker locker(&elem, "");
        elem.setLabel("test");
        ASSERT_OWNER_NOT_NOTIFYED
    }

    elem.setLabel("test");
    ASSERT_OWNER_NOTIFYED
}

//------------------------------------------------------------------------------

TEST_METHOD(ElementOwner_setParam_must_recalculate_matrix)
{
    TestElement elem;
    elem.params()[0]->setValue(100_mkm);
    ASSERT_MATRIX_CALCULATED(elem)
}

TEST_METHOD(ElementOwner_setParam_must_recalculate_matrix_when_locked)
{
    TestElement elem;
    ElementEventsLocker locker(&elem, "");
    elem.params()[0]->setValue(100_mkm);
    ASSERT_MATRIX_CALCULATED(elem)
}

TEST_METHOD(ElementOwner_setParam_must_raise_event)
{
    ELEMENT_AND_OWNER

    {
        ElementEventsLocker locker(&elem, "");
        elem.params()[0]->setValue(100_mkm);
        ASSERT_OWNER_NOT_NOTIFYED
    }

    elem.params()[0]->setValue(150_mm);
    ASSERT_OWNER_NOTIFYED
}

//------------------------------------------------------------------------------

class TestParam : public Z::Parameter
{
public:
    TestParam(Ori::Testing::TestBase *test, const QString& name): Z::Parameter(Z::Dims::none(), name), test(test) {}
    ~TestParam() override {
        TEST_LOG("~TestParam() " + alias())
        SET_TEST_DATA(QString("param destructor %1").arg(alias()), true);
    }
    Ori::Testing::TestBase *test;
};

#define ADD_PARAM(name, value) \
    auto p_##name = new TestParam(test, #name);\
    p_##name->setValue(value);\
    elem.addParam(p_##name);

#define ASSERT_PARAMS(expected_names) {\
    QStringList strs; \
    for (auto p : elem.params()) strs << p->alias();\
    QString existed_names = strs.join(", ");\
    ASSERT_EQ_STR(existed_names, expected_names)\
}

TEST_METHOD(Element_addParam__must_insert_at_index)
{
    TestElement elem(true);

    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    ASSERT_PARAMS("a, b")
    
    auto p_c = new TestParam(test, "c");
    elem.addParam(p_c, 1);
    ASSERT_PARAMS("a, c, b")
}

TEST_METHOD(Element_addParam__must_listen_new_param)
{
    TestElement elem(true);
    ASSERT_IS_NULL(elem.changedParam)

    ADD_PARAM(a, 1)
    ASSERT_IS_NULL(elem.changedParam)

    p_a->setValue(1_mm);
    ASSERT_EQ_PTR(elem.changedParam, p_a)
}

TEST_METHOD(Element_addParam__must_do_nothing_if_already_added)
{
    TestElement elem(true);

    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    ASSERT_PARAMS("a, b")

    elem.addParam(p_a);
    ASSERT_PARAMS("a, b")
}

TEST_METHOD(Element_removeParam__must_unlisten)
{
    TestElement elem(true);
    ADD_PARAM(a, 1)

    elem.removeParam(p_a, false);
    p_a->setValue(2_m);
    
    ASSERT_IS_NULL(elem.changedParam)
    delete p_a;
}

TEST_METHOD(Element_removeParam__must_destruct)
{
    TestElement elem(true);

    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    ASSERT_PARAMS("a, b")

    elem.removeParam(p_a, true);
    ASSERT_IS_TRUE(test->data("param destructor a").toBool())
    ASSERT_PARAMS("b")
}

TEST_METHOD(Element_removeParam__does_nothing_if_not_added)
{
    TestElement elem(true);
    std::shared_ptr<Z::Parameter> p_a(new TestParam(test, "a"));
    elem.removeParam(p_a.get(), true);
    ASSERT_IS_FALSE(test->data("param destructor a").toBool())
}

TEST_METHOD(Element_moveParamUp)
{
    TestElement elem(true);
    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    ADD_PARAM(c, 3)
    ADD_PARAM(d, 4)

    elem.moveParamUp(p_b);
    ASSERT_PARAMS("b, a, c, d")

    elem.moveParamUp(p_b);
    ASSERT_PARAMS("a, c, d, b")

    elem.moveParamUp(p_b);
    ASSERT_PARAMS("a, c, b, d")
}

TEST_METHOD(Element_moveParamDown)
{
    TestElement elem(true);
    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    ADD_PARAM(c, 3)
    ADD_PARAM(d, 4)

    elem.moveParamDown(p_c);
    ASSERT_PARAMS("a, b, d, c")

    elem.moveParamDown(p_c);
    ASSERT_PARAMS("c, a, b, d")

    elem.moveParamDown(p_c);
    ASSERT_PARAMS("a, c, b, d")
}

//------------------------------------------------------------------------------

DECLARE_ELEMENT(TestElementRange, Element)
    TestElementRange() { ELEM_PROLOG_RANGE }
DECLARE_ELEMENT_END

DECLARE_ELEMENT(TestElementDynamic, Element)
    TestElementDynamic() { ELEM_PROLOG_DYNAMIC }
DECLARE_ELEMENT_END

DECLARE_ELEMENT(TestElementInterface, Element)
    TestElementInterface() { ELEM_PROLOG_INTERFACE }
DECLARE_ELEMENT_END

#define ASSERT_BASE_MATRICES\
    ASSERT_IS_TRUE(elem.hasMatrix(MatrixKind::T))\
    ASSERT_IS_TRUE(elem.hasMatrix(MatrixKind::S))\
    ASSERT_IS_TRUE(elem.hasMatrix(MatrixKind::InvT))\
    ASSERT_IS_TRUE(elem.hasMatrix(MatrixKind::InvS))\
    ASSERT_EQ_PTR(&elem.Mt(), &elem.M(MatrixKind::T))\
    ASSERT_EQ_PTR(&elem.Ms(), &elem.M(MatrixKind::S))\
    ASSERT_EQ_PTR(&elem.Mt_inv(), &elem.M(MatrixKind::InvT))\
    ASSERT_EQ_PTR(&elem.Ms_inv(), &elem.M(MatrixKind::InvS))\
    ASSERT_EQ_PTR(elem.pMt(), elem.pM(MatrixKind::T))\
    ASSERT_EQ_PTR(elem.pMs(), elem.pM(MatrixKind::S))\
    ASSERT_EQ_PTR(elem.pMt_inv(), elem.pM(MatrixKind::InvT))\
    ASSERT_EQ_PTR(elem.pMs_inv(), elem.pM(MatrixKind::InvS))

TEST_METHOD(ElementKind__Simple)
{
    TestElement elem(true);
    ASSERT_IS_TRUE(elem.kind() == ElementKind::Simple)
    ASSERT_BASE_MATRICES
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::T1))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::S1))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::T2))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::S2))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::DynT))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::DynS))
    ASSERT_IS_NULL(elem.param("L"))
    ASSERT_IS_NULL(elem.param("n"))
    ASSERT_IS_NULL(elem.param("n1"))
    ASSERT_IS_NULL(elem.param("n2"))
    ASSERT_IS_FALSE(elem.isRange())
    ASSERT_IS_FALSE(elem.asRange().has_value())
    ASSERT_IS_FALSE(elem.isDynamic())
    ASSERT_IS_FALSE(elem.asDynamic().has_value())
    ASSERT_IS_FALSE(elem.isInterface())
    ASSERT_IS_FALSE(elem.asInterface().has_value())
}

TEST_METHOD(ElementKind__Range)
{
    TestElementRange elem;
    ASSERT_IS_TRUE(elem.kind() == ElementKind::Range)
    ASSERT_BASE_MATRICES
    ASSERT_IS_TRUE(elem.hasMatrix(MatrixKind::T1))
    ASSERT_IS_TRUE(elem.hasMatrix(MatrixKind::S1))
    ASSERT_IS_TRUE(elem.hasMatrix(MatrixKind::T2))
    ASSERT_IS_TRUE(elem.hasMatrix(MatrixKind::S2))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::DynT))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::DynS))
    ASSERT_IS_NOT_NULL(elem.param("L"))
    ASSERT_IS_NOT_NULL(elem.param("n"))
    ASSERT_IS_NULL(elem.param("n1"))
    ASSERT_IS_NULL(elem.param("n2"))
    ASSERT_IS_TRUE(elem.isRange())
    ASSERT_IS_TRUE(elem.asRange().has_value())
    ASSERT_IS_FALSE(elem.isDynamic())
    ASSERT_IS_FALSE(elem.asDynamic().has_value())
    ASSERT_IS_FALSE(elem.isInterface())
    ASSERT_IS_FALSE(elem.asInterface().has_value())
}

TEST_METHOD(ElementKind__Dynamic)
{
    TestElementDynamic elem;
    ASSERT_IS_TRUE(elem.kind() == ElementKind::Dynamic)
    ASSERT_BASE_MATRICES
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::T1))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::S1))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::T2))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::S2))
    ASSERT_IS_TRUE(elem.hasMatrix(MatrixKind::DynT))
    ASSERT_IS_TRUE(elem.hasMatrix(MatrixKind::DynS))
    ASSERT_IS_NULL(elem.param("L"))
    ASSERT_IS_NULL(elem.param("n"))
    ASSERT_IS_NULL(elem.param("n1"))
    ASSERT_IS_NULL(elem.param("n2"))
    ASSERT_IS_FALSE(elem.isRange())
    ASSERT_IS_FALSE(elem.asRange().has_value())
    ASSERT_IS_TRUE(elem.isDynamic())
    ASSERT_IS_TRUE(elem.asDynamic().has_value())
    ASSERT_IS_FALSE(elem.isInterface())
    ASSERT_IS_FALSE(elem.asInterface().has_value())
}

TEST_METHOD(ElementKind__Interface)
{
    TestElementInterface elem;
    ASSERT_IS_TRUE(elem.kind() == ElementKind::Interface)
    ASSERT_BASE_MATRICES
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::T1))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::S1))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::T2))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::S2))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::DynT))
    ASSERT_IS_FALSE(elem.hasMatrix(MatrixKind::DynS))
    ASSERT_IS_NULL(elem.param("L"))
    ASSERT_IS_NULL(elem.param("n"))
    ASSERT_IS_NOT_NULL(elem.param("n1"))
    ASSERT_IS_NOT_NULL(elem.param("n2"))
    ASSERT_IS_FALSE(elem.isRange())
    ASSERT_IS_FALSE(elem.asRange().has_value())
    ASSERT_IS_FALSE(elem.isDynamic())
    ASSERT_IS_FALSE(elem.asDynamic().has_value())
    ASSERT_IS_TRUE(elem.isInterface())
    ASSERT_IS_TRUE(elem.asInterface().has_value())
}

TEST_METHOD(ElementKind__RangeImpl)
{
    TestElementRange elem;
    auto r = elem.asRange();
    ASSERT_IS_TRUE(r.has_value())
    ASSERT_EQ_PTR(r->elem, &elem)
    ASSERT_EQ_PTR(r->paramLength(), elem.param("L"))
    ASSERT_EQ_PTR(r->paramIor(), elem.param("n"))
    ASSERT_EQ_PTR(&r->Mt1(), &elem.M(MatrixKind::T1))
    ASSERT_EQ_PTR(&r->Mt2(), &elem.M(MatrixKind::T2))
    ASSERT_EQ_PTR(&r->Ms1(), &elem.M(MatrixKind::S1))
    ASSERT_EQ_PTR(&r->Ms2(), &elem.M(MatrixKind::S2))
    ASSERT_EQ_PTR(r->pMt1(), elem.pM(MatrixKind::T1))
    ASSERT_EQ_PTR(r->pMt2(), elem.pM(MatrixKind::T2))
    ASSERT_EQ_PTR(r->pMs1(), elem.pM(MatrixKind::S1))
    ASSERT_EQ_PTR(r->pMs2(), elem.pM(MatrixKind::S2))
}

TEST_METHOD(ElementKind__DynamicImpl)
{
    TestElementDynamic elem;
    auto d = elem.asDynamic();
    ASSERT_IS_TRUE(d.has_value())
    ASSERT_EQ_PTR(d->elem, &elem)
    ASSERT_EQ_PTR(&d->Mt_dyn(), &elem.M(MatrixKind::DynT))
    ASSERT_EQ_PTR(&d->Ms_dyn(), &elem.M(MatrixKind::DynS))
    ASSERT_EQ_PTR(d->pMt_dyn(), elem.pM(MatrixKind::DynT))
    ASSERT_EQ_PTR(d->pMs_dyn(), elem.pM(MatrixKind::DynS))
}

TEST_METHOD(ElementKind__InterfaceImpl)
{
    TestElementInterface elem;
    auto i = elem.asInterface();
    ASSERT_IS_TRUE(i.has_value())
    ASSERT_EQ_PTR(i->elem, &elem)
    ASSERT_EQ_PTR(i->paramIor1(), elem.param("n1"))
    ASSERT_EQ_PTR(i->paramIor2(), elem.param("n2"))
}

//------------------------------------------------------------------------------

TEST_GROUP("Element",
    ADD_TEST(Element_ctor_generates_id),

    ADD_TEST(Element_must_add_param_in_ctor),
    ADD_TEST(Element_must_listen_its_params),
    ADD_TEST(Element_hasParams),

    ADD_TEST(Element_unlock_does_not_calculates_matrix),
    ADD_TEST(Element_unlock_does_not_raise_events),

    ADD_TEST(Element_setDisabled_must_set_flag),
    ADD_TEST(Element_setDisabled_must_raise_event),

    ADD_TEST(Element_setTitle_must_assign_value),
    ADD_TEST(Element_setTitle_must_raise_event),

    ADD_TEST(Element_setLabel_must_assign_value),
    ADD_TEST(Element_setLabel_must_raise_event),

    ADD_TEST(ElementOwner_setParam_must_recalculate_matrix),
    ADD_TEST(ElementOwner_setParam_must_recalculate_matrix_when_locked),
    ADD_TEST(ElementOwner_setParam_must_raise_event),
    
    ADD_TEST(Element_addParam__must_insert_at_index),
    ADD_TEST(Element_addParam__must_listen_new_param),
    ADD_TEST(Element_addParam__must_do_nothing_if_already_added),
    ADD_TEST(Element_removeParam__must_unlisten),
    ADD_TEST(Element_removeParam__must_destruct),
    ADD_TEST(Element_removeParam__does_nothing_if_not_added),
    ADD_TEST(Element_moveParamUp),
    ADD_TEST(Element_moveParamDown),
    
    ADD_TEST(ElementKind__Simple),
    ADD_TEST(ElementKind__Range),
    ADD_TEST(ElementKind__RangeImpl),
    ADD_TEST(ElementKind__Dynamic),
    ADD_TEST(ElementKind__DynamicImpl),
    ADD_TEST(ElementKind__Interface),
    ADD_TEST(ElementKind__InterfaceImpl),
)

} // namespace ElementTests
} // namespace Tests
} // namespace Z

