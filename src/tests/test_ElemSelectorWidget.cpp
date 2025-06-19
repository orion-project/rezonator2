#include "../core/Elements.h"
#include "../core/ElementFilter.h"
#include "../core/Schema.h"
#include "../widgets/ElemSelectorWidget.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {
namespace ElementSelectorWidgetTests {

#define ASSERT_PTR_LIST(provided, expected)\
    ASSERT_EQ_INT(provided.size(), expected.size())\
    for (int i = 0; i < provided.size(); i++) {\
        ASSERT_EQ_PTR(provided.at(i), expected.at(i))\
    }

#define TEST_SCHEMA(var)\
    Schema var;\
    schema.insertElements({\
        new ElemMatrix,\
        new ElemEmptyRange,\
        new ElemThinLens,\
        new ElemEmptyRange,\
        new ElemFlatMirror,\
    }, -1, Arg::RaiseEvents(false));\

//------------------------------------------------------------------------------

namespace ElemSelectorWidgetTests {

template<bool Result> class TestElemFilter : public ElementFilterCondition
{
public:
    QVector<const Element*> checkedElems;
    bool check(const Element *elem) override { checkedElems.append(elem); return Result; }
};

namespace constructor {

TEST_METHOD(appends_all_elements_when_no_filter)
{
    TEST_SCHEMA(schema)
    ElemSelectorWidget target(&schema, {});
    ASSERT_PTR_LIST(target.elements(), schema.elements())
    ASSERT_EQ_INT(target.count(), schema.count())
}

TEST_METHOD(must_use_filter)
{
    TEST_SCHEMA(schema)
    auto condition = new TestElemFilter<true>();
    ElementFilterPtr f(new ElementFilter({condition}));
    ElemSelectorWidget target(&schema, {.filter = f});
    ASSERT_PTR_LIST(condition->checkedElems, schema.elements())
    ASSERT_PTR_LIST(target.elements(), schema.elements())
    ASSERT_EQ_INT(target.count(), schema.count())
}

TEST_METHOD(must_use_filter_for_custom_params)
{
    TEST_SCHEMA(schema)
    schema.addGlobalParam(new Z::Parameter(Z::Dims::none(), ""));
    auto condition = new TestElemFilter<true>();
    ElementFilterPtr f(new ElementFilter({condition}));
    ElemSelectorWidget target(&schema, {.filter = f, .includeCustomParams = true});
    Elements elems(schema.elements());
    elems << const_cast<Element*>(schema.globalParamsAsElem());
    ASSERT_PTR_LIST(condition->checkedElems, elems)
    ASSERT_PTR_LIST(target.elements(), elems)
    ASSERT_EQ_INT(target.count(), schema.count()+1)
}

TEST_METHOD(must_respect_filter)
{
    TEST_SCHEMA(schema)
    auto condition = new TestElemFilter<false>();
    schema.addGlobalParam(new Z::Parameter(Z::Dims::none(), ""));
    ElementFilterPtr f(new ElementFilter({condition}));
    ElemSelectorWidget target(&schema, {.filter = f});
    ASSERT_PTR_LIST(condition->checkedElems, schema.elements())
    ASSERT_EQ_INT(target.elements().size(), 0)
    ASSERT_EQ_INT(target.count(), 0)
}

TEST_METHOD(must_respect_filter_for_custom_params)
{
    TEST_SCHEMA(schema)
    auto condition = new TestElemFilter<false>();
    schema.addGlobalParam(new Z::Parameter(Z::Dims::none(), ""));
    ElementFilterPtr f(new ElementFilter({condition}));
    ElemSelectorWidget target(&schema, {.filter = f, .includeCustomParams = true});
    Elements elems(schema.elements());
    elems << const_cast<Element*>(schema.globalParamsAsElem());
    ASSERT_PTR_LIST(condition->checkedElems, elems)
    ASSERT_EQ_INT(target.elements().size(), 0)
    ASSERT_EQ_INT(target.count(), 0)
}

} // namespace constructor

TEST_METHOD(must_initially_select_the_first)
{
    TEST_SCHEMA(schema)
    ElemSelectorWidget target(&schema, {});
    ASSERT_EQ_INT(target.currentIndex(), 0)
    ASSERT_EQ_PTR(target.selectedElement(), schema.elements().first())
}

TEST_METHOD(can_set_selected_elem_and_current_index)
{
    TEST_SCHEMA(schema)
    ElemSelectorWidget target(&schema, {});

    target.setSelectedElement(schema.element(1));
    ASSERT_EQ_INT(target.currentIndex(), 1)
    ASSERT_EQ_PTR(target.selectedElement(), schema.element(1))

    target.setCurrentIndex(2);
    ASSERT_EQ_INT(target.currentIndex(), 2)
    ASSERT_EQ_PTR(target.selectedElement(), schema.element(2))
}

TEST_GROUP("ElemSelectorWidget",
    ADD_GUI_TEST(constructor::appends_all_elements_when_no_filter),
    ADD_GUI_TEST(constructor::must_use_filter),
    ADD_GUI_TEST(constructor::must_use_filter_for_custom_params),
    ADD_GUI_TEST(constructor::must_respect_filter),
    ADD_GUI_TEST(constructor::must_respect_filter_for_custom_params),
    ADD_GUI_TEST(must_initially_select_the_first),
    ADD_GUI_TEST(can_set_selected_elem_and_current_index),
)

} // namespace ElemSelectorWidgetTests

//------------------------------------------------------------------------------

namespace ParamSelectorWidgetTests {

template<bool Result>
class TestParamFilter : public Z::ParameterFilterCondition
{
public:
    QVector<Z::Parameter*> checkedParams;
    bool check(Z::Parameter *p) const override
    {
        const_cast<TestParamFilter*>(this)->checkedParams.append(p);
        return Result;
    }
};

namespace populate {

TEST_METHOD(appends_all_elements_when_no_filter)
{
    ElemMatrix elem;
    ParamSelectorWidget target;
    target.populate(&elem);
    ASSERT_PTR_LIST(target.parameters(), elem.params())
    ASSERT_EQ_INT(target.count(), elem.params().size())
}

TEST_METHOD(must_use_filter)
{
    auto condition = new TestParamFilter<true>();
    Z::ParameterFilterPtr filter(new Z::ParameterFilter({condition}));
    ElemMatrix elem;
    ParamSelectorWidget target(filter);
    target.populate(&elem);
    ASSERT_PTR_LIST(condition->checkedParams, elem.params())
    ASSERT_PTR_LIST(target.parameters(), elem.params())
    ASSERT_EQ_INT(target.count(), elem.params().size())
}

TEST_METHOD(must_respect_filter)
{
    auto condition = new TestParamFilter<false>();
    Z::ParameterFilterPtr filter(new Z::ParameterFilter({condition}));
    ElemMatrix elem;
    ParamSelectorWidget target(filter);
    target.populate(&elem);
    ASSERT_PTR_LIST(condition->checkedParams, elem.params())
    ASSERT_EQ_INT(target.parameters().size(), 0)
    ASSERT_EQ_INT(target.count(), 0)
}

} // namespace populate

TEST_METHOD(must_select_the_first_when_populated)
{
    ElemMatrix elem;
    ParamSelectorWidget target;
    target.populate(&elem);
    ASSERT_EQ_INT(target.currentIndex(), 0)
    ASSERT_EQ_PTR(target.selectedParameter(), elem.params().first())
}

TEST_METHOD(can_set_selected_param_and_current_index)
{
    ElemMatrix elem;
    ParamSelectorWidget target;
    target.populate(&elem);

    target.setSelectedParameter(elem.params().at(2));
    ASSERT_EQ_INT(target.currentIndex(), 2)
    ASSERT_EQ_PTR(target.selectedParameter(), elem.params().at(2))

    target.setCurrentIndex(3);
    ASSERT_EQ_INT(target.currentIndex(), 3)
    ASSERT_EQ_PTR(target.selectedParameter(), elem.params().at(3))
}

namespace repopulate {

#define POPULATED_AND_SELECTED_TARGET\
    ElemMatrix elem;\
    ParamSelectorWidget target;\
    target.populate(&elem);\
    target.setCurrentIndex(2);

#define ASSERT_TARGET_IS_EMPTY\
    ASSERT_EQ_INT(target.count(), 0)\
    ASSERT_EQ_INT(target.parameters().size(), 0)\
    ASSERT_EQ_INT(target.currentIndex(), -1)\
    ASSERT_IS_NULL(target.selectedParameter())

TEST_METHOD(with_null)
{
    POPULATED_AND_SELECTED_TARGET
    target.populate(nullptr);
    ASSERT_TARGET_IS_EMPTY
}

TEST_METHOD(with_elem_having_no_params)
{
    POPULATED_AND_SELECTED_TARGET
    ElemFlatMirror elem2;
    target.populate(&elem2);
    ASSERT_TARGET_IS_EMPTY
}

} // namespace repopulate

TEST_GROUP("ParamSelectorWidget",
    ADD_GUI_TEST(populate::appends_all_elements_when_no_filter),
    ADD_GUI_TEST(populate::must_use_filter),
    ADD_GUI_TEST(populate::must_respect_filter),
    ADD_GUI_TEST(must_select_the_first_when_populated),
    ADD_GUI_TEST(can_set_selected_param_and_current_index),
    ADD_GUI_TEST(repopulate::with_null),
    ADD_GUI_TEST(repopulate::with_elem_having_no_params),
)

} // namespace ParamSelectorWidgetTests

//------------------------------------------------------------------------------

namespace ElemAndParamSelectorTests {

TEST_METHOD(must_initially_select_the_first)
{
    TEST_SCHEMA(schema)
    ElemAndParamSelector target(&schema, {});
    ASSERT_EQ_PTR(target.selectedElement(), schema.elements().first())
    ASSERT_EQ_PTR(target.selectedParameter(), schema.elements().first()->params().first())
}

TEST_METHOD(must_populate_params_when_selected_element_changes)
{
    TEST_SCHEMA(schema)
    ElemAndParamSelector target(&schema, {});
    target.setSelectedElement(schema.elements().at(2));
    ASSERT_PTR_LIST(target.paramSelector()->parameters(), schema.elements().at(2)->params())
}

TEST_GROUP("ElemAndParamSelector",
    ADD_GUI_TEST(must_initially_select_the_first),
    ADD_GUI_TEST(must_populate_params_when_selected_element_changes),
)

} // namespace ElemAndParamSelectorTests

//------------------------------------------------------------------------------

TEST_GROUP("Element Selector Widgets",
    ADD_GROUP(ElemSelectorWidgetTests),
    ADD_GROUP(ParamSelectorWidgetTests),
    ADD_GROUP(ElemAndParamSelectorTests),
)

} // namespace ElementSelectorWidgetTests
} // namespace Tests
} // namespace Z

