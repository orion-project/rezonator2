#include "testing/OriTestBase.h"
#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../funcs/StabilityMapFunction.h"

#include <QTextStream>

// Expected values for these tests calculated by `test_files/test_plot_funcs.rez`

static QString arrToStr(const QVector<double> arr) {
    QString res;
    QTextStream stream(&res);
    stream.setRealNumberNotation(QTextStream::SmartNotation);
    stream.setRealNumberPrecision(16);
    int size = arr.size();
    for (int i = 0; i < size; i++) {
        stream << arr.at(i);
        if (i < size-1)
            stream << ',';
    }
    return res;
}

#define ASSERT_NEAR_DBL_ARR(expr_arr1, expr_arr2, epsilon) {\
    const QVector<double>& arr1 = expr_arr1; \
    const QVector<double>& arr2 = expr_arr2; \
    ASSERT_EQ_INT(arr1.size(), arr2.size())\
    for (int i = 0; i < arr1.size(); i++) {\
        double v1 = arr1.at(i);\
        double v2 = arr2.at(i);\
        double delta = qAbs(v1 - v2); \
        if (std::isinf(v1) || std::isnan(v1) || delta > epsilon) {\
            test->setResult(false); \
            test->setMessage("Array is not equal to expected" ); \
            test->logAssertion("ARE ARRAYS NEAR EQUAL", \
                               QString("%1 == %2").arg(#expr_arr1).arg(#expr_arr2), \
                               arrToStr(arr2), arrToStr(arr1), __FILE__, __LINE__); \
            return; \
        }\
    }\
}

#define ASSERT_NEAR_TS(expr, expected_t, expected_s, epsilon) {\
    auto res = expr; \
    ASSERT_NEAR_DBL(res.T, expected_t, epsilon)\
    ASSERT_NEAR_DBL(res.S, expected_s, epsilon)\
}


namespace Z {
namespace Tests {
namespace PlotFunctionsTests {

struct TestSchema {
    Schema* schema;
    ElemCurveMirror* elem_M_back;
    ElemEmptyRange* elem_L_foc;
    ElemCurveMirror* elem_M_foc;
    ElemEmptyRange* elem_L;
    ElemFlatMirror* elem_M_out;
    QStringList errors;

    TestSchema() {
        schema = new Schema;
        schema->wavelength().setValue(1000_nm);
        elem_M_back = makeElem<ElemCurveMirror>({{"R", 30_mm}, {"Alpha", 5_deg}});
        elem_L_foc = makeElem<ElemEmptyRange>({{"L", 56_mm}});
        elem_M_foc = makeElem<ElemCurveMirror>({{"R", 50_mm}, {"Alpha", 5_deg}});
        elem_L = makeElem<ElemEmptyRange>({{"L", 420_mm}});
        elem_M_out = makeElem<ElemFlatMirror>({});
        schema->insertElements({elem_M_back, elem_L_foc, elem_M_foc, elem_L, elem_M_out}, -1, Arg::RaiseEvents(false));
    }

    ~TestSchema() {
        delete schema;
    }

    template <class TElem>
    TElem* makeElem(const QMap<QString, Z::Value>& params)
    {
        TElem* elem = new TElem;
        auto it = params.constBegin();
        while (it != params.constEnd()) {
            auto p = elem->params().byAlias(it.key());
            if (p)
                p->setValue(it.value());
            else
                errors << QString("%1: param not found: %2").arg(elem->type()).arg(it.key());
            it++;
        }
        return elem;
    }
};

#define TEST_SCHEMA \
    TestSchema s; \
    TEST_LOG(s.errors.join('\n')) \
    ASSERT_IS_TRUE(s.errors.isEmpty())

//------------------------------------------------------------------------------

namespace StabilityMap {

#define TEST_STAB_MAP_FUNC(mode) \
    TEST_SCHEMA \
    StabilityMapFunction func(s.schema); \
    func.setStabilityCalcMode(mode); \
    func.arg()->element = s.elem_L_foc; \
    func.arg()->parameter = s.elem_L_foc->paramLength(); \
    func.arg()->range = Z::VariableRange::withPoints(24_mm, 60_mm, 10); \
    func.calculate(); \
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_T), 1) \
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_S), 1)

TEST_CASE_METHOD(calculate, Z::Enums::StabilityCalcMode mode, const QVector<double>& expected_t, const QVector<double>& expected_s) {
    TEST_STAB_MAP_FUNC(mode)
    auto resT = func.result(Z::WorkPlane::Plane_T, 0);
    auto resS = func.result(Z::WorkPlane::Plane_S, 0);
    QVector<double> expected_x = {0.024,0.028,0.032,0.036,0.04,0.044,0.048,0.052,0.056,0.06};
    ASSERT_NEAR_DBL_ARR(resT.x(), expected_x, 1e-7)
    ASSERT_NEAR_DBL_ARR(resT.y(), expected_t, 1e-7)
    ASSERT_NEAR_DBL_ARR(resS.x(), expected_x, 1e-7)
    ASSERT_NEAR_DBL_ARR(resS.y(), expected_s, 1e-7)
}

TEST_CASE(calculate_normal, calculate, Z::Enums::StabilityCalcMode::Normal,
    {-2.24824294,2.74190048,6.3679392,8.62987323,9.52770256,9.0614272,7.23104715,4.03656241,-0.522027033,-6.44472117},
    {-2.49658894,2.48410585,6.13218941,8.44766175,9.43052287,9.08077276,7.39841143,4.38343887,0.0358550835,-5.64433992})
TEST_CASE(calculate_squared, calculate, Z::Enums::StabilityCalcMode::Squared,
    {-4.05459631,-6.51801823,-39.5506496,-73.4747119,-89.7771161,-81.109463,-51.2880429,-15.2938361,0.727487776,-40.5344309},
    {-5.23295635,-5.17078186,-36.603747,-70.3629891,-87.9347616,-81.4604339,-53.7364916,-18.2145363,0.998714413,-30.8585732})

TEST_METHOD(calculateAt) {
    TEST_STAB_MAP_FUNC(Z::Enums::StabilityCalcMode::Squared)
    ASSERT_NEAR_TS(func.calculateAt(3_cm), -21.3297163, -19.0231552, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(45_mm), -75.2428819, -76.1782408, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(7_cm), -739.896527, -658.205705, 1e-6)
}

TEST_GROUP("Stability Map",
           ADD_TEST(calculate_normal),
           ADD_TEST(calculate_squared),
           ADD_TEST(calculateAt),
           )
} // namespace StabilityMap

//------------------------------------------------------------------------------

TEST_GROUP("Plot functions",
           ADD_GROUP(StabilityMap),
           )

} // namespace PlotFunctionsTests
} // namespace Tests
} // namespace Z

