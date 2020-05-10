#include "testing/OriTestBase.h"
#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../funcs/StabilityMapFunction.h"
#include "../funcs/StabilityMap2DFunction.h"

#include <QTextStream>

// Expected values for these tests calculated by `test_files/test_plot_funcs.rez`

static QString arrToStr(const QVector<double> arr)
{
    QString res;
    QTextStream stream(&res);
    stream.setRealNumberNotation(QTextStream::SmartNotation);
    stream.setRealNumberPrecision(16);
    int size = arr.size();
    for (int i = 0; i < size; i++)
    {
        stream << arr.at(i);
        if (i < size-1)
            stream << ',';
    }
    return res;
}

#define ASSERT_NEAR_DBL_ARR(expr_arr1, expr_arr2, epsilon) {\
    const QVector<double>& arr1 = expr_arr1; \
    const QVector<double>& arr2 = expr_arr2; \
    ASSERT_EQ_INT(arr1.size(), arr2.size()) \
    for (int i = 0; i < arr1.size(); i++) \
    {\
        double v1 = arr1.at(i);\
        double v2 = arr2.at(i);\
        double delta = qAbs(v1 - v2); \
        if (std::isinf(v1) || std::isnan(v1) || delta > epsilon) \
        {\
            test->setResult(false); \
            test->setMessage("Array is not equal to expected" ); \
            test->logAssertion("ARE ARRAYS NEAR EQUAL", \
                               QString("%1 == %2").arg(#expr_arr1).arg(#expr_arr2), \
                               arrToStr(arr2), arrToStr(arr1), __FILE__, __LINE__); \
            return; \
        }\
    }\
}

#define ASSERT_NEAR_TS(expr, expected_t, expected_s, epsilon) \
{\
    auto res = expr; \
    ASSERT_NEAR_DBL(res.T, expected_t, epsilon)\
    ASSERT_NEAR_DBL(res.S, expected_s, epsilon)\
}


namespace Z {
namespace Tests {
namespace PlotFunctionsTests {

struct TestSchema
{
    Schema* schema;
    ElemCurveMirror* elem_M_back;
    ElemEmptyRange* elem_L_foc;
    ElemCurveMirror* elem_M_foc;
    ElemEmptyRange* elem_L;
    ElemFlatMirror* elem_M_out;
    QStringList errors;

    TestSchema()
    {
        schema = new Schema;
        schema->wavelength().setValue(1000_nm);
        elem_M_back = makeElem<ElemCurveMirror>({{"R", 30_mm}, {"Alpha", 5_deg}});
        elem_L_foc = makeElem<ElemEmptyRange>({{"L", 56_mm}});
        elem_M_foc = makeElem<ElemCurveMirror>({{"R", 50_mm}, {"Alpha", 5_deg}});
        elem_L = makeElem<ElemEmptyRange>({{"L", 420_mm}});
        elem_M_out = makeElem<ElemFlatMirror>({});
        schema->insertElements({elem_M_back, elem_L_foc, elem_M_foc, elem_L, elem_M_out}, -1, Arg::RaiseEvents(false));
    }

    ~TestSchema()
    {
        delete schema;
    }

    template <class TElem>
    TElem* makeElem(const QMap<QString, Z::Value>& params)
    {
        TElem* elem = new TElem;
        auto it = params.constBegin();
        while (it != params.constEnd())
        {
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

TEST_CASE_METHOD(calculate, Z::Enums::StabilityCalcMode mode, const QVector<double>& expectedT, const QVector<double>& expectedS)
{
    TEST_STAB_MAP_FUNC(mode)
    auto resT = func.result(Z::WorkPlane::Plane_T, 0);
    auto resS = func.result(Z::WorkPlane::Plane_S, 0);
    QVector<double> expectedX = {0.024,0.028,0.032,0.036,0.04,0.044,0.048,0.052,0.056,0.06};
    ASSERT_NEAR_DBL_ARR(resT.x(), expectedX, 1e-7)
    ASSERT_NEAR_DBL_ARR(resT.y(), expectedT, 1e-7)
    ASSERT_NEAR_DBL_ARR(resS.x(), expectedX, 1e-7)
    ASSERT_NEAR_DBL_ARR(resS.y(), expectedS, 1e-7)
}

TEST_CASE(calculate_normal, calculate, Z::Enums::StabilityCalcMode::Normal,
    {-2.24824294,2.74190048,6.3679392,8.62987323,9.52770256,9.0614272,7.23104715,4.03656241,-0.522027033,-6.44472117},
    {-2.49658894,2.48410585,6.13218941,8.44766175,9.43052287,9.08077276,7.39841143,4.38343887,0.0358550835,-5.64433992})
TEST_CASE(calculate_squared, calculate, Z::Enums::StabilityCalcMode::Squared,
    {-4.05459631,-6.51801823,-39.5506496,-73.4747119,-89.7771161,-81.109463,-51.2880429,-15.2938361,0.727487776,-40.5344309},
    {-5.23295635,-5.17078186,-36.603747,-70.3629891,-87.9347616,-81.4604339,-53.7364916,-18.2145363,0.998714413,-30.8585732})

TEST_METHOD(calculateAt)
{
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

namespace StabilityMap2D {

#define TEST_STAB_MAP_2D_FUNC(mode) \
    TEST_SCHEMA \
    StabilityMap2DFunction func(s.schema); \
    func.setStabilityCalcMode(mode); \
    func.paramX()->element = s.elem_L_foc; \
    func.paramX()->parameter = s.elem_L_foc->paramLength(); \
    func.paramX()->range = Z::VariableRange::withPoints(0_mm, 100_mm, 10); \
    func.paramY()->element = s.elem_L; \
    func.paramY()->parameter = s.elem_L->paramLength(); \
    func.paramY()->range = Z::VariableRange::withPoints(0_mm, 500_mm, 10); \
    func.calculate();

TEST_CASE_METHOD(calculate, Z::Enums::StabilityCalcMode mode, const QVector<double>& expectedT, const QVector<double>& expectedS)
{
    TEST_STAB_MAP_2D_FUNC(mode)
    ASSERT_NEAR_DBL_ARR(func.resultsT(), expectedT, 1e-4)
    ASSERT_NEAR_DBL_ARR(func.resultsS(), expectedS, 1e-4)
}

TEST_CASE(calculate_normal, calculate, Z::Enums::StabilityCalcMode::Normal,
    {1,-7.17927275,-15.3585455,-23.5378183,-31.717091,-39.8963638,-48.0756365,-56.2549093,-64.434182,-72.6134548,
    -0.30411652,-3.91559238,-7.52706824,-11.1385441,-14.75002,-18.3614958,-21.9729717,-25.5844475,-29.1959234,-32.8073993,
    -0.94475698,-1.46845913,-1.99216128,-2.51586343,-3.03956557,-3.56326772,-4.08696987,-4.61067202,-5.13437417,-5.65807632,
    -0.92192138,0.162127003,1.24617539,2.33022377,3.41427215,4.49832054,5.58236892,6.6664173,7.75046569,8.83451407,
    -0.23560972,0.976166015,2.18794175,3.39971749,4.61149322,5.82326896,7.03504469,8.24682043,9.45859616,10.6703719,
    1.114178,0.973657908,0.833137815,0.692617722,0.552097629,0.411577537,0.271057444,0.130537351,-0.00998274185,-0.150502835,
    3.12744178,0.154602681,-2.81823642,-5.79107552,-8.76391462,-11.7367537,-14.7095928,-17.6824319,-20.655271,-23.6281101,
    5.80418162,-1.48099967,-8.76618095,-16.0513622,-23.3365435,-30.6217248,-37.9069061,-45.1920874,-52.4772687,-59.76245,
    9.14439752,-3.93314913,-17.0106958,-30.0882424,-43.1657891,-56.2433357,-69.3208824,-82.3984291,-95.4759757,-108.553522,
    13.1480895,-7.20184572,-27.5517809,-47.9017161,-68.2516513,-88.6015865,-108.951522,-129.301457,-149.651392,-170.001327},
    {1,-7.11714198,-15.234284,-23.351426,-31.4685679,-39.5857099,-47.7028519,-55.8199939,-63.9371359,-72.0542779,
     -0.296711071,-3.90965155,-7.52259203,-11.1355325,-14.748473,-18.3614135,-21.974354,-25.5872944,-29.2002349,-32.8131754,
     -0.939987491,-1.49527788,-2.05056827,-2.60585865,-3.16114904,-3.71643943,-4.27172982,-4.82702021,-5.38231059,-5.93760098,
     -0.92982926,0.125979033,1.18178733,2.23759562,3.29340391,4.34921221,5.4050205,6.46082879,7.51663709,8.57244538,
     -0.266236378,0.954119186,2.17447475,3.39483031,4.61518588,5.83554144,7.055897,8.27625257,9.49660813,10.7169637,
     1.05079116,0.989142578,0.927494001,0.865845423,0.804196846,0.742548268,0.680899691,0.619251113,0.557602536,0.495953958,
     3.02125334,0.23104921,-2.55915492,-5.34935905,-8.13956318,-10.9297673,-13.7199714,-16.5101756,-19.3003797,-22.0905838,
     5.64515018,-1.32016092,-8.28547201,-15.2507831,-22.2160942,-29.1814053,-36.1467164,-43.1120275,-50.0773386,-57.0426497,
     8.92248166,-3.66448781,-16.2514573,-28.8384267,-41.4253962,-54.0123657,-66.5993351,-79.1863046,-91.7732741,-104.360244,
     12.8532478,-6.80193145,-26.4571107,-46.11229,-65.7674692,-85.4226485,-105.077828,-124.733007,-144.388186,-164.043365})
TEST_CASE(calculate_squared, calculate, Z::Enums::StabilityCalcMode::Squared,
    {0,-50.5419572,-234.88492,-553.028888,-1004.97386,-1590.71984,-2310.26683,-3163.61482,-4150.76381,-5271.71381,
     0.907513142,-14.3318637,-55.6567563,-123.067165,-216.563089,-336.144529,-481.811484,-653.563956,-851.401943,-1075.32545,
     0.107434249,-1.15637221,-2.96870655,-5.32956877,-8.23895888,-11.6968769,-15.7033227,-20.2582965,-25.3617981,-31.0138276,
     0.150060969,0.973714835,-0.552953094,-4.42994282,-10.6572543,-19.2348876,-30.1628427,-43.4411196,-59.0697183,-77.0486388,
     0.94448806,0.0470999105,-3.7870891,-10.558079,-20.2658697,-32.9104613,-48.4918538,-67.0100471,-88.4650413,-112.856836,
     -0.241392617,0.0519902785,0.305881381,0.520280691,0.695188208,0.830603931,0.926527862,0.98296,0.999900345,0.977348897,
     -8.78089209,0.976098011,-6.94245652,-32.5365557,-75.8061995,-136.751388,-215.372121,-311.668399,-425.640221,-557.287588,
     -32.6885243,-1.19336001,-75.8459285,-256.64623,-543.594264,-936.690031,-1435.93353,-2041.32476,-2752.86373,-3570.55043,
     -82.620006,-14.4696621,-288.363771,-904.302333,-1862.28535,-3162.31282,-4804.38474,-6788.50111,-9114.66194,-11782.8672,
     -171.872257,-50.8665817,-758.100632,-2293.57441,-4657.28791,-7849.24113,-11869.4341,-16717.8668,-22394.5392,-28899.4513},
    {0,-49.65371,-231.083408,-544.289094,-989.270768,-1566.02843,-2274.56208,-3114.87172,-4086.95734,-5190.81896,
     0.91196254,-14.2853753,-55.5893909,-123.000084,-216.517456,-336.141505,-481.872232,-653.709636,-851.653719,-1075.70448,
     0.116423516,-1.23585594,-3.20483022,-5.79049933,-8.99286327,-12.811922,-17.2476756,-22.3001241,-27.9692673,-34.2551054,
     0.135417547,0.984129283,-0.396621286,-4.00683416,-9.84650934,-17.9156468,-28.2142466,-40.7423087,-55.4998331,-72.4868198,
     0.929118191,0.0896565795,-3.72834044,-10.5248729,-20.2999407,-33.0535439,-48.7856825,-67.4963566,-89.185566,-113.853311,
     -0.104162053,0.0215969603,0.139754879,0.250311703,0.353267433,0.448622069,0.536375611,0.616528059,0.689079412,0.754029671,
     -8.12797175,0.946616262,-5.5492739,-27.6156422,-65.2524887,-118.459813,-187.237616,-271.585897,-371.504657,-486.993894,
     -30.8677205,-0.742824848,-67.6490464,-231.586385,-492.554841,-850.554415,-1305.58511,-1857.64691,-2506.73984,-3252.86388,
     -78.610679,-12.4284709,-263.109864,-830.654857,-1715.06345,-2916.33565,-4434.47144,-6269.47084,-8421.33384,-10890.0604,
     -164.205979,-45.2662715,-698.978707,-2125.34329,-4324.36001,-7296.02887,-11040.3499,-15557.323,-20846.9483,-26909.2258})

TEST_METHOD(calculateAt)
{
    TEST_STAB_MAP_2D_FUNC(Z::Enums::StabilityCalcMode::Normal)
    ASSERT_NEAR_TS(func.calculateAt(30_mm, 50_mm), -0.317023954, -0.349557439, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(85_mm, 70_mm), -5.81129978, -5.47201575, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(5_cm, 40_cm), 5.54489081, 5.78383948, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(12_cm, 30_cm), -178.690727, -172.950357, 1e-6)
}

TEST_GROUP("2D Stability Map",
           ADD_TEST(calculate_normal),
           ADD_TEST(calculate_squared),
           ADD_TEST(calculateAt),
           )
} // namespace StabilityMap2

//------------------------------------------------------------------------------

TEST_GROUP("Plot functions",
           ADD_GROUP(StabilityMap),
           ADD_GROUP(StabilityMap2D),
           )

} // namespace PlotFunctionsTests
} // namespace Tests
} // namespace Z

