#include "../core/Parameters.h"
#include "../tests/TestUtils.h"

#include "testing/OriTestBase.h"

#include <QtMath>

namespace Z {
namespace Tests {
namespace ParametersTests {

//------------------------------------------------------------------------------

TEST_METHOD(Parameter_ctor_default)
{
    Z::Parameter p;
    ASSERT_EQ_PTR(p.dim(), Z::Dims::none())
    ASSERT_IS_TRUE(p.visible())
}

TEST_METHOD(Parameter_ctor_params)
{
    Z::Parameter p(Z::Dims::linear(), "alias", "label", "name", "description", "category", false);
    ASSERT_EQ_PTR(p.dim(), Z::Dims::linear())
    ASSERT_EQ_STR(p.alias(), "alias")
    ASSERT_EQ_STR(p.label(), "label")
    ASSERT_EQ_STR(p.name(), "name")
    ASSERT_EQ_STR(p.description(), "description")
    ASSERT_EQ_STR(p.category(), "category")
    ASSERT_IS_FALSE(p.visible())
}

TEST_METHOD(Parameter_setValue_getValue)
{
    Z::Parameter p(Z::Dims::linear(), "", "", "");

    p.setValue(100_m);
    ASSERT_EQ_ZVALUE(p.value(), 100_m)

    p.setValue(3.14_mm);
    ASSERT_EQ_ZVALUE(p.value(), 3.14_mm)
}

TEST_METHOD(Parameter_setValue_resets_expr)
{
    Z::Parameter p;
    p.setExpr("2+2");
    ASSERT_EQ_STR(p.expr(), "2+2");
    p.setValue(0);
    ASSERT_IS_TRUE(p.expr().isEmpty());
}

TEST_METHOD(Parameter_setValue_resets_error)
{
    Z::Parameter p;
    p.setError("error");
    ASSERT_EQ_STR(p.error(), "error");
    p.setValue(0);
    ASSERT_IS_TRUE(p.error().isEmpty());
}

class TestParamVerifier : public Z::ValueVerifier
{
public:
    bool on = true;
    bool enabled() const override { return on; }
    QString verify(const Z::Value& value) const override {
        if (value.isZero()) return "not allowed";
        return QString();
    }
};

TEST_METHOD(Parameter_verify)
{
    Z::Parameter p;
    ASSERT_IS_TRUE(p.verify(0).isEmpty())
    ASSERT_IS_TRUE(p.verify(1).isEmpty())
    TestParamVerifier v;
    p.setVerifier(&v);
    ASSERT_IS_FALSE(p.verify(0).isEmpty())
    ASSERT_IS_TRUE(p.verify(1).isEmpty())
    v.on = false;
    ASSERT_IS_TRUE(p.verify(0).isEmpty())
    ASSERT_IS_TRUE(p.verify(1).isEmpty())
}

//------------------------------------------------------------------------------

class TestParamListener : public Z::ParameterListener
{
public:
    QString changedParam, failedParam;
    void parameterChanged(Z::ParameterBase *p) override { changedParam = p->alias(); }
    void parameterFailed(Z::ParameterBase *p) override { failedParam = p->alias(); }
};

TEST_METHOD(ParameterListener_parameterChanged)
{
    TestParamListener listener;
    Z::Parameter p("p1");
    p.addListener(&listener);
    p.setValue(100_mm);
    ASSERT_EQ_STR(listener.changedParam, p.alias())
    ASSERT_IS_TRUE(listener.failedParam.isEmpty())
}

TEST_METHOD(ParameterListener_parameterFailed)
{
    TestParamListener listener;
    Z::Parameter p("p1");
    p.addListener(&listener);
    p.setError("error");
    ASSERT_IS_TRUE(listener.changedParam.isEmpty())
    ASSERT_EQ_STR(listener.failedParam, p.alias())
}

TEST_METHOD(ParameterListener_parameterFailed_no_event_when_reset_error)
{
    TestParamListener listener;
    Z::Parameter p("p1");
    p.addListener(&listener);
    p.setError("");
    ASSERT_IS_TRUE(listener.changedParam.isEmpty())
    ASSERT_IS_TRUE(listener.failedParam.isEmpty())
}

//------------------------------------------------------------------------------

TEST_METHOD(Parameters_byAlias)
{
    Z::Parameter p1(Z::Dims::none(), "p1", "", "");
    Z::Parameter p2(Z::Dims::none(), "p2", "", "");
    Z::Parameters params { &p1, &p2 };
    ASSERT_EQ_PTR(params.byAlias("p1"), &p1)
    ASSERT_EQ_PTR(params.byAlias("p2"), &p2)
    ASSERT_IS_NULL(params.byAlias("p3"))
}

TEST_METHOD(Parameters_byIndex)
{
    Z::Parameter p1(Z::Dims::none(), "p1", "", "");
    Z::Parameter p2(Z::Dims::none(), "p2", "", "");
    Z::Parameters params { &p1, &p2 };
    ASSERT_EQ_PTR(params.byIndex(0), &p1)
    ASSERT_EQ_PTR(params.byIndex(1), &p2)
    ASSERT_IS_NULL(params.byIndex(2))
}

TEST_METHOD(Parameters_byPointer)
{
    Z::Parameter p1(Z::Dims::none(), "p1", "", "");
    Z::Parameter p2(Z::Dims::none(), "p2", "", "");
    Z::Parameter p3(Z::Dims::none(), "p3", "", "");
    Z::Parameters params { &p1, &p2 };
    ASSERT_EQ_PTR(params.byPointer(&p1), &p1)
    ASSERT_EQ_PTR(params.byPointer(&p2), &p2)
    ASSERT_IS_NULL(params.byPointer(&p3))
}

//------------------------------------------------------------------------------

TEST_METHOD(ParameterLink_constructor_destructor)
{
    Z::Parameter tgt;
    tgt.setValue(1);
    
    Z::Parameter src;
    src.setValue(2);
    
    auto lnk = new ParamLink(&src, &tgt);
    ASSERT_EQ_INT(tgt.valueDriver(), ParamValueDriver::Link)
    ASSERT_IS_TRUE(src.listeners().contains(lnk));
    ASSERT_EQ_DBL(tgt.value().value(), 1);
    
    lnk->apply();
    ASSERT_EQ_DBL(tgt.value().value(), 2);
    
    src.setValue(3);
    ASSERT_EQ_DBL(tgt.value().value(), 3);
    
    delete lnk;
    ASSERT_EQ_INT(tgt.valueDriver(), ParamValueDriver::None)
    ASSERT_IS_FALSE(src.listeners().contains(lnk));
    ASSERT_EQ_DBL(tgt.value().value(), src.value().value());
}

TEST_METHOD(ParameterLink_apply_si_when_dim_mismatch)
{
    Z::Parameter tgt(Z::Dims::linear(), "");
    tgt.setValue(1_cm);
    
    Z::Parameter src(Z::Dims::angular(), "");
    src.setValue(1_deg);
    
    ParamLink lnk(&src, &tgt);
    lnk.apply();
    
    ASSERT_EQ_DBL(tgt.value().toSi(), qDegreesToRadians(1.0));
    ASSERT_EQ_UNIT(tgt.value().unit(), Z::Units::cm());
}

TEST_METHOD(ParameterLink_apply_error_when_source_failed)
{
    Z::Parameter tgt;
    Z::Parameter src("src-param");
    ParamLink lnk(&src, &tgt);
    
    ASSERT_IS_FALSE(tgt.failed())
    
    src.setError("src-error");
    ASSERT_IS_TRUE(tgt.failed())
    ASSERT_IS_TRUE(tgt.error().contains("src-param"))
    ASSERT_IS_TRUE(tgt.error().contains("src-error"))
}

TEST_METHOD(ParameterLink_apply_error_when_verification_failed)
{
    Z::Parameter tgt;
    TestParamVerifier v;
    tgt.setVerifier(&v);

    Z::Parameter src("src-param");

    ParamLink lnk(&src, &tgt);

    src.setValue(1);
    ASSERT_IS_FALSE(tgt.failed())

    src.setValue(0);
    ASSERT_IS_TRUE(tgt.failed())
    ASSERT_IS_TRUE(tgt.error().contains("src-param"))
    ASSERT_IS_TRUE(tgt.error().contains(src.value().displayStr()))
}

//------------------------------------------------------------------------------

namespace ParameterFilterTests {

TEST_METHOD(ParameterFilterVisible_check)
{
    Z::ParameterFilterVisible filter;

    Z::Parameter p;
    ASSERT_IS_TRUE(filter.check(&p))

    p.setVisible(false);
    ASSERT_IS_FALSE(filter.check(&p))
}

TEST_GROUP("ParameterFilter",
    ADD_TEST(ParameterFilterVisible_check)
)

} // namespace ParameterFilterTests

//------------------------------------------------------------------------------

TEST_GROUP("Parameters",
    ADD_TEST(Parameter_ctor_default),
    ADD_TEST(Parameter_ctor_params),
    ADD_TEST(Parameter_setValue_getValue),
    ADD_TEST(Parameter_setValue_resets_expr),
    ADD_TEST(Parameter_setValue_resets_error),
    ADD_TEST(Parameter_verify),
    ADD_TEST(ParameterListener_parameterChanged),
    ADD_TEST(ParameterListener_parameterFailed),
    ADD_TEST(ParameterListener_parameterFailed_no_event_when_reset_error),
    ADD_TEST(Parameters_byAlias),
    ADD_TEST(Parameters_byIndex),
    ADD_TEST(Parameters_byPointer),
    ADD_TEST(ParameterLink_constructor_destructor),
    ADD_TEST(ParameterLink_apply_si_when_dim_mismatch),
    ADD_TEST(ParameterLink_apply_error_when_source_failed),
    ADD_TEST(ParameterLink_apply_error_when_verification_failed),
    ADD_GROUP(ParameterFilterTests),
)

} // namespace ParametersTests
} // namespace Tests
} // namespace Z
