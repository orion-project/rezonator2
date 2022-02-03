#include "testing/OriTestBase.h"
#include "TestUtils.h"
#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../funcs/StabilityMapFunction.h"
#include "../funcs/StabilityMap2DFunction.h"
#include "../funcs/CausticFunction.h"
#include "../funcs/BeamVariationFunction.h"
#include "../funcs/MultirangeCausticFunction.h"
#include "../funcs/MultibeamCausticFunction.h"

#include <QTextStream>

// Expected values for these tests calculated by `test_files/test_plot_funcs.rez`
// It also contains test pumps and can be switched to SP mode.
// Use "Copy" on the data grid to get test values.
// For "2D Stab Map" use "Copy Graph Data" with the transposed mode turned on in export settings.

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
                               QString("%1 == %2").arg(#expr_arr1, #expr_arr2), \
                               arrToStr(arr2), arrToStr(arr1), __FILE__, __LINE__); \
            return; \
        }\
    }\
}

#define ASSERT_FUNC_OK \
    if (!func.ok()) \
        TEST_LOG(func.errorText()) \
    ASSERT_IS_TRUE(func.ok())

#define ASSERT_FUNC_RESULT_COUNT(expected_count) \
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_T), expected_count) \
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_S), expected_count) \

#define ASSERT_FUNC_RESULT(index, plane, expected_x, epsilon_x, expected_y, epsilon_y) {\
    auto res = func.result(Z::WorkPlane::Plane_ ## plane, index); \
    ASSERT_NEAR_DBL_ARR(res.x(), expected_x, epsilon_x) \
    ASSERT_NEAR_DBL_ARR(res.y(), expected_y, epsilon_y) \
}

#define ASSERT_FUNC_RESULT_TS(index, expected_x, epsilon_x, expected_yt, expected_ys, epsilon_y) {\
    auto resT = func.result(Z::WorkPlane::Plane_T, index); \
    ASSERT_NEAR_DBL_ARR(resT.x(), expected_x, epsilon_x) \
    ASSERT_NEAR_DBL_ARR(resT.y(), expected_yt, epsilon_y) \
    auto resS = func.result(Z::WorkPlane::Plane_S, index); \
    ASSERT_NEAR_DBL_ARR(resS.x(), expected_x, epsilon_x) \
    ASSERT_NEAR_DBL_ARR(resS.y(), expected_ys, epsilon_y) \
}

#define ARR(var_name, ...) \
    const QVector<double> var_name = {__VA_ARGS__};

#define ARR1(var_name, arr_offset, ...)\
    QVector<double> var_name = {__VA_ARGS__}; \
    for (int i = 0; i < var_name.size(); i++) \
        var_name[i] = var_name.at(i) - arr_offset.last();

//------------------------------------------------------------------------------

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

    TestSchema(TripType tripType = TripType::SW)
    {
        schema = new Schema;
        schema->wavelength().setValue(1000_nm);
        schema->setTripType(tripType);
        elem_M_back = makeElem<ElemCurveMirror>({{"R", 30_mm}, {"Alpha", 5_deg}});
        elem_L_foc = makeElem<ElemEmptyRange>({{"L", 56_mm}});
        elem_M_foc = makeElem<ElemCurveMirror>({{"R", 50_mm}, {"Alpha", 10_deg}});
        elem_L = makeElem<ElemEmptyRange>({{"L", 420_mm}});
        elem_M_out = makeElem<ElemFlatMirror>({});
        schema->insertElements({elem_M_back, elem_L_foc, elem_M_foc, elem_L, elem_M_out}, -1, Arg::RaiseEvents(false));
        if (tripType == TripType::SP)
        {
            auto p = new PumpParams_Waist;
            p->waist()->setValue(Z::ValueTS(110, 90, Z::Units::mkm()));
            p->distance()->setValue(Z::ValueTS(100, 100, Z::Units::mm()));
            p->MI()->setValue(Z::ValueTS(1, 1, Z::Units::none()));
            p->activate(true);
            schema->pumps()->append(p);

            auto p1 = new PumpParams_Waist;
            p1->waist()->setValue(Z::ValueTS(220, 180, Z::Units::mkm()));
            p1->distance()->setValue(Z::ValueTS(100, 100, Z::Units::mm()));
            p1->MI()->setValue(Z::ValueTS(1, 1, Z::Units::none()));
            schema->pumps()->append(p1);
        }
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

#define TEST_SCHEMA(tripType) \
    TestSchema s(tripType); \
    TEST_LOG(s.errors.join('\n')) \
    ASSERT_IS_TRUE(s.errors.isEmpty())

//------------------------------------------------------------------------------

namespace StabilityMap {

#define TEST_STAB_MAP_FUNC(mode) \
    TEST_SCHEMA(TripType::SW) \
    StabilityMapFunction func(s.schema); \
    func.setStabilityCalcMode(mode); \
    func.arg()->element = s.elem_L_foc; \
    func.arg()->parameter = s.elem_L_foc->paramLength(); \
    func.arg()->range = Z::VariableRange::withPoints(24_mm, 60_mm, 10); \
    func.calculate(); \
    ASSERT_FUNC_OK \
    ASSERT_FUNC_RESULT_COUNT(1)

TEST_METHOD(calculate_normal)
{
    TEST_STAB_MAP_FUNC(Z::Enums::StabilityCalcMode::Normal)
    ARR(_x, 0.024,0.028,0.032,0.036,0.04,0.044,0.048,0.052,0.056,0.06)
    ARR(_t, -1.86736957,3.13668409,6.74389975,8.9542774,9.76781706,9.18451871,7.20438235,3.827408,-0.946404361,-7.11705472)
    ARR(_s, -2.86711956,2.09741941,5.76059464,8.12240613,9.18285387,8.94193787,7.39965813,4.55601464,0.411007405,-5.03536357)
    ASSERT_FUNC_RESULT_TS(0, _x, 1e-7, _t, _s, 1e-7)
}

TEST_METHOD(calculate_squared)
{
    TEST_STAB_MAP_FUNC(Z::Enums::StabilityCalcMode::Squared)
    ARR(_x, 0.024,0.028,0.032,0.036,0.04,0.044,0.048,0.052,0.056,0.06)
    ARR(_t, -2.48706913,-8.83878707,-44.4801838,-79.1790838,-94.41025,-83.3553839,-50.9031251,-13.649052,0.104318786,-49.6524679)
    ARR(_s, -7.22037459,-3.39916819,-32.1844507,-64.9734814,-83.3248053,-78.9582529,-53.7549404,-19.7572694,0.831072913,-24.3548863)
    ASSERT_FUNC_RESULT_TS(0, _x, 1e-7, _t, _s, 1e-7)
}

TEST_METHOD(calculateAt)
{
    TEST_STAB_MAP_FUNC(Z::Enums::StabilityCalcMode::Squared)
    ASSERT_NEAR_TS(func.calculateAt(3_cm), -25.1621679, -15.7418247, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(45_mm), -76.8001297, -74.3141195, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(7_cm), -820.10025, -591.667213, 1e-6)
}

TEST_GROUP("StabilityMapFunction",
           ADD_TEST(calculate_normal),
           ADD_TEST(calculate_squared),
           ADD_TEST(calculateAt),
           )
} // namespace StabilityMap

//------------------------------------------------------------------------------

namespace StabilityMap2D {

#define TEST_STAB_MAP_2D_FUNC(mode) \
    TEST_SCHEMA(TripType::SW) \
    StabilityMap2DFunction func(s.schema); \
    func.setStabilityCalcMode(mode); \
    func.paramX()->element = s.elem_L_foc; \
    func.paramX()->parameter = s.elem_L_foc->paramLength(); \
    func.paramX()->range = Z::VariableRange::withPoints(0_mm, 100_mm, 10); \
    func.paramY()->element = s.elem_L; \
    func.paramY()->parameter = s.elem_L->paramLength(); \
    func.paramY()->range = Z::VariableRange::withPoints(0_mm, 500_mm, 10); \
    func.calculate(); \
    ASSERT_FUNC_OK

TEST_METHOD(calculate_normal)
{
    TEST_STAB_MAP_2D_FUNC(Z::Enums::StabilityCalcMode::Normal)
    ARR(_t,
        1,-7.23085841,-15.4617168,-23.6925752,-31.9234337,-40.1542921,-48.3851505,-56.6160089,-64.8468673,-73.0777257,
        -0.310597896,-3.90621853,-7.50183916,-11.0974598,-14.6930804,-18.2887011,-21.8843217,-25.4799423,-29.075563,-32.6711836,
        -0.95004822,-1.42487798,-1.89970773,-2.37453749,-2.84936725,-3.324197,-3.79902676,-4.27385651,-4.74868627,-5.22351603,
        -0.91835097,0.213163249,1.34467747,2.47619169,3.6077059,4.73922012,5.87073434,7.00224856,8.13376278,9.265277,
        -0.215506147,1.00790514,2.23131643,3.45472773,4.67813902,5.90155031,7.1249616,8.34837289,9.57178418,10.7951955,
        1.15848625,0.959347709,0.76020917,0.56107063,0.36193209,0.162793551,-0.0363449891,-0.235483529,-0.434622069,-0.633760608,
        3.20362622,0.0674909454,-3.06864433,-6.2047796,-9.34091487,-12.4770501,-15.6131854,-18.7493207,-21.885456,-25.0215912,
        5.91991376,-1.66766515,-9.25524406,-16.842823,-24.4304019,-32.0179808,-39.6055597,-47.1931386,-54.7807175,-62.3682964,
        9.30734888,-4.24612057,-17.79959,-31.3530595,-44.9065289,-58.4599984,-72.0134678,-85.5669373,-99.1204067,-112.673876,
        13.3659316,-7.66787532,-28.7016822,-49.7354891,-70.769296,-91.8031029,-112.83691,-133.870717,-154.904524,-175.93833
    )
    ARR(_s,
        1,-7.06653334,-15.1330667,-23.1996,-31.2661334,-39.3326667,-47.3992,-55.4657334,-63.5322667,-71.5988001,
        -0.290323866,-3.91811887,-7.54591387,-11.1737089,-14.8015039,-18.4292989,-22.0570939,-25.6848889,-29.3126839,-32.9404789,
        -0.934682126,-1.53740976,-2.14013739,-2.74286502,-3.34559266,-3.94832029,-4.55104792,-5.15377555,-5.75650318,-6.35923082,
        -0.933074782,0.0755939866,1.08426276,2.09293152,3.10160029,4.11026906,5.11893783,6.1276066,7.13627537,8.14494414,
        -0.285501834,0.920892368,2.12728657,3.33368077,4.54007497,5.74646918,6.95286338,8.15925758,9.36565178,10.572046,
        1.00803672,0.998485386,0.988934053,0.97938272,0.969831387,0.960280054,0.950728721,0.941177388,0.931626054,0.922074721,
        2.94754088,0.308373041,-2.3307948,-4.96996263,-7.60913047,-10.2482983,-12.8874661,-15.526634,-18.1658018,-20.8049697,
        5.53301064,-1.14944467,-7.83189998,-14.5143553,-21.1968106,-27.8792659,-34.5617212,-41.2441765,-47.9266318,-54.6090871,
        8.76444601,-3.37496774,-15.5143815,-27.6537952,-39.793209,-51.9326227,-64.0720365,-76.2114502,-88.350864,-100.490278,
        12.641847,-6.36819618,-25.3782393,-44.3882825,-63.3983256,-82.4083688,-101.418412,-120.428455,-139.438498,-158.448541
    )
    ASSERT_NEAR_DBL_ARR(func.resultsT(), _t, 1e-4)
    ASSERT_NEAR_DBL_ARR(func.resultsS(), _s, 1e-4)
}

TEST_METHOD(calculate_squared)
{
    TEST_STAB_MAP_2D_FUNC(Z::Enums::StabilityCalcMode::Squared)
    ARR(_t,
        0,-51.2853134,-238.064687,-560.338122,-1018.10562,-1611.36717,-2340.12279,-3204.37246,-4204.1162,-5339.354,
        0.903528947,-14.2585432,-55.2775908,-122.153614,-214.886613,-333.476587,-477.923536,-648.227461,-844.388362,-1066.40624,
        0.0974083806,-1.03027725,-2.60888947,-4.63842829,-7.1188937,-10.0502857,-13.4326043,-17.2658495,-21.5500213,-26.2851197,
        0.156631496,0.954561429,-0.808157491,-5.13152526,-12.0155419,-21.4602074,-33.4655217,-48.0314849,-65.1580969,-84.8453578,
        0.953557101,-0.015872779,-3.97877303,-10.9351437,-20.8849847,-33.828296,-49.7650778,-68.6953299,-90.6190524,-115.536245,
        -0.342090389,0.0796519725,0.422082018,0.685199748,0.869005162,0.97349826,0.998679042,0.944547508,0.811103658,0.598347491,
        -9.26322095,0.995444972,-8.41657801,-37.4992899,-86.2526907,-154.67678,-242.771559,-350.537026,-477.973183,-625.080028,
        -34.0453789,-1.78110705,-84.6595425,-282.680685,-595.844536,-1024.15109,-1567.60036,-2226.19233,-2999.92701,-3888.8044,
        -85.6267431,-17.0295399,-315.825405,-982.014338,-2015.59634,-3416.57141,-5184.93955,-7320.70075,-9823.85502,-12694.4024,
        -177.648127,-57.796312,-822.786562,-2472.61888,-5007.29325,-8426.8097,-12731.1682,-17920.3688,-23994.4114,-30953.2961
    )
    ARR(_s,
        0,-48.9358934,-228.009707,-537.221441,-976.571095,-1546.05867,-2245.68416,-3075.44758,-4035.34891,-5125.38817,
        0.915712053,-14.3516555,-55.9408161,-123.85177,-218.084517,-338.639057,-485.51539,-658.713517,-858.233436,-1084.07515,
        0.126369323,-1.36362877,-3.58018805,-6.52330853,-10.1929902,-14.5892331,-19.7120372,-25.5614024,-32.1373289,-39.4398166,
        0.12937145,0.994285549,-0.175625723,-3.38036237,-8.61992438,-15.8943118,-25.2035245,-36.5475627,-49.9264262,-65.340115,
        0.918488703,0.151957246,-3.52534815,-10.1134275,-19.6122808,-32.021908,-47.3423092,-65.5734843,-86.7154333,-110.768156,
        -0.0161380279,0.00302693317,0.0220094383,0.0408094875,0.0594270808,0.0778622182,0.0961148996,0.114185125,0.132072895,0.149778208,
        -7.68799723,0.904906068,-4.43260438,-23.7005286,-56.8988665,-104.027618,-165.086784,-240.076363,-328.996356,-431.846762,
        -29.6142067,-0.321223045,-60.3386572,-209.666509,-448.304779,-776.253467,-1193.51257,-1700.0821,-2295.96204,-2981.1524,
        -75.8155138,-10.3904072,-239.696033,-763.732391,-1582.49948,-2695.9973,-4104.22586,-5807.18515,-7804.87517,-10097.2959,
        -158.816295,-39.5539225,-643.055032,-1969.31962,-4018.34769,-6790.13925,-10284.6943,-14502.0128,-19442.0948,-25104.9403
    )
    ASSERT_NEAR_DBL_ARR(func.resultsT(), _t, 1e-4)
    ASSERT_NEAR_DBL_ARR(func.resultsS(), _s, 1e-4)
}

TEST_METHOD(calculateAt)
{
    TEST_STAB_MAP_2D_FUNC(Z::Enums::StabilityCalcMode::Normal)
    ASSERT_NEAR_TS(func.calculateAtXY(30_mm, 50_mm), -0.270565484, -0.395066604, 1e-7)
    ASSERT_NEAR_TS(func.calculateAtXY(85_mm, 70_mm), -6.18395212, -5.12656322, 1e-7)
    ASSERT_NEAR_TS(func.calculateAtXY(5_cm, 40_cm), 5.43798072, 5.86145994, 1e-7)
    ASSERT_NEAR_TS(func.calculateAtXY(12_cm, 30_cm), -184.537358, -167.424983, 1e-6)
}

TEST_GROUP("StabilityMap2DFunction",
           ADD_TEST(calculate_normal),
           ADD_TEST(calculate_squared),
           ADD_TEST(calculateAt),
           )
} // namespace StabilityMap2

//------------------------------------------------------------------------------

namespace Caustic {

#define TEST_CAUSTIC_FUNC(tripType, mode) \
    TEST_SCHEMA(tripType) \
    CausticFunction func(s.schema); \
    func.setMode(mode); \
    func.arg()->element = s.elem_L_foc; \
    func.arg()->range.start = 0_m; \
    func.arg()->range.stop = 0_m; \
    func.arg()->range.step = 0_m; \
    func.arg()->range.points = 10; \
    func.calculate(); \
    ASSERT_FUNC_OK

TEST_METHOD(calculate_resonator_W)
{
    TEST_CAUSTIC_FUNC(TripType::SW, CausticFunction::Mode::BeamRadius)
    ASSERT_FUNC_RESULT_COUNT(1)
    ARR(_x, 0,0.00622222222,0.0124444444,0.0186666667,0.0248888889,0.0311111111,0.0373333333,0.0435555556,0.0497777778,0.056)
    ARR(_t, 0.00109733338,0.000868870808,0.00064041466,0.000411975623,0.00018361758,4.58850784e-05,0.000273667661,0.000502076747,0.000730524918,0.000958984241)
    ARR(_s, 0.000615955382,0.000488698393,0.000361477458,0.000234351299,0.000107656197,2.59601224e-05,0.000148905156,0.000275837468,0.000403006971,0.000530243033)
    ASSERT_FUNC_RESULT_TS(0, _x, 1e-9, _t, _s, 1e-11)
}

TEST_METHOD(calculate_resonator_R)
{
    TEST_CAUSTIC_FUNC(TripType::SW, CausticFunction::Mode::FrontRadius)
    ASSERT_FUNC_RESULT_COUNT(2)
    ARR(_x1, 0,0.00622222222,0.0124444444,0.0186666667,0.0248888889)
    ARR(_t1, -0.0298858409,-0.0236641092,-0.0174427274,-0.0112222781,-0.00500624552)
    ARR(_s1, -0.0301145951,-0.0238973801,-0.0176836952,-0.0114793065,-0.00531754314)
    ARR(_x2, 0.0311111111,0.0373333333,0.0435555556,0.0497777778,0.056)
    ARR(_t2, 0.00127255679,0.00745683984,0.0136756567,0.0198966038,0.0261181585)
    ARR(_s2, 0.00158494703,0.00731783016,0.013503126,0.0197117691,0.0259269355)
    ASSERT_FUNC_RESULT_TS(0, _x1, 1e-9, _t1, _s1, 1e-10)
    ASSERT_FUNC_RESULT_TS(1, _x2, 1e-9, _t2, _s2, 1e-10)
}

TEST_METHOD(calculateAt_resonator_W)
{
    TEST_CAUSTIC_FUNC(TripType::SW, CausticFunction::Mode::BeamRadius)
    ASSERT_NEAR_TS(func.calculateAt(-0.01_m), 0.00109733338, 0.000615955382, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0_m), 0.00109733338, 0.000615955382, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.01_m), 0.000730164138, 0.000411450674, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.056_m), 0.000958984241, 0.000530243033, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.057_m), 0.000958984241, 0.000530243033, 1e-11)
}

TEST_METHOD(calculateAt_resonator_R)
{
    TEST_CAUSTIC_FUNC(TripType::SW, CausticFunction::Mode::FrontRadius)
    ASSERT_NEAR_TS(func.calculateAt(-0.01_m), -0.0298858409, -0.0301145951, 1e-10)
    ASSERT_NEAR_TS(func.calculateAt(0_m), -0.0298858409, -0.0301145951, 1e-10)
    ASSERT_NEAR_TS(func.calculateAt(0.01_m), -0.019886779, -0.0201241552, 1e-10)
    ASSERT_NEAR_TS(func.calculateAt(0.056_m), 0.0261181585, 0.0259269355, 1e-10)
    ASSERT_NEAR_TS(func.calculateAt(0.057_m), 0.0261181585, 0.0259269355, 1e-10)
}

TEST_METHOD(calculate_SP_W)
{
    TEST_CAUSTIC_FUNC(TripType::SP, CausticFunction::Mode::BeamRadius)
    ASSERT_FUNC_RESULT_COUNT(1)
    ARR(_x, 0,0.00622222222,0.0124444444,0.0186666667,0.0248888889,0.0311111111,0.0373333333,0.0435555556,0.0497777778,0.056)
    ARR(_t, 0.000309574733,0.000197602035,8.63751636e-05,3.2845545e-05,0.00014107107,0.000252838526,0.000364907727,0.00047706607,0.000589262655,0.000701479133)
    ARR(_s, 0.000364949149,0.000235528372,0.000106536883,2.85895426e-05,0.000154516592,0.000283767781,0.000413235635,0.000542765131,0.000672320643,0.00080188956)
    ASSERT_FUNC_RESULT_TS(0, _x, 1e-9, _t, _s, 1e-11)
}

TEST_METHOD(calculate_SP_R)
{
    TEST_CAUSTIC_FUNC(TripType::SP, CausticFunction::Mode::FrontRadius)
    ASSERT_FUNC_RESULT_COUNT(2)
    ARR(_x1, 0,0.00622222222,0.0124444444)
    ARR(_t1, -0.0171868882,-0.0109965068,-0.00489067264)
    ARR(_s1, -0.0175373544,-0.01133209,-0.00516851329)
    ARR(_x2, 0.0186666667,0.0248888889,0.0311111111,0.0373333333,0.0435555556,0.0497777778,0.056)
    ARR(_t2, 0.00215836494,0.00788109536,0.0140484522,0.0202496057,0.0264606814,0.032676006,0.0388935389)
    ARR(_s2, 0.00162417505,0.00745521932,0.0136440855,0.019853888,0.0260696237,0.0322878608,0.0385073863)
    ASSERT_FUNC_RESULT_TS(0, _x1, 1e-9, _t1, _s1, 1e-10)
    ASSERT_FUNC_RESULT_TS(1, _x2, 1e-9, _t2, _s2, 1e-10)
}

TEST_METHOD(calculateAt_SP_W)
{
    TEST_CAUSTIC_FUNC(TripType::SP, CausticFunction::Mode::BeamRadius)
    ASSERT_NEAR_TS(func.calculateAt(-0.01_m), 0.000309574733, 0.000364949149, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0_m), 0.000309574733, 0.000364949149, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.01_m), 0.000129859819, 0.000157093288, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.056_m), 0.000701479133, 0.00080188956, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.057_m), 0.000701479133, 0.00080188956, 1e-11)
}

TEST_METHOD(calculateAt_SP_R)
{
    TEST_CAUSTIC_FUNC(TripType::SP, CausticFunction::Mode::FrontRadius)
    ASSERT_NEAR_TS(func.calculateAt(-0.01_m), -0.0171868882, -0.0175373544, 1e-10)
    ASSERT_NEAR_TS(func.calculateAt(0_m), -0.0171868882, -0.0175373544, 1e-10)
    ASSERT_NEAR_TS(func.calculateAt(0.01_m), -0.00726517034, -0.00757832363, 1e-10)
    ASSERT_NEAR_TS(func.calculateAt(0.056_m), 0.0388935389, 0.0385073863, 1e-10)
    ASSERT_NEAR_TS(func.calculateAt(0.057_m), 0.0388935389, 0.0385073863, 1e-10)
}

TEST_GROUP("CausticFunction",
           ADD_TEST(calculate_resonator_W),
           ADD_TEST(calculate_resonator_R),
           ADD_TEST(calculateAt_resonator_W),
           ADD_TEST(calculateAt_resonator_R),
           ADD_TEST(calculate_SP_W),
           ADD_TEST(calculate_SP_R),
           ADD_TEST(calculateAt_SP_W),
           ADD_TEST(calculateAt_SP_R),
           )

} // namespace Caustic

//------------------------------------------------------------------------------

namespace BeamVariation {

#define TEST_BEAM_VARIATION_FUNC(trip_type) \
    TEST_SCHEMA(trip_type) \
    BeamVariationFunction func(s.schema); \
    func.arg()->element = s.elem_L_foc; \
    func.arg()->parameter = s.elem_L_foc->paramLength(); \
    func.arg()->range = Z::VariableRange::withPoints(25_mm, 27_mm, 20); \
    func.pos()->element = s.elem_M_out; \
    func.calculate(); \
    ASSERT_FUNC_OK \
    ASSERT_FUNC_RESULT_COUNT(1)

TEST_METHOD(calculate_resonator)
{
    TEST_BEAM_VARIATION_FUNC(TripType::SW)
    ARR(_tx, 0.025,0.0251052632,0.0252105263,0.0253157895,0.0254210526,0.0255263158,0.0256315789,0.0257368421,0.0258421053,0.0259473684,0.0260526316)
    ARR(_ty, 0.000474270138,0.000435605112,0.000403921206,0.000376397382,0.000351384874,0.000327751355,0.000304562815,0.000280853233,0.000255318847,0.000225580421,0.000185063238)
    ARR(_sx, 0.0254210526,0.0255263158,0.0256315789,0.0257368421,0.0258421053,0.0259473684,0.0260526316,0.0261578947,0.0262631579,0.0263684211,0.0264736842,0.0265789474,0.0266842105,0.0267894737,0.0268947368,0.027)
    ARR(_sy, 0.000930919808,0.000648199425,0.000553511537,0.000496469276,0.000455139959,0.000422142333,0.000394095366,0.000369135562,0.000346075806,0.000324042881,0.000302276294,0.000279962518,0.000255999241,0.000228440335,0.000192404036,0.000117954805)
    ASSERT_FUNC_RESULT(0, T, _tx, 1e-10, _ty, 1e-12)
    ASSERT_FUNC_RESULT(0, S, _sx, 1e-10, _sy, 1e-12)
}

TEST_METHOD(calculateAt_resonator)
{
    TEST_BEAM_VARIATION_FUNC(TripType::SW)
    ASSERT_NEAR_TS(func.calculateAt(0.0255_m), 0.000333582736, 0.000685633948, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.0253_m), 0.000380333432, Double::nan(), 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.0262_m), Double::nan(), 0.000359737034, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.0271_m), Double::nan(), Double::nan(), 1e-11)
}

TEST_METHOD(calculate_SP)
{
    TEST_BEAM_VARIATION_FUNC(TripType::SP)
    ARR(_x, 0.025,0.0251052632,0.0252105263,0.0253157895,0.0254210526,0.0255263158,0.0256315789,0.0257368421,0.0258421053,0.0259473684,0.0260526316,0.0261578947,0.0262631579,0.0263684211,0.0264736842,0.0265789474,0.0266842105,0.0267894737,0.0268947368,0.027)
    ARR(_ty, 0.00530514846,0.00527469404,0.00524424014,0.00521378675,0.00518333389,0.00515288156,0.00512242978,0.00509197856,0.0050615279,0.00503107782,0.00500062832,0.00497017942,0.00493973113,0.00490928346,0.00487883642,0.00484839002,0.00481794428,0.0047874992,0.00475705481,0.00472661111)
    ARR(_sy, 0.00632613743,0.00629208078,0.00625802439,0.00622396826,0.00618991241,0.00615585683,0.00612180153,0.00608774652,0.0060536918,0.00601963738,0.00598558326,0.00595152944,0.00591747593,0.00588342274,0.00584936987,0.00581531733,0.00578126512,0.00574721325,0.00571316173,0.00567911056)
    ASSERT_FUNC_RESULT_TS(0, _x, 1e-10, _ty, _sy, 1e-11)
}

TEST_METHOD(calculateAt_SP)
{
    TEST_BEAM_VARIATION_FUNC(TripType::SP)
    ASSERT_NEAR_TS(func.calculateAt(0.0255_m), 0.00516049459, 0.0061643707, 1e-11)
}

TEST_GROUP("BeamVariationFunction",
           ADD_TEST(calculate_resonator),
           ADD_TEST(calculateAt_resonator),
           ADD_TEST(calculate_SP),
           ADD_TEST(calculateAt_SP),
           )
} // namespace BeamVariation

//------------------------------------------------------------------------------

namespace MultirangeCaustic {

#define TEST_MULTIRANGE_CAUSTIC_FUNC(tripType, mode) \
    TEST_SCHEMA(tripType) \
    MultirangeCausticFunction func(s.schema); \
    Z::Variable v1; \
    v1.element = s.elem_L_foc; \
    v1.range.start = 0_m; \
    v1.range.stop = 0_m; \
    v1.range.step = 0_m; \
    v1.range.points = 10; \
    Z::Variable v2; \
    v2.element = s.elem_L; \
    v2.range.start = 0_m; \
    v2.range.stop = 0_m; \
    v2.range.step = 0_m; \
    v2.range.points = 10; \
    func.setArgs({v1, v2}); \
    func.setMode(mode); \
    func.calculate(); \
    ASSERT_FUNC_OK

TEST_METHOD(calculate_resonator_W)
{
    TEST_MULTIRANGE_CAUSTIC_FUNC(TripType::SW, CausticFunction::Mode::BeamRadius)
    ASSERT_FUNC_RESULT_COUNT(2)
    ARR(_x1, 0,0.00622222222,0.0124444444,0.0186666667,0.0248888889,0.0311111111,0.0373333333,0.0435555556,0.0497777778,0.056)
    ARR(_t1, 0.00109733338,0.000868870808,0.00064041466,0.000411975623,0.00018361758,4.58850784e-05,0.000273667661,0.000502076747,0.000730524918,0.000958984241)
    ARR(_s1, 0.000615955382,0.000488698393,0.000361477458,0.000234351299,0.000107656197,2.59601224e-05,0.000148905156,0.000275837468,0.000403006971,0.000530243033)
    ARR1(_x2, _x1, 0.056,0.102666667,0.149333333,0.196,0.242666667,0.289333333,0.336,0.382666667,0.429333333,0.476)
    ARR(_t2, 0.000958984241,0.000854872249,0.000751118711,0.000647895855,0.000545505019,0.000444521516,0.000346179141,0.000253569679,0.000175989015,0.000140938463)
    ARR(_s2, 0.000530243033,0.000510662424,0.000492739667,0.000476661807,0.000462621236,0.00045080835,0.000441402039,0.000434558607,0.000430400321,0.000429005269)
    ASSERT_FUNC_RESULT_TS(0, _x1, 1e-9, _t1, _s1, 1e-10)
    ASSERT_FUNC_RESULT_TS(1, _x2, 1e-9, _t2, _s2, 1e-10)
}

TEST_METHOD(calculateAt_resonator_W)
{
    TEST_MULTIRANGE_CAUSTIC_FUNC(TripType::SW, CausticFunction::Mode::BeamRadius)
    ASSERT_NEAR_TS(func.calculateAt(0.01), 0.000730164138, 0.000411450674, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.2), 0.000639081203, 0.000475375763, 1e-11)
}

TEST_METHOD(calculateAt_resonator_R)
{
    TEST_MULTIRANGE_CAUSTIC_FUNC(TripType::SW, CausticFunction::Mode::FrontRadius)
    ASSERT_NEAR_TS(func.calculateAt(0.01), -0.019886779, -0.0201241552, 1e-9)
    ASSERT_NEAR_TS(func.calculateAt(0.2), -0.290109408, -1.48727058, 1e-8)
}

TEST_METHOD(calculate_SP_W)
{
    TEST_MULTIRANGE_CAUSTIC_FUNC(TripType::SP, CausticFunction::Mode::BeamRadius)
    ASSERT_FUNC_RESULT_COUNT(2)
    ARR(_x1, 0,0.00622222222,0.0124444444,0.0186666667,0.0248888889,0.0311111111,0.0373333333,0.0435555556,0.0497777778,0.056)
    ARR(_t1, 0.000309574733,0.000197602035,8.63751636e-05,3.2845545e-05,0.00014107107,0.000252838526,0.000364907727,0.00047706607,0.000589262655,0.000701479133)
    ARR(_s1, 0.000364949149,0.000235528372,0.000106536883,2.85895426e-05,0.000154516592,0.000283767781,0.000413235635,0.000542765131,0.000672320643,0.00080188956)
    ARR1(_x2, _x1, 0.056,0.102666667,0.149333333,0.196,0.242666667,0.289333333,0.336,0.382666667,0.429333333,0.476)
    ARR(_t2, 0.000701479133,0.000214573171,0.000277676527,0.000765023397,0.00125320042,0.0017415096,0.00222986413,0.00271823957,0.00320662635,0.00369501999)
    ARR(_s2, 0.00080188956,0.000300142818,0.000206105501,0.00070725396,0.00120965771,0.00171221211,0.00221481461,0.00271743852,0.00322007382,0.00372271589)
    ASSERT_FUNC_RESULT_TS(0, _x1, 1e-9, _t1, _s1, 1e-10)
    ASSERT_FUNC_RESULT_TS(1, _x2, 1e-9, _t2, _s2, 1e-10)
}

TEST_METHOD(calculate_SP_R)
{
    TEST_MULTIRANGE_CAUSTIC_FUNC(TripType::SP, CausticFunction::Mode::FrontRadius)
    ASSERT_FUNC_RESULT_COUNT(4)
    ARR(_x1, 0,0.00622222222,0.0124444444)
    ARR(_t1, -0.0171868882,-0.0109965068,-0.00489067264)
    ARR(_s1, -0.0175373544,-0.01133209,-0.00516851329)
    ARR(_x2, 0.0186666667,0.0248888889,0.0311111111,0.0373333333,0.0435555556,0.0497777778,0.056)
    ARR(_t2, 0.00215836494,0.00788109536,0.0140484522,0.0202496057,0.0264606814,0.032676006,0.0388935389)
    ARR(_s2, 0.00162417505,0.00745521932,0.0136440855,0.019853888,0.0260696237,0.0322878608,0.0385073863)
    ARR1(_x3, _x2, 0.056,0.102666667)
    ARR(_t3, -0.0670877402,-0.0207110562)
    ARR(_s3, -0.0744975161,-0.0280011199)
    ARR1(_x4, _x2, 0.149333333,0.196,0.242666667,0.289333333,0.336,0.382666667,0.429333333,0.476)
    ARR(_t4, 0.0266919191,0.0731539898,0.119775583,0.166422462,0.21307801,0.259737554,0.306399268,0.353062291)
    ARR(_s4, 0.0193344753,0.065718396,0.112337371,0.158984352,0.20564027,0.252300164,0.298962173,0.345625439)
    ASSERT_FUNC_RESULT_TS(0, _x1, 1e-9, _t1, _s1, 1e-10)
    ASSERT_FUNC_RESULT_TS(1, _x2, 1e-9, _t2, _s2, 1e-10)
    ASSERT_FUNC_RESULT_TS(2, _x3, 1e-9, _t3, _s3, 1e-10)
    ASSERT_FUNC_RESULT_TS(3, _x4, 1e-9, _t4, _s4, 1e-9)
}

TEST_METHOD(calculateAt_SP_W)
{
    TEST_MULTIRANGE_CAUSTIC_FUNC(TripType::SP, CausticFunction::Mode::BeamRadius)
    ASSERT_NEAR_TS(func.calculateAt(0.01), 0.000129859819, 0.000157093288, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.2), 0.000806855964, 0.000750303669, 1e-11)
}

TEST_METHOD(calculateAt_SP_R)
{
    TEST_MULTIRANGE_CAUSTIC_FUNC(TripType::SP, CausticFunction::Mode::FrontRadius)
    ASSERT_NEAR_TS(func.calculateAt(0.01), -0.00726517034, -0.00757832363, 1e-10)
    ASSERT_NEAR_TS(func.calculateAt(0.2), 0.0771479866, 0.0697118023, 1e-10)
}

TEST_GROUP("MultirangeCausticFunction",
           ADD_TEST(calculate_resonator_W),
           ADD_TEST(calculateAt_resonator_W),
           ADD_TEST(calculateAt_resonator_R),
           ADD_TEST(calculate_SP_W),
           ADD_TEST(calculate_SP_R),
           ADD_TEST(calculateAt_SP_W),
           ADD_TEST(calculateAt_SP_R),
           )
} // namespace MultirangeCaustic

//------------------------------------------------------------------------------

TEST_GROUP("Plot functions",
           ADD_GROUP(StabilityMap),
           ADD_GROUP(StabilityMap2D),
           ADD_GROUP(Caustic),
           ADD_GROUP(BeamVariation),
           ADD_GROUP(MultirangeCaustic),
           )

} // namespace PlotFunctionsTests
} // namespace Tests
} // namespace Z
