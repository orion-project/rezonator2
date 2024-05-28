#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../math/RoundTripCalculator.h"
#include "../tests/TestUtils.h"

#include "core/OriTemplates.h"

namespace Z {
namespace Tests {
namespace RoundTripCalculatorTests {

namespace  {
DECLARE_ELEMENT(TestElem, Element) DECLARE_ELEMENT_END
DECLARE_ELEMENT(TestElemRange, ElementRange) DECLARE_ELEMENT_END
}

static const int EL_COUNT = 4;
static const int EL_BEG = 0;
static const int EL_MID = 2;
static const int EL_END = 3;

static const TripType SW = TripType::SW;
static const TripType SP = TripType::SP;
static const TripType RR = TripType::RR;

BOOL_PARAM(UseRange)
BOOL_PARAM(DoSplit)
INT_PARAM(RefIndex)

/// Test schema and its round-trip calculator.
struct TestData
{
    QSharedPointer<Schema> schema;
    QSharedPointer<RoundTripCalculator> calc;

    /// Make a test schema containing `EL_COUNT` of `TestElem`s.
    /// Only element at `refIndex` will be a specified `refElem`.
    TestData(TripType tripType, int refIndex, Element* refElem)
    {
        schema.reset(new Schema);
        schema->setTripType(tripType);

        Elements elems;
        for (int i = 0; i < EL_COUNT; i++)
        {
            Element* el = (i == refIndex)? refElem: new TestElem;
            el->setLabel(QString::number(i));
            elems << el;
        }
        schema->insertElements(elems, -1, Arg::RaiseEvents(false));

        calc.reset(new RoundTripCalculator(schema.data(), refElem));
    }

    /// Make a test schema from specified list of elements.
    TestData(TripType tripType, const RefIndex& refIndex, std::initializer_list<Element*> elems)
    {
        schema.reset(new Schema);
        schema->setTripType(tripType);
        schema->insertElements(elems, -1, Arg::RaiseEvents(false));
        calc.reset(new RoundTripCalculator(schema.data(), schema->element(refIndex)));
    }
};

//------------------------------------------------------------------------------
/**
    Test which elements are contained in the round-trip.
*/
namespace RoundTripElements {

TEST_CASE_METHOD(rt_elems, TripType tripType, int refIndex, QString expectedRoundTripDescr)
{
    auto refElem = new TestElem;
    TestData d(tripType, refIndex, refElem);

    int expectedRoundTripSize = -1;
    switch (tripType)
    {
    case SW: expectedRoundTripSize = d.schema->count()*2 - 2; break;
    case SP: expectedRoundTripSize = refIndex + 1; break;
    case RR: expectedRoundTripSize = d.schema->count();
    }

    d.calc->calcRoundTrip(true);
    TEST_LOG(d.calc->roundTripStr())
    ASSERT_EQ_INT(d.calc->roundTrip().size(), expectedRoundTripSize)
    ASSERT_EQ_STR(d.calc->roundTripStr().trimmed(), expectedRoundTripDescr)
    ASSERT_EQ_PTR(d.calc->roundTrip().first(), refElem)

    d.calc->calcRoundTrip(false);
    TEST_LOG(d.calc->roundTripStr())
    ASSERT_EQ_INT(d.calc->roundTrip().size(), expectedRoundTripSize)
    ASSERT_EQ_STR(d.calc->roundTripStr().trimmed(), expectedRoundTripDescr)
    ASSERT_EQ_PTR(d.calc->roundTrip().first(), refElem)
}

TEST_CASE(rt_sw_elems,     rt_elems, SW, EL_MID, "2 1 0 1 2 3")
TEST_CASE(rt_sw_elems_beg, rt_elems, SW, EL_BEG, "0 1 2 3 2 1")
TEST_CASE(rt_sw_elems_end, rt_elems, SW, EL_END, "3 2 1 0 1 2")

TEST_CASE(rt_sp_elems,     rt_elems, SP, EL_MID, "2 1 0")
TEST_CASE(rt_sp_elems_beg, rt_elems, SP, EL_BEG, "0")
TEST_CASE(rt_sp_elems_end, rt_elems, SP, EL_END, "3 2 1 0")

TEST_CASE(rt_rr_elems,     rt_elems, RR, EL_MID, "2 1 0 3")
TEST_CASE(rt_rr_elems_beg, rt_elems, RR, EL_BEG, "0 3 2 1")
TEST_CASE(rt_rr_elems_end, rt_elems, RR, EL_END, "3 2 1 0")

TEST_METHOD(must_be_empty_when_null_ref)
{
    Schema schema;
    schema.insertElements({new TestElem, new TestElem, new TestElem}, -1, Arg::RaiseEvents(false));
    RoundTripCalculator calc(&schema, nullptr);
    calc.calcRoundTrip();
    ASSERT_IS_TRUE(calc.isEmpty())
}

TEST_METHOD(must_be_empty_when_invalid_ref)
{
    Schema schema;
    schema.insertElements({new TestElem, new TestElem, new TestElem}, -1, Arg::RaiseEvents(false));
    TestElem elemNotInSchema;
    RoundTripCalculator calc(&schema, &elemNotInSchema);
    calc.calcRoundTrip();
    ASSERT_IS_TRUE(calc.isEmpty())
}

TEST_METHOD(must_be_empty_when_disabled_ref)
{
    Schema schema;
    schema.insertElements({new TestElem, new TestElem, new TestElem}, -1, Arg::RaiseEvents(false));
    schema.element(1)->setDisabled(true);
    RoundTripCalculator calc(&schema, schema.element(1));
    calc.calcRoundTrip();
    ASSERT_IS_TRUE(calc.isEmpty())
}

TEST_METHOD(disabled_elems_must_be_skipped)
{
    Schema schema;
    schema.insertElements({new TestElem, new TestElem, new TestElem}, -1, Arg::RaiseEvents(false));
    schema.element(1)->setDisabled(true);
    RoundTripCalculator calc(&schema, schema.element(2));
    calc.calcRoundTrip();
    ASSERT_IS_TRUE(calc.roundTrip().contains(schema.element(0)))
    ASSERT_IS_FALSE(calc.roundTrip().contains(schema.element(1)))
    ASSERT_IS_TRUE(calc.roundTrip().contains(schema.element(2)))
}

TEST_GROUP("Elements in round-trip",
           ADD_TEST(rt_sw_elems),
           ADD_TEST(rt_sw_elems_beg),
           ADD_TEST(rt_sw_elems_end),
           ADD_TEST(rt_sp_elems),
           ADD_TEST(rt_sp_elems_beg),
           ADD_TEST(rt_sp_elems_end),
           ADD_TEST(rt_rr_elems),
           ADD_TEST(rt_rr_elems_beg),
           ADD_TEST(rt_rr_elems_end),
           ADD_TEST(must_be_empty_when_null_ref),
           ADD_TEST(must_be_empty_when_invalid_ref),
           ADD_TEST(must_be_empty_when_disabled_ref),
           ADD_TEST(disabled_elems_must_be_skipped),
           )
}

//------------------------------------------------------------------------------
/**
    Test which end matrices are contained in the round-trip
    when splitting of the reference range element is not required
    or the reference element is not a range so splitting is impossible.
*/
namespace RoundTripEndMatrices_NoRange_NoSplit {

TEST_CASE_METHOD(rt_matrs_nosplit, TripType tripType, int refIndex, UseRange&& useRangeAsRef, DoSplit&& doSplitRefRange)
{
    auto refElem = useRangeAsRef ? dynamic_cast<Element*>(new TestElemRange) : dynamic_cast<Element*>(new TestElem);
    TestData d(tripType, refIndex, refElem);

    d.calc->calcRoundTrip(doSplitRefRange);
    ASSERT_EQ_INT(d.calc->matrsT().size(), d.calc->roundTrip().size())
    ASSERT_EQ_INT(d.calc->matrsS().size(), d.calc->roundTrip().size())
    ASSERT_EQ_PTR(d.calc->matrsT().first(), refElem->pMt())
    ASSERT_EQ_PTR(d.calc->matrsS().first(), refElem->pMs())
    if (tripType == SW && refIndex == d.schema->count()-1)
    {
        // In SW-schemas, if the reference element is the last one in the schema
        // then the last element of the round-trip will be an element
        // before-the-last in the schema and from back-propagation stage:
        //
        //          Forward part of round-trip
        //        <=== M3 <==== M2 <===== M1 <==== M0 (round-trip begin)
        //
        //   schema:  [  ]-----[  ]------[  ]-----[  ] (the last is ref)
        //
        //        ==========> M4_inv ==> M5_inv (round-trip end)
        //          Backward part of round-trip (inv matrices are taken)
        //
        ASSERT_EQ_PTR(d.calc->matrsT().last(), d.calc->roundTrip().last()->pMt_inv())
        ASSERT_EQ_PTR(d.calc->matrsS().last(), d.calc->roundTrip().last()->pMs_inv())
    }
    else
    {
        ASSERT_EQ_PTR(d.calc->matrsT().last(), d.calc->roundTrip().last()->pMt())
        ASSERT_EQ_PTR(d.calc->matrsS().last(), d.calc->roundTrip().last()->pMs())
    }
}

TEST_CASE(rt_sw_matrs_norange_nopslit,     rt_matrs_nosplit, SW, EL_MID, UseRange(false), DoSplit(false))
TEST_CASE(rt_sw_matrs_norange_nopslit_beg, rt_matrs_nosplit, SW, EL_BEG, UseRange(false), DoSplit(false))
TEST_CASE(rt_sw_matrs_norange_nopslit_end, rt_matrs_nosplit, SW, EL_END, UseRange(false), DoSplit(false))
TEST_CASE(rt_sw_matrs_norange_split,       rt_matrs_nosplit, SW, EL_MID, UseRange(false), DoSplit(true))
TEST_CASE(rt_sw_matrs_norange_split_beg,   rt_matrs_nosplit, SW, EL_BEG, UseRange(false), DoSplit(true))
TEST_CASE(rt_sw_matrs_norange_split_end,   rt_matrs_nosplit, SW, EL_END, UseRange(false), DoSplit(true))
TEST_CASE(rt_sw_matrs_range_nosplit,       rt_matrs_nosplit, SW, EL_MID, UseRange(true), DoSplit(false))
TEST_CASE(rt_sw_matrs_range_nosplit_beg,   rt_matrs_nosplit, SW, EL_BEG, UseRange(true), DoSplit(false))
TEST_CASE(rt_sw_matrs_range_nosplit_end,   rt_matrs_nosplit, SW, EL_END, UseRange(true), DoSplit(false))

TEST_CASE(rt_sp_matrs_norange_nopslit,     rt_matrs_nosplit, SP, EL_MID, UseRange(false), DoSplit(false))
TEST_CASE(rt_sp_matrs_norange_nopslit_beg, rt_matrs_nosplit, SP, EL_BEG, UseRange(false), DoSplit(false))
TEST_CASE(rt_sp_matrs_norange_nopslit_end, rt_matrs_nosplit, SP, EL_END, UseRange(false), DoSplit(false))
TEST_CASE(rt_sp_matrs_norange_split,       rt_matrs_nosplit, SP, EL_MID, UseRange(false), DoSplit(true))
TEST_CASE(rt_sp_matrs_norange_split_beg,   rt_matrs_nosplit, SP, EL_BEG, UseRange(false), DoSplit(true))
TEST_CASE(rt_sp_matrs_norange_split_end,   rt_matrs_nosplit, SP, EL_END, UseRange(false), DoSplit(true))
TEST_CASE(rt_sp_matrs_range_nosplit,       rt_matrs_nosplit, SP, EL_MID, UseRange(true), DoSplit(false))
TEST_CASE(rt_sp_matrs_range_nosplit_beg,   rt_matrs_nosplit, SP, EL_BEG, UseRange(true), DoSplit(false))
TEST_CASE(rt_sp_matrs_range_nosplit_end,   rt_matrs_nosplit, SP, EL_END, UseRange(true), DoSplit(false))

TEST_CASE(rt_rr_matrs_norange_nopslit,     rt_matrs_nosplit, RR, EL_MID, UseRange(false), DoSplit(false))
TEST_CASE(rt_rr_matrs_norange_nopslit_beg, rt_matrs_nosplit, RR, EL_BEG, UseRange(false), DoSplit(false))
TEST_CASE(rt_rr_matrs_norange_nopslit_end, rt_matrs_nosplit, RR, EL_END, UseRange(false), DoSplit(false))
TEST_CASE(rt_rr_matrs_norange_split,       rt_matrs_nosplit, RR, EL_MID, UseRange(false), DoSplit(true))
TEST_CASE(rt_rr_matrs_norange_split_beg,   rt_matrs_nosplit, RR, EL_BEG, UseRange(false), DoSplit(true))
TEST_CASE(rt_rr_matrs_norange_split_end,   rt_matrs_nosplit, RR, EL_END, UseRange(false), DoSplit(true))
TEST_CASE(rt_rr_matrs_range_nosplit,       rt_matrs_nosplit, RR, EL_MID, UseRange(true), DoSplit(false))
TEST_CASE(rt_rr_matrs_range_nosplit_beg,   rt_matrs_nosplit, RR, EL_BEG, UseRange(true), DoSplit(false))
TEST_CASE(rt_rr_matrs_range_nosplit_end,   rt_matrs_nosplit, RR, EL_END, UseRange(true), DoSplit(false))

TEST_GROUP("Round-trip end matrices (no range, no-split)",
           ADD_TEST(rt_sw_matrs_norange_nopslit),
           ADD_TEST(rt_sw_matrs_norange_nopslit_beg),
           ADD_TEST(rt_sw_matrs_norange_nopslit_end),
           ADD_TEST(rt_sw_matrs_norange_split),
           ADD_TEST(rt_sw_matrs_norange_split_beg),
           ADD_TEST(rt_sw_matrs_norange_split_end),
           ADD_TEST(rt_sw_matrs_range_nosplit),
           ADD_TEST(rt_sw_matrs_range_nosplit_beg),
           ADD_TEST(rt_sw_matrs_range_nosplit_end),

           ADD_TEST(rt_sp_matrs_norange_nopslit),
           ADD_TEST(rt_sp_matrs_norange_nopslit_beg),
           ADD_TEST(rt_sp_matrs_norange_nopslit_end),
           ADD_TEST(rt_sp_matrs_norange_split),
           ADD_TEST(rt_sp_matrs_norange_split_beg),
           ADD_TEST(rt_sp_matrs_norange_split_end),
           ADD_TEST(rt_sp_matrs_range_nosplit),
           ADD_TEST(rt_sp_matrs_range_nosplit_beg),
           ADD_TEST(rt_sp_matrs_range_nosplit_end),

           ADD_TEST(rt_rr_matrs_norange_nopslit),
           ADD_TEST(rt_rr_matrs_norange_nopslit_beg),
           ADD_TEST(rt_rr_matrs_norange_nopslit_end),
           ADD_TEST(rt_rr_matrs_norange_split),
           ADD_TEST(rt_rr_matrs_norange_split_beg),
           ADD_TEST(rt_rr_matrs_norange_split_end),
           ADD_TEST(rt_rr_matrs_range_nosplit),
           ADD_TEST(rt_rr_matrs_range_nosplit_beg),
           ADD_TEST(rt_rr_matrs_range_nosplit_end),
           )
}

//------------------------------------------------------------------------------
/**
    Test which matrices are the round-trip ends
    when the reference element is a range and range split is required.
*/
namespace RoundTripEndMatrices_RangeSplit {

TEST_CASE_METHOD(rt_matrs_sw_rr, TripType tripType, int refIndex)
{
    ElementRange *refElem = new TestElemRange;
    TestData d(tripType, refIndex, refElem);

    d.calc->calcRoundTrip(true);
    ASSERT_EQ_INT(d.calc->matrsT().size(), d.calc->roundTrip().size()+1)
    ASSERT_EQ_INT(d.calc->matrsS().size(), d.calc->roundTrip().size()+1)
    ASSERT_EQ_PTR(d.calc->matrsT().first(), refElem->pMt1())
    ASSERT_EQ_PTR(d.calc->matrsS().first(), refElem->pMs1())
    ASSERT_EQ_PTR(d.calc->matrsT().last(), refElem->pMt2())
    ASSERT_EQ_PTR(d.calc->matrsS().last(), refElem->pMs2())
}

TEST_CASE_METHOD(rt_matrs_sp, int refIndex)
{
    ElementRange *refElem = new TestElemRange;
    TestData d(SP, refIndex, refElem);

    d.calc->calcRoundTrip(true);
    ASSERT_EQ_INT(d.calc->matrsT().size(), d.calc->roundTrip().size())
    ASSERT_EQ_INT(d.calc->matrsS().size(), d.calc->roundTrip().size())
    ASSERT_EQ_PTR(d.calc->matrsT().first(), refElem->pMt1())
    ASSERT_EQ_PTR(d.calc->matrsS().first(), refElem->pMs1())
    ASSERT_EQ_PTR(d.calc->matrsT().last(), d.calc->roundTrip().last()->pMt())
    ASSERT_EQ_PTR(d.calc->matrsS().last(), d.calc->roundTrip().last()->pMs())
}

TEST_CASE_METHOD(rt_matrs_sp_beg, int refIndex)
{
    ElementRange *refElem = new TestElemRange;
    TestData d(SP, refIndex, refElem);

    d.calc->calcRoundTrip(true);
    ASSERT_EQ_INT(d.calc->matrsT().size(), 1)
    ASSERT_EQ_INT(d.calc->matrsS().size(), 1)
    ASSERT_EQ_PTR(d.calc->matrsT().first(), refElem->pMt1())
    ASSERT_EQ_PTR(d.calc->matrsS().first(), refElem->pMs1())
}

TEST_CASE(rt_sw_matrs,     rt_matrs_sw_rr, SW, EL_MID)
TEST_CASE(rt_sw_matrs_beg, rt_matrs_sw_rr, SW, EL_BEG)
TEST_CASE(rt_sw_matrs_end, rt_matrs_sw_rr, SW, EL_END)

TEST_CASE(rt_sp_matrs,     rt_matrs_sp,        EL_MID)
TEST_CASE(rt_sp_matrs_beg, rt_matrs_sp_beg,    EL_BEG)
TEST_CASE(rt_sp_matrs_end, rt_matrs_sp,        EL_END)

TEST_CASE(rt_rr_matrs,     rt_matrs_sw_rr, RR, EL_MID)
TEST_CASE(rt_rr_matrs_beg, rt_matrs_sw_rr, RR, EL_BEG)
TEST_CASE(rt_rr_matrs_end, rt_matrs_sw_rr, RR, EL_END)

TEST_GROUP("Round-trip end matrices (range split)",
           ADD_TEST(rt_sw_matrs),
           ADD_TEST(rt_sw_matrs_beg),
           ADD_TEST(rt_sw_matrs_end),

           ADD_TEST(rt_sp_matrs),
           ADD_TEST(rt_sp_matrs_beg),
           ADD_TEST(rt_sp_matrs_end),

           ADD_TEST(rt_rr_matrs),
           ADD_TEST(rt_rr_matrs_beg),
           ADD_TEST(rt_rr_matrs_end),
           )
}
//------------------------------------------------------------------------------

namespace GeneralFuncs {

/// RT-Calculator allowing for setting matrices directly
class TestRoundTripCalculator : public RoundTripCalculator {
public:
    TestRoundTripCalculator(Schema *s) : RoundTripCalculator(s, nullptr) {}
    TestRoundTripCalculator(Schema *s, const Matrix &mt, const Matrix &ms) : RoundTripCalculator(s, nullptr) {
        _mt = mt;
        _ms = ms;
    }
    void addT(std::initializer_list<const Matrix*> matrs) {
        for (auto &m : matrs) _matrsT.append(m);
    }
    void addS(std::initializer_list<const Matrix*> matrs) {
        for (auto &m : matrs) _matrsS.append(m);
    }
};

// Calculation: $PROJECT/calc/RoundTripCalculator.py
TEST_METHOD(multMatrix)
{
    Matrix L1_t(1, 0.05, 0, 1);
    Matrix L1_s = L1_t;

    Matrix F1_t(1, 0, -10.1543, 1);
    Matrix F1_s(1, 0, -9.84808, 1);

    Matrix L2_t(1, 0.075, 0, 1);
    Matrix L2_s = L2_t;

    Matrix Cr1_t(1, 0.0650857, 0, 1);
    Matrix Cr1_s(1, 0.0676818, 0, 1);

    Matrix L3_t(1, 0.1, 0, 1);
    Matrix L3_s = L3_t;

    Schema schema;
    TestRoundTripCalculator c(&schema);
    c.addT({&L3_t, &Cr1_t, &L2_t, &F1_t, &L1_t});
    c.addS({&L3_s, &Cr1_s, &L2_s, &F1_s, &L1_s});
    c.multMatrix("test::multMatrix");
    ASSERT_MATRIX_NEAR(c.Mt(), -1.4379022, 0.1681906, -10.1543000, 0.4922850, 1e-7)
    ASSERT_MATRIX_NEAR(c.Ms(), -1.3899498, 0.1731843, -9.8480800, 0.5075960, 1e-7)
}

#define ASSERT_STABILITY(c, expected_t, expected_s) \
{\
    auto s = c.isStable();\
    ASSERT_IS_TRUE(s.T == expected_t)\
    ASSERT_IS_TRUE(s.S == expected_s)\
}

// Calculation: $PROJECT/bin/test_files/test_stability.rez (Ref:M_out)
TEST_METHOD(stability_stable)
{
    // L_foc = 56mm
    Matrix mt(-0.946404361, 0.0201553431, -5.17573851, -0.946404361);
    Matrix ms(0.411007405, 0.527102031, -1.57668319, 0.411007405);

    Schema schema;
    TestRoundTripCalculator c(&schema, mt, ms);
    c.setStabilityCalcMode(Z::Enums::StabilityCalcMode::Normal);
    ASSERT_NEAR_TS(c.stability(), -0.946404361, 0.411007405, 1e-9);
    ASSERT_STABILITY(c, true, true)

    c.setStabilityCalcMode(Z::Enums::StabilityCalcMode::Squared);
    ASSERT_NEAR_TS(c.stability(), 0.104318786, 0.831072913, 1e-9);
    ASSERT_STABILITY(c, true, true)
}

TEST_METHOD(stability_unstable_S)
{
    // L_foc = 55mm
    Matrix mt(0.378002292, 0.517339649, -1.65677282, 0.378002292);
    Matrix ms(1.56926206, 0.957958409, 1.52677132, 1.56926206);

    Schema schema;
    TestRoundTripCalculator c(&schema, mt, ms);
    c.setStabilityCalcMode(Z::Enums::StabilityCalcMode::Normal);
    ASSERT_NEAR_TS(c.stability(), 0.378002292, 1.56926206, 1e-8);
    ASSERT_STABILITY(c, true, false)

    c.setStabilityCalcMode(Z::Enums::StabilityCalcMode::Squared);
    ASSERT_NEAR_TS(c.stability(), 0.857114268, -1.46258343, 1e-7);
    ASSERT_STABILITY(c, true, false)
}

TEST_METHOD(stability_unstable_T)
{
    // L_foc = 57mm
    Matrix mt(-2.35811339, -0.511546559, -8.91551057, -2.35811339);
    Matrix ms(-0.828582488, 0.0641496036, -4.88625094, -0.828582488);

    Schema schema;
    TestRoundTripCalculator c(&schema, mt, ms);
    c.setStabilityCalcMode(Z::Enums::StabilityCalcMode::Normal);
    ASSERT_NEAR_TS(c.stability(), -2.35811339, -0.828582488, 1e-8);
    ASSERT_STABILITY(c, false, true)

    c.setStabilityCalcMode(Z::Enums::StabilityCalcMode::Squared);
    ASSERT_NEAR_TS(c.stability(), -4.56069875, 0.313451061, 1e-7);
    ASSERT_STABILITY(c, false, true)
}

TEST_METHOD(stability_unstable)
{
    // L_foc = 54mm
    Matrix mt(1.61510657, 0.980006359, 1.64138652, 1.61510657);
    Matrix ms(2.64618149, 1.35671874, 4.42411261, 2.64618149);

    Schema schema;
    TestRoundTripCalculator c(&schema, mt, ms);
    c.setStabilityCalcMode(Z::Enums::StabilityCalcMode::Normal);
    ASSERT_NEAR_TS(c.stability(), 1.61510657, 2.64618149, 1e-8);
    ASSERT_STABILITY(c, false, false)

    c.setStabilityCalcMode(Z::Enums::StabilityCalcMode::Squared);
    ASSERT_NEAR_TS(c.stability(), -1.60856923, -6.00227648, 1e-8);
    ASSERT_STABILITY(c, false, false)
}

TEST_GROUP("General functionality",
           ADD_TEST(multMatrix),
           ADD_TEST(stability_stable),
           ADD_TEST(stability_unstable_S),
           ADD_TEST(stability_unstable_T),
           ADD_TEST(stability_unstable),
           )
}

//------------------------------------------------------------------------------
/**
    Test that a schema containig a composite element combined from interfaces
    gives the same round-trip matrix as the schema containig regular analog of the interfaced element.
*/
namespace InterfacedElements {

#define RUN_TEST_DATA(d) \
    d.calc->calcRoundTrip(false); \
    d.calc->multMatrix("test::InterfacedElements"); \
    TEST_LOG(d.calc->roundTripStr()); \
    TEST_LOG("Mt = " + d.calc->Mt().str()); \
    TEST_LOG("Ms = " + d.calc->Ms().str());

namespace Normal {
TEST_CASE_METHOD(rt_ifaces_normal, TripType tripType, const RefIndex& refIndex1, const RefIndex& refIndex2)
{
    TestData d1(tripType, refIndex1, {
                    makeElem<ElemEmptyRange>("L1", "L = 100mm"),
                    makeElem<ElemPlate>("Cr", "L=100mm; n = 2"),
                    makeElem<ElemEmptyRange>("L2", "L = 100mm"),
                });
    TestData d2(tripType, refIndex2, {
                    makeElem<ElemEmptyRange>("L1", "L = 100mm"),
                    makeElem<ElemNormalInterface>("Cr_in", "n1 = 1; n2 = 2"),
                    makeElem<ElemMediumRange>("Cr", "L = 100mm; n = 2"),
                    makeElem<ElemNormalInterface>("Cr_out", "n1 = 2; n2 = 1"),
                    makeElem<ElemEmptyRange>("L2", "L = 100mm"),
                });
    RUN_TEST_DATA(d1)
    RUN_TEST_DATA(d2)
    ASSERT_EQ_MATRIX(d1.calc->Mt(), d2.calc->Mt())
    ASSERT_EQ_MATRIX(d1.calc->Ms(), d2.calc->Ms())
}

TEST_CASE(rt_ifaces_normal_sw_0, rt_ifaces_normal, SW, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_normal_sw_1, rt_ifaces_normal, SW, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_normal_sw_2, rt_ifaces_normal, SW, RefIndex(2), RefIndex(4))
TEST_CASE(rt_ifaces_normal_rr_0, rt_ifaces_normal, RR, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_normal_rr_1, rt_ifaces_normal, RR, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_normal_rr_2, rt_ifaces_normal, RR, RefIndex(2), RefIndex(4))
TEST_CASE(rt_ifaces_normal_sp_0, rt_ifaces_normal, SP, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_normal_sp_1, rt_ifaces_normal, SP, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_normal_sp_2, rt_ifaces_normal, SP, RefIndex(2), RefIndex(4))

TEST_GROUP("Normal",
    ADD_TEST(rt_ifaces_normal_sw_0),
    ADD_TEST(rt_ifaces_normal_sw_1),
    ADD_TEST(rt_ifaces_normal_sw_2),
    ADD_TEST(rt_ifaces_normal_rr_0),
    ADD_TEST(rt_ifaces_normal_rr_1),
    ADD_TEST(rt_ifaces_normal_rr_2),
    ADD_TEST(rt_ifaces_normal_sp_0),
    ADD_TEST(rt_ifaces_normal_sp_1),
    ADD_TEST(rt_ifaces_normal_sp_2),
)
} // namespace Normal

namespace Brewster {
// Calculation: $PROJECT/calc/RoundTripCalculator.py (def mult_matrices_interface_brewster)
TEST_CASE_METHOD(rt_ifaces_brewster, TripType tripType, const RefIndex& refIndex1, const RefIndex& refIndex2)
{
    TestData d1(tripType, refIndex1, {
                    makeElem<ElemEmptyRange>("L1", "L = 100mm"),
                    makeElem<ElemBrewsterCrystal>("Cr", "L=100mm; n = 2"),
                    makeElem<ElemEmptyRange>("L2", "L = 100mm"),
                });
    TestData d2(tripType, refIndex2, {
                    makeElem<ElemEmptyRange>("L1", "L = 100mm"),
                    makeElem<ElemBrewsterInterface>("Cr_in", "n1 = 1; n2 = 2"),
                    makeElem<ElemMediumRange>("Cr", "L = 100mm; n = 2"),
                    makeElem<ElemBrewsterInterface>("Cr_out", "n1 = 2; n2 = 1"),
                    makeElem<ElemEmptyRange>("L2", "L = 100mm"),
                });
    RUN_TEST_DATA(d1)
    RUN_TEST_DATA(d2)
    ASSERT_EQ_MATRIX(d1.calc->Mt(), d2.calc->Mt())
    ASSERT_EQ_MATRIX(d1.calc->Ms(), d2.calc->Ms())
}

TEST_CASE(rt_ifaces_brewster_sw_0, rt_ifaces_brewster, SW, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_brewster_sw_1, rt_ifaces_brewster, SW, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_brewster_sw_2, rt_ifaces_brewster, SW, RefIndex(2), RefIndex(4))
TEST_CASE(rt_ifaces_brewster_rr_0, rt_ifaces_brewster, RR, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_brewster_rr_1, rt_ifaces_brewster, RR, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_brewster_rr_2, rt_ifaces_brewster, RR, RefIndex(2), RefIndex(4))
TEST_CASE(rt_ifaces_brewster_sp_0, rt_ifaces_brewster, SP, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_brewster_sp_1, rt_ifaces_brewster, SP, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_brewster_sp_2, rt_ifaces_brewster, SP, RefIndex(2), RefIndex(4))

TEST_GROUP("Brewster",
    ADD_TEST(rt_ifaces_brewster_sw_0),
    ADD_TEST(rt_ifaces_brewster_sw_1),
    ADD_TEST(rt_ifaces_brewster_sw_2),
    ADD_TEST(rt_ifaces_brewster_rr_0),
    ADD_TEST(rt_ifaces_brewster_rr_1),
    ADD_TEST(rt_ifaces_brewster_rr_2),
    ADD_TEST(rt_ifaces_brewster_sp_0),
    ADD_TEST(rt_ifaces_brewster_sp_1),
    ADD_TEST(rt_ifaces_brewster_sp_2),
)
} // namespace Brewster

namespace Tilted {
TEST_CASE_METHOD(rt_ifaces_tilted, TripType tripType, const RefIndex& refIndex1, const RefIndex& refIndex2)
{
    TestData d1(tripType, refIndex1, {
                    makeElem<ElemEmptyRange>("L1", "L = 100mm"),
                    makeElem<ElemTiltedCrystal>("Cr", "L=100mm; n = 2; Alpha=1rad"),
                    makeElem<ElemEmptyRange>("L2", "L = 100mm"),
                });
    TestData d2(tripType, refIndex2, {
                    makeElem<ElemEmptyRange>("L1", "L = 100mm"),
                    makeElem<ElemTiltedInterface>("Cr_in", "n1 = 1; n2 = 2; Alpha=1rad"),
                    makeElem<ElemMediumRange>("Cr", "L = 100mm; n = 2"),
                    makeElem<ElemTiltedInterface>("Cr_out", QString("n1 = 2; n2 = 1; Alpha=%1rad").arg(asin(sin(1)/2.0))),
                    makeElem<ElemEmptyRange>("L2", "L = 100mm"),
                });
    TestData d3(tripType, refIndex2, {
                    makeElem<ElemEmptyRange>("L1", "L = 100mm"),
                    makeElem<ElemTiltedInterface>("Cr_in", "n1 = 1; n2 = 2; Alpha=1rad"),
                    makeElem<ElemMediumRange>("Cr", "L = 100mm; n = 2"),
                    makeElem<ElemTiltedInterface>("Cr_out", "n1 = 2; n2 = 1; Alpha=-1rad"),
                    makeElem<ElemEmptyRange>("L2", "L = 100mm"),
                });
    RUN_TEST_DATA(d1)
    RUN_TEST_DATA(d2)
    RUN_TEST_DATA(d3)
    ASSERT_NEAR_MATRIX(d1.calc->Mt(), d2.calc->Mt(), 1e-6)
    ASSERT_NEAR_MATRIX(d1.calc->Ms(), d2.calc->Ms(), 1e-6)
    ASSERT_NEAR_MATRIX(d1.calc->Mt(), d3.calc->Mt(), 1e-6)
    ASSERT_NEAR_MATRIX(d1.calc->Ms(), d3.calc->Ms(), 1e-6)
}

TEST_CASE(rt_ifaces_tilted_sw_0, rt_ifaces_tilted, SW, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_tilted_sw_1, rt_ifaces_tilted, SW, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_tilted_sw_2, rt_ifaces_tilted, SW, RefIndex(2), RefIndex(4))
TEST_CASE(rt_ifaces_tilted_rr_0, rt_ifaces_tilted, RR, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_tilted_rr_1, rt_ifaces_tilted, RR, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_tilted_rr_2, rt_ifaces_tilted, RR, RefIndex(2), RefIndex(4))
TEST_CASE(rt_ifaces_tilted_sp_0, rt_ifaces_tilted, SP, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_tilted_sp_1, rt_ifaces_tilted, SP, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_tilted_sp_2, rt_ifaces_tilted, SP, RefIndex(2), RefIndex(4))

TEST_GROUP("Tilted",
    ADD_TEST(rt_ifaces_tilted_sw_0),
    ADD_TEST(rt_ifaces_tilted_sw_1),
    ADD_TEST(rt_ifaces_tilted_sw_2),
    ADD_TEST(rt_ifaces_tilted_rr_0),
    ADD_TEST(rt_ifaces_tilted_rr_1),
    ADD_TEST(rt_ifaces_tilted_rr_2),
    ADD_TEST(rt_ifaces_tilted_sp_0),
    ADD_TEST(rt_ifaces_tilted_sp_1),
    ADD_TEST(rt_ifaces_tilted_sp_2),
)
} // namespace Tilted

namespace Spherical {
TEST_CASE_METHOD(rt_ifaces_spherical, TripType tripType, const RefIndex& refIndex1, const RefIndex& refIndex2)
{
    TestData d1(tripType, refIndex1, {
                    makeElem<ElemEmptyRange>("L1", "L = 100mm"),
                    makeElem<ElemThickLens>("Cr", "L=100mm; n = 2; R1=-90mm; R2=150mm"),
                    makeElem<ElemEmptyRange>("L2", "L = 100mm"),
                });
    TestData d2(tripType, refIndex2, {
                    makeElem<ElemEmptyRange>("L1", "L = 100mm"),
                    makeElem<ElemSphericalInterface>("Cr_in", "n1 = 1; n2 = 2; R=-90mm"),
                    makeElem<ElemMediumRange>("Cr", "L = 100mm; n = 2"),
                    makeElem<ElemSphericalInterface>("Cr_out", "n1 = 2; n2 = 1; R=150mm"),
                    makeElem<ElemEmptyRange>("L2", "L = 100mm"),
                });
    RUN_TEST_DATA(d1)
    RUN_TEST_DATA(d2)
    ASSERT_EQ_MATRIX(d1.calc->Mt(), d2.calc->Mt())
    ASSERT_EQ_MATRIX(d1.calc->Ms(), d2.calc->Ms())
}

TEST_CASE(rt_ifaces_spherical_sw_0, rt_ifaces_spherical, SW, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_spherical_sw_1, rt_ifaces_spherical, SW, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_spherical_sw_2, rt_ifaces_spherical, SW, RefIndex(2), RefIndex(4))
TEST_CASE(rt_ifaces_spherical_rr_0, rt_ifaces_spherical, RR, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_spherical_rr_1, rt_ifaces_spherical, RR, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_spherical_rr_2, rt_ifaces_spherical, RR, RefIndex(2), RefIndex(4))
TEST_CASE(rt_ifaces_spherical_sp_0, rt_ifaces_spherical, SP, RefIndex(0), RefIndex(0))
TEST_CASE(rt_ifaces_spherical_sp_1, rt_ifaces_spherical, SP, RefIndex(1), RefIndex(3))
TEST_CASE(rt_ifaces_spherical_sp_2, rt_ifaces_spherical, SP, RefIndex(2), RefIndex(4))

TEST_GROUP("Spherical",
    ADD_TEST(rt_ifaces_spherical_sw_0),
    ADD_TEST(rt_ifaces_spherical_sw_1),
    ADD_TEST(rt_ifaces_spherical_sw_2),
    ADD_TEST(rt_ifaces_spherical_rr_0),
    ADD_TEST(rt_ifaces_spherical_rr_1),
    ADD_TEST(rt_ifaces_spherical_rr_2),
    ADD_TEST(rt_ifaces_spherical_sp_0),
    ADD_TEST(rt_ifaces_spherical_sp_1),
    ADD_TEST(rt_ifaces_spherical_sp_2),
)
} // namespace Shperical

#undef RUN_TEST_DATA

TEST_GROUP("Composite interfaced elements",
    ADD_GROUP(Normal),
    ADD_GROUP(Brewster),
    ADD_GROUP(Tilted),
    ADD_GROUP(Spherical),
)
} // namespace InterfacedElements

//------------------------------------------------------------------------------

TEST_GROUP("RoundTripCalculator",
           ADD_GROUP(RoundTripElements),
           ADD_GROUP(RoundTripEndMatrices_NoRange_NoSplit),
           ADD_GROUP(RoundTripEndMatrices_RangeSplit),
           ADD_GROUP(GeneralFuncs),
           ADD_GROUP(InterfacedElements),
           )
} // namespace RoundTripCalculatorTests
} // namespace Tests
} // namespace Z

