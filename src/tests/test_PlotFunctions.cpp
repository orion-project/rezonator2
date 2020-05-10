#include "testing/OriTestBase.h"
#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../funcs/StabilityMapFunction.h"
#include "../funcs/StabilityMap2DFunction.h"
#include "../funcs/CausticFunction.h"

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
    {-1.86736957,3.13668409,6.74389975,8.9542774,9.76781706,9.18451871,7.20438235,3.827408,-0.946404361,-7.11705472},
    {-2.86711956,2.09741941,5.76059464,8.12240613,9.18285387,8.94193787,7.39965813,4.55601464,0.411007405,-5.03536357})
TEST_CASE(calculate_squared, calculate, Z::Enums::StabilityCalcMode::Squared,
    {-2.48706913,-8.83878707,-44.4801838,-79.1790838,-94.41025,-83.3553839,-50.9031251,-13.649052,0.104318786,-49.6524679},
    {-7.22037459,-3.39916819,-32.1844507,-64.9734814,-83.3248053,-78.9582529,-53.7549404,-19.7572694,0.831072913,-24.3548863})

TEST_METHOD(calculateAt)
{
    TEST_STAB_MAP_FUNC(Z::Enums::StabilityCalcMode::Squared)
    ASSERT_NEAR_TS(func.calculateAt(3_cm), -25.1621679, -15.7418247, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(45_mm), -76.8001297, -74.3141195, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(7_cm), -820.10025, -591.667213, 1e-6)
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
    TEST_SCHEMA(TripType::SW) \
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
    {1,-7.23085841,-15.4617168,-23.6925752,-31.9234337,-40.1542921,-48.3851505,-56.6160089,-64.8468673,-73.0777257,
     -0.310597896,-3.90621853,-7.50183916,-11.0974598,-14.6930804,-18.2887011,-21.8843217,-25.4799423,-29.075563,-32.6711836,
     -0.95004822,-1.42487798,-1.89970773,-2.37453749,-2.84936725,-3.324197,-3.79902676,-4.27385651,-4.74868627,-5.22351603,
     -0.91835097,0.213163249,1.34467747,2.47619169,3.6077059,4.73922012,5.87073434,7.00224856,8.13376278,9.265277,
     -0.215506147,1.00790514,2.23131643,3.45472773,4.67813902,5.90155031,7.1249616,8.34837289,9.57178418,10.7951955,
     1.15848625,0.959347709,0.76020917,0.56107063,0.36193209,0.162793551,-0.0363449891,-0.235483529,-0.434622069,-0.633760608,
     3.20362622,0.0674909454,-3.06864433,-6.2047796,-9.34091487,-12.4770501,-15.6131854,-18.7493207,-21.885456,-25.0215912,
     5.91991376,-1.66766515,-9.25524406,-16.842823,-24.4304019,-32.0179808,-39.6055597,-47.1931386,-54.7807175,-62.3682964,
     9.30734888,-4.24612057,-17.79959,-31.3530595,-44.9065289,-58.4599984,-72.0134678,-85.5669373,-99.1204067,-112.673876,
     13.3659316,-7.66787532,-28.7016822,-49.7354891,-70.769296,-91.8031029,-112.83691,-133.870717,-154.904524,-175.93833},
    {1,-7.06653334,-15.1330667,-23.1996,-31.2661334,-39.3326667,-47.3992,-55.4657334,-63.5322667,-71.5988001,
     -0.290323866,-3.91811887,-7.54591387,-11.1737089,-14.8015039,-18.4292989,-22.0570939,-25.6848889,-29.3126839,-32.9404789,
     -0.934682126,-1.53740976,-2.14013739,-2.74286502,-3.34559266,-3.94832029,-4.55104792,-5.15377555,-5.75650318,-6.35923082,
     -0.933074782,0.0755939866,1.08426276,2.09293152,3.10160029,4.11026906,5.11893783,6.1276066,7.13627537,8.14494414,
     -0.285501834,0.920892368,2.12728657,3.33368077,4.54007497,5.74646918,6.95286338,8.15925758,9.36565178,10.572046,
     1.00803672,0.998485386,0.988934053,0.97938272,0.969831387,0.960280054,0.950728721,0.941177388,0.931626054,0.922074721,
     2.94754088,0.308373041,-2.3307948,-4.96996263,-7.60913047,-10.2482983,-12.8874661,-15.526634,-18.1658018,-20.8049697,
     5.53301064,-1.14944467,-7.83189998,-14.5143553,-21.1968106,-27.8792659,-34.5617212,-41.2441765,-47.9266318,-54.6090871,
     8.76444601,-3.37496774,-15.5143815,-27.6537952,-39.793209,-51.9326227,-64.0720365,-76.2114502,-88.350864,-100.490278,
     12.641847,-6.36819618,-25.3782393,-44.3882825,-63.3983256,-82.4083688,-101.418412,-120.428455,-139.438498,-158.448541})
TEST_CASE(calculate_squared, calculate, Z::Enums::StabilityCalcMode::Squared,
    {0,-51.2853134,-238.064687,-560.338122,-1018.10562,-1611.36717,-2340.12279,-3204.37246,-4204.1162,-5339.354,
     0.903528947,-14.2585432,-55.2775908,-122.153614,-214.886613,-333.476587,-477.923536,-648.227461,-844.388362,-1066.40624,
     0.0974083806,-1.03027725,-2.60888947,-4.63842829,-7.1188937,-10.0502857,-13.4326043,-17.2658495,-21.5500213,-26.2851197,
     0.156631496,0.954561429,-0.808157491,-5.13152526,-12.0155419,-21.4602074,-33.4655217,-48.0314849,-65.1580969,-84.8453578,
     0.953557101,-0.015872779,-3.97877303,-10.9351437,-20.8849847,-33.828296,-49.7650778,-68.6953299,-90.6190524,-115.536245,
     -0.342090389,0.0796519725,0.422082018,0.685199748,0.869005162,0.97349826,0.998679042,0.944547508,0.811103658,0.598347491,
     -9.26322095,0.995444972,-8.41657801,-37.4992899,-86.2526907,-154.67678,-242.771559,-350.537026,-477.973183,-625.080028,
     -34.0453789,-1.78110705,-84.6595425,-282.680685,-595.844536,-1024.15109,-1567.60036,-2226.19233,-2999.92701,-3888.8044,
     -85.6267431,-17.0295399,-315.825405,-982.014338,-2015.59634,-3416.57141,-5184.93955,-7320.70075,-9823.85502,-12694.4024,
     -177.648127,-57.796312,-822.786562,-2472.61888,-5007.29325,-8426.8097,-12731.1682,-17920.3688,-23994.4114,-30953.2961},
    {0,-48.9358934,-228.009707,-537.221441,-976.571095,-1546.05867,-2245.68416,-3075.44758,-4035.34891,-5125.38817,
     0.915712053,-14.3516555,-55.9408161,-123.85177,-218.084517,-338.639057,-485.51539,-658.713517,-858.233436,-1084.07515,
     0.126369323,-1.36362877,-3.58018805,-6.52330853,-10.1929902,-14.5892331,-19.7120372,-25.5614024,-32.1373289,-39.4398166,
     0.12937145,0.994285549,-0.175625723,-3.38036237,-8.61992438,-15.8943118,-25.2035245,-36.5475627,-49.9264262,-65.340115,
     0.918488703,0.151957246,-3.52534815,-10.1134275,-19.6122808,-32.021908,-47.3423092,-65.5734843,-86.7154333,-110.768156,
     -0.0161380279,0.00302693317,0.0220094383,0.0408094875,0.0594270808,0.0778622182,0.0961148996,0.114185125,0.132072895,0.149778208,
     -7.68799723,0.904906068,-4.43260438,-23.7005286,-56.8988665,-104.027618,-165.086784,-240.076363,-328.996356,-431.846762,
     -29.6142067,-0.321223045,-60.3386572,-209.666509,-448.304779,-776.253467,-1193.51257,-1700.0821,-2295.96204,-2981.1524,
     -75.8155138,-10.3904072,-239.696033,-763.732391,-1582.49948,-2695.9973,-4104.22586,-5807.18515,-7804.87517,-10097.2959,
     -158.816295,-39.5539225,-643.055032,-1969.31962,-4018.34769,-6790.13925,-10284.6943,-14502.0128,-19442.0948,-25104.9403})

TEST_METHOD(calculateAt)
{
    TEST_STAB_MAP_2D_FUNC(Z::Enums::StabilityCalcMode::Normal)
    ASSERT_NEAR_TS(func.calculateAt(30_mm, 50_mm), -0.270565484, -0.395066604, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(85_mm, 70_mm), -6.18395212, -5.12656322, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(5_cm, 40_cm), 5.43798072, 5.86145994, 1e-7)
    ASSERT_NEAR_TS(func.calculateAt(12_cm, 30_cm), -184.537358, -167.424983, 1e-6)
}

TEST_GROUP("2D Stability Map",
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
    func.arg()->range.points = 10; \
    func.calculate();

TEST_METHOD(calculate_resonator_W)
{
    TEST_CAUSTIC_FUNC(TripType::SW, CausticFunction::Mode::BeamRadius)
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_T), 1)
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_S), 1)
    auto resT = func.result(Z::WorkPlane::Plane_T, 0);
    auto resS = func.result(Z::WorkPlane::Plane_S, 0);
    QVector<double> expectedX = {0,0.00622222222,0.0124444444,0.0186666667,0.0248888889,0.0311111111,0.0373333333,0.0435555556,0.0497777778,0.056};
    QVector<double> expectedT = {0.00109733338,0.000868870808,0.00064041466,0.000411975623,0.00018361758,4.58850784e-05,0.000273667661,0.000502076747,0.000730524918,0.000958984241};
    QVector<double> expectedS = {0.000615955382,0.000488698393,0.000361477458,0.000234351299,0.000107656197,2.59601224e-05,0.000148905156,0.000275837468,0.000403006971,0.000530243033};
    ASSERT_NEAR_DBL_ARR(resT.x(), expectedX, 1e-9)
    ASSERT_NEAR_DBL_ARR(resT.y(), expectedT, 1e-11)
    ASSERT_NEAR_DBL_ARR(resS.x(), expectedX, 1e-9)
    ASSERT_NEAR_DBL_ARR(resS.y(), expectedS, 1e-11)
}

TEST_METHOD(calculate_resonator_R)
{
    TEST_CAUSTIC_FUNC(TripType::SW, CausticFunction::Mode::FontRadius)
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_T), 2)
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_S), 2)
    auto resT1 = func.result(Z::WorkPlane::Plane_T, 0);
    auto resS1 = func.result(Z::WorkPlane::Plane_S, 0);
    QVector<double> expectedX1 = {0,0.00622222222,0.0124444444,0.0186666667,0.0248888889};
    QVector<double> expectedT1 = {-0.0298858409,-0.0236641092,-0.0174427274,-0.0112222781,-0.00500624552};
    QVector<double> expectedS1 = {-0.0301145951,-0.0238973801,-0.0176836952,-0.0114793065,-0.00531754314};
    ASSERT_NEAR_DBL_ARR(resT1.x(), expectedX1, 1e-9)
    ASSERT_NEAR_DBL_ARR(resT1.y(), expectedT1, 1e-10)
    ASSERT_NEAR_DBL_ARR(resS1.x(), expectedX1, 1e-9)
    ASSERT_NEAR_DBL_ARR(resS1.y(), expectedS1, 1e-10)
    auto resT2 = func.result(Z::WorkPlane::Plane_T, 1);
    auto resS2 = func.result(Z::WorkPlane::Plane_S, 1);
    QVector<double> expectedX2 = {0.0311111111,0.0373333333,0.0435555556,0.0497777778,0.056};
    QVector<double> expectedT2 = {0.00127255679,0.00745683984,0.0136756567,0.0198966038,0.0261181585};
    QVector<double> expectedS2 = {0.00158494703,0.00731783016,0.013503126,0.0197117691,0.0259269355};
    ASSERT_NEAR_DBL_ARR(resT2.x(), expectedX2, 1e-9)
    ASSERT_NEAR_DBL_ARR(resT2.y(), expectedT2, 1e-10)
    ASSERT_NEAR_DBL_ARR(resS2.x(), expectedX2, 1e-9)
    ASSERT_NEAR_DBL_ARR(resS2.y(), expectedS2, 1e-10)
}

TEST_METHOD(calculateAt_resonator)
{
    TEST_CAUSTIC_FUNC(TripType::SW, CausticFunction::Mode::BeamRadius)
    ASSERT_NEAR_TS(func.calculateAt(-0.01), 0.00109733338, 0.000615955382, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0), 0.00109733338, 0.000615955382, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.01), 0.000730164138, 0.000411450674, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.056), 0.000958984241, 0.000530243033, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.057), 0.000958984241, 0.000530243033, 1e-11)
}

TEST_METHOD(calculate_SP_W)
{
    TEST_CAUSTIC_FUNC(TripType::SP, CausticFunction::Mode::BeamRadius)
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_T), 1)
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_S), 1)
    auto resT = func.result(Z::WorkPlane::Plane_T, 0);
    auto resS = func.result(Z::WorkPlane::Plane_S, 0);
    QVector<double> expectedX = {0,0.00622222222,0.0124444444,0.0186666667,0.0248888889,0.0311111111,0.0373333333,0.0435555556,0.0497777778,0.056};
    QVector<double> expectedT = {0.000309574733,0.000197602035,8.63751636e-05,3.2845545e-05,0.00014107107,0.000252838526,0.000364907727,0.00047706607,0.000589262655,0.000701479133};
    QVector<double> expectedS = {0.000364949149,0.000235528372,0.000106536883,2.85895426e-05,0.000154516592,0.000283767781,0.000413235635,0.000542765131,0.000672320643,0.00080188956};
    ASSERT_NEAR_DBL_ARR(resT.x(), expectedX, 1e-9)
    ASSERT_NEAR_DBL_ARR(resT.y(), expectedT, 1e-11)
    ASSERT_NEAR_DBL_ARR(resS.x(), expectedX, 1e-9)
    ASSERT_NEAR_DBL_ARR(resS.y(), expectedS, 1e-11)
}

TEST_METHOD(calculate_SP_R)
{
    TEST_CAUSTIC_FUNC(TripType::SP, CausticFunction::Mode::FontRadius)
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_T), 2)
    ASSERT_EQ_INT(func.resultCount(Z::WorkPlane::Plane_S), 2)
    auto resT1 = func.result(Z::WorkPlane::Plane_T, 0);
    auto resS1 = func.result(Z::WorkPlane::Plane_S, 0);
    QVector<double> expectedX1 = {0,0.00622222222,0.0124444444};
    QVector<double> expectedT1 = {-0.0171868882,-0.0109965068,-0.00489067264};
    QVector<double> expectedS1 = {-0.0175373544,-0.01133209,-0.00516851329};
    ASSERT_NEAR_DBL_ARR(resT1.x(), expectedX1, 1e-9)
    ASSERT_NEAR_DBL_ARR(resT1.y(), expectedT1, 1e-10)
    ASSERT_NEAR_DBL_ARR(resS1.x(), expectedX1, 1e-9)
    ASSERT_NEAR_DBL_ARR(resS1.y(), expectedS1, 1e-10)
    auto resT2 = func.result(Z::WorkPlane::Plane_T, 1);
    auto resS2 = func.result(Z::WorkPlane::Plane_S, 1);
    QVector<double> expectedX2 = {0.0186666667,0.0248888889,0.0311111111,0.0373333333,0.0435555556,0.0497777778,0.056};
    QVector<double> expectedT2 = {0.00215836494,0.00788109536,0.0140484522,0.0202496057,0.0264606814,0.032676006,0.0388935389};
    QVector<double> expectedS2 = {0.00162417505,0.00745521932,0.0136440855,0.019853888,0.0260696237,0.0322878608,0.0385073863};
    ASSERT_NEAR_DBL_ARR(resT2.x(), expectedX2, 1e-9)
    ASSERT_NEAR_DBL_ARR(resT2.y(), expectedT2, 1e-10)
    ASSERT_NEAR_DBL_ARR(resS2.x(), expectedX2, 1e-9)
    ASSERT_NEAR_DBL_ARR(resS2.y(), expectedS2, 1e-10)
}

TEST_METHOD(calculateAt_SP)
{
    TEST_CAUSTIC_FUNC(TripType::SP, CausticFunction::Mode::BeamRadius)
    ASSERT_NEAR_TS(func.calculateAt(-0.01), 0.000309574733, 0.000364949149, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0), 0.000309574733, 0.000364949149, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.01), 0.000129859819, 0.000157093288, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.056), 0.000701479133, 0.00080188956, 1e-11)
    ASSERT_NEAR_TS(func.calculateAt(0.057), 0.000701479133, 0.00080188956, 1e-11)
}

TEST_GROUP("Caustic",
           ADD_TEST(calculate_resonator_W),
           ADD_TEST(calculate_resonator_R),
           ADD_TEST(calculateAt_resonator),
           ADD_TEST(calculate_SP_W),
           ADD_TEST(calculate_SP_R),
           ADD_TEST(calculateAt_SP),
           )

} // namespace Caustic

//------------------------------------------------------------------------------

TEST_GROUP("Plot functions",
           ADD_GROUP(StabilityMap),
           ADD_GROUP(StabilityMap2D),
           ADD_GROUP(Caustic),
           )

} // namespace PlotFunctionsTests
} // namespace Tests
} // namespace Z

