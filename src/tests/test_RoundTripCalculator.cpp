#include "TestUtils.h"
#include "../funcs/RoundTripCalculator.h"

namespace Z {
namespace Tests {
namespace RoundTripCalculatorTests {

DECLARE_ELEMENT(TestElem, Element) DECLARE_ELEMENT_END
DECLARE_ELEMENT(TestElemRange, ElementRange) DECLARE_ELEMENT_END

static const int EL_COUNT = 4;
static const int EL_BEG = 0;
static const int EL_MID = 2;
static const int EL_END = 3;

static const TripType SW = TripType::SW;
static const TripType SP = TripType::SP;
static const TripType RR = TripType::RR;

#define BOOL_PARAM(param_name) \
    struct param_name { \
        param_name(bool v) : value(v) {} \
        operator bool() const { return value; } \
        bool value; \
    };

struct TestData
{
    QSharedPointer<Schema> schema;
    QSharedPointer<RoundTripCalculator> calc;

    TestData(TripType tripType, int refIndex, Element* refElem)
    {
        schema.reset(new Schema);
        schema->setTripType(tripType);

        for (int i = 0; i < EL_COUNT; i++)
        {
            Element* el = (i == refIndex)? refElem: new TestElem;
            el->setLabel(QString::number(i));
            schema->insertElement(el);
        }

        calc.reset(new RoundTripCalculator(schema.data(), refElem));
    }
};

//------------------------------------------------------------------------------
//
// Test which elements are contained in the round-trip.

TEST_CASE_METHOD(rt_elems, TripType tripType, int refIndex, QString expectedRoundTripDescr)
{
    auto refElem = new TestElem;
    TestData d(tripType, refIndex, refElem);

    int expectedRoundTripSize;
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

//------------------------------------------------------------------------------
//
// Test which end matrices are contained in the round-trip
// when splitting of the reference range element is not required
// or the reference element is not a range so splitting is impossible.

BOOL_PARAM(UseRange)
BOOL_PARAM(DoSplit)
TEST_CASE_METHOD(rt_matrs_nosplit, TripType tripType, int refIndex, UseRange&& useRangeAsRef, DoSplit&& doSplitRefRange)
{
    auto refElem = useRangeAsRef ? (Element*)(new TestElemRange) : (Element*)(new TestElem);
    TestData d(tripType, refIndex, refElem);

    d.calc->calcRoundTrip(doSplitRefRange);
    ASSERT_EQ_INT(d.calc->matrsT().size(), d.calc->roundTrip().size())
    ASSERT_EQ_INT(d.calc->matrsS().size(), d.calc->roundTrip().size())
    ASSERT_EQ_PTR(d.calc->matrsT().first(), refElem->pMt())
    ASSERT_EQ_PTR(d.calc->matrsS().first(), refElem->pMs())
    ASSERT_EQ_PTR(d.calc->matrsT().last(), d.calc->roundTrip().last()->pMt())
    ASSERT_EQ_PTR(d.calc->matrsS().last(), d.calc->roundTrip().last()->pMs())
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

//------------------------------------------------------------------------------
//
// Test which matrices are the round-trip ends
// when the reference element is a range and range split is required.

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

TEST_CASE(rt_sp_matrs,     rt_matrs_sp,          EL_MID)
TEST_CASE(rt_sp_matrs_beg, rt_matrs_sp_beg,      EL_BEG)
TEST_CASE(rt_sp_matrs_end, rt_matrs_sp,          EL_END)

TEST_CASE(rt_rr_matrs,     rt_matrs_sw_rr, RR, EL_MID)
TEST_CASE(rt_rr_matrs_beg, rt_matrs_sw_rr, RR, EL_BEG)
TEST_CASE(rt_rr_matrs_end, rt_matrs_sw_rr, RR, EL_END)

//------------------------------------------------------------------------------

// Calculation: $PROJECT/calc/RoundTripCalculator.py
TEST_METHOD(mult_matrices)
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
    class TestRoundTripCalculator : public RoundTripCalculator {
    public:
        TestRoundTripCalculator(Schema*s) : RoundTripCalculator(s) {}
        void addT(std::initializer_list<Matrix*> matrs) {
            for (auto &m : matrs) _matrsT.append(m);
        }
        void addS(std::initializer_list<Matrix*> matrs) {
            for (auto &m : matrs) _matrsS.append(m);
        }
    };
    TestRoundTripCalculator c(&schema);
    c.addT({&L3_t, &Cr1_t, &L2_t, &F1_t, &L1_t});
    c.addS({&L3_s, &Cr1_s, &L2_s, &F1_s, &L1_s});
    c.multMatrix();
    ASSERT_MATRIX_NEAR(c.Mt(), -1.4379022, 0.1681906, -10.1543000, 0.4922850, 1e-7)
    ASSERT_MATRIX_NEAR(c.Ms(), -1.3899498, 0.1731843, -9.8480800, 0.5075960, 1e-7)
}

//------------------------------------------------------------------------------

TEST_GROUP("Round-trip Calculator",
           ADD_TEST(rt_sw_elems),
           ADD_TEST(rt_sw_elems_beg),
           ADD_TEST(rt_sw_elems_end),
           ADD_TEST(rt_sp_elems),
           ADD_TEST(rt_sp_elems_beg),
           ADD_TEST(rt_sp_elems_end),
           ADD_TEST(rt_rr_elems),
           ADD_TEST(rt_rr_elems_beg),
           ADD_TEST(rt_rr_elems_end),


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


           ADD_TEST(rt_sw_matrs),
           ADD_TEST(rt_sw_matrs_beg),
           ADD_TEST(rt_sw_matrs_end),

           ADD_TEST(rt_sp_matrs),
           ADD_TEST(rt_sp_matrs_beg),
           ADD_TEST(rt_sp_matrs_end),

           ADD_TEST(rt_rr_matrs),
           ADD_TEST(rt_rr_matrs_beg),
           ADD_TEST(rt_rr_matrs_end),


           ADD_TEST(mult_matrices)
)

} // namespace RoundTripCalculatorTests
} // namespace Tests
} // namespace Z

