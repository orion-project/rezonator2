#include "testing/OriTestBase.h"
#include "../funcs/RoundTripCalculator.h"

namespace Z {
namespace Tests {
namespace RoundTripCalculatorTests {

DECLARE_ELEMENT(TestElem, Element) DECLARE_ELEMENT_END
DECLARE_ELEMENT(TestElemRange, ElementRange) DECLARE_ELEMENT_END

#define EL_COUNT 4
#define EL_BEG 0
#define EL_MID 2
#define EL_END 3

#define SW TripType::SW
#define SP TripType::SP
#define RR TripType::RR

void populateSchema(Schema* schema, int ref, Element* ref_elem)
{
    for (int i = 0; i < EL_COUNT; i++)
    {
        Element* el = (i == ref)? ref_elem: new TestElem;
        el->setLabel(QString::number(i));
        schema->insertElement(el);
    }
}

#define PREPARE_ROUND_TRIP(make_ref_elem)\
    auto ref_elem = make_ref_elem;\
    Schema schema;\
    schema.setTripType(tripType);\
    populateSchema(&schema, ref, ref_elem);\
    RoundTripCalculator calc(&schema, ref_elem);

//------------------------------------------------------------------------------

#define ASSERT_ROUND_TRIP_SIZE\
    if (tripType == SW)\
        ASSERT_EQ_INT(calc.roundTrip().size(), schema.count()*2-2)\
    else if (tripType == SP)\
        ASSERT_EQ_INT(calc.roundTrip().size(), ref+1)\
    else if (tripType == RR)\
        ASSERT_EQ_INT(calc.roundTrip().size(), schema.count())

TEST_CASE_METHOD(rt, TripType tripType, int ref, QString expected)
{
    PREPARE_ROUND_TRIP(new TestElem)

    calc.calcRoundTrip(true);
    TEST_LOG(calc.roundTripStr())
    ASSERT_ROUND_TRIP_SIZE
    ASSERT_EQ_STR(calc.roundTripStr().trimmed(), expected)
    ASSERT_EQ_PTR(calc.roundTrip().first(), ref_elem)

    calc.calcRoundTrip(false);
    TEST_LOG(calc.roundTripStr())
    ASSERT_ROUND_TRIP_SIZE
    ASSERT_EQ_STR(calc.roundTripStr().trimmed(), expected)
    ASSERT_EQ_PTR(calc.roundTrip().first(), ref_elem)
}

TEST_CASE(rt_sw,     rt, SW, EL_MID, "2 1 0 1 2 3")
TEST_CASE(rt_sw_beg, rt, SW, EL_BEG, "0 1 2 3 2 1")
TEST_CASE(rt_sw_end, rt, SW, EL_END, "3 2 1 0 1 2")

TEST_CASE(rt_sp,     rt, SP, EL_MID, "2 1 0")
TEST_CASE(rt_sp_beg, rt, SP, EL_BEG, "0")
TEST_CASE(rt_sp_end, rt, SP, EL_END, "3 2 1 0")

TEST_CASE(rt_rr,     rt, RR, EL_MID, "2 1 0 3")
TEST_CASE(rt_rr_beg, rt, RR, EL_BEG, "0 3 2 1")
TEST_CASE(rt_rr_end, rt, RR, EL_END, "3 2 1 0")

//------------------------------------------------------------------------------

#define ASSERT_ROUND_TRIP_MATRS\
    ASSERT_EQ_INT(calc.matrsT().size(), calc.roundTrip().size())\
    ASSERT_EQ_INT(calc.matrsS().size(), calc.roundTrip().size())\
    ASSERT_EQ_PTR(calc.matrsT().first(), ref_elem->pMt())\
    ASSERT_EQ_PTR(calc.matrsS().first(), ref_elem->pMs())\
    ASSERT_EQ_PTR(calc.matrsT().last(), calc.roundTrip().last()->pMt())\
    ASSERT_EQ_PTR(calc.matrsS().last(), calc.roundTrip().last()->pMs())

TEST_CASE_METHOD(rt_matrs_nosplit, TripType tripType, int ref, bool range, bool split)
{
    PREPARE_ROUND_TRIP((range? ((Element*)new TestElemRange): ((Element*)new TestElem)))

    calc.calcRoundTrip(split);
    ASSERT_ROUND_TRIP_MATRS
}

TEST_CASE(rt_sw_matrs_norange_nopslit,     rt_matrs_nosplit, SW, EL_MID, false, false)
TEST_CASE(rt_sw_matrs_norange_nopslit_beg, rt_matrs_nosplit, SW, EL_BEG, false, false)
TEST_CASE(rt_sw_matrs_norange_nopslit_end, rt_matrs_nosplit, SW, EL_END, false, false)
TEST_CASE(rt_sw_matrs_norange_split,       rt_matrs_nosplit, SW, EL_MID, false, true)
TEST_CASE(rt_sw_matrs_norange_split_beg,   rt_matrs_nosplit, SW, EL_BEG, false, true)
TEST_CASE(rt_sw_matrs_norange_split_end,   rt_matrs_nosplit, SW, EL_END, false, true)
TEST_CASE(rt_sw_matrs_range_nosplit,       rt_matrs_nosplit, SW, EL_MID, true, false)
TEST_CASE(rt_sw_matrs_range_nosplit_beg,   rt_matrs_nosplit, SW, EL_BEG, true, false)
TEST_CASE(rt_sw_matrs_range_nosplit_end,   rt_matrs_nosplit, SW, EL_END, true, false)

TEST_CASE(rt_sp_matrs_norange_nopslit,     rt_matrs_nosplit, SP, EL_MID, false, false)
TEST_CASE(rt_sp_matrs_norange_nopslit_beg, rt_matrs_nosplit, SP, EL_BEG, false, false)
TEST_CASE(rt_sp_matrs_norange_nopslit_end, rt_matrs_nosplit, SP, EL_END, false, false)
TEST_CASE(rt_sp_matrs_norange_split,       rt_matrs_nosplit, SP, EL_MID, false, true)
TEST_CASE(rt_sp_matrs_norange_split_beg,   rt_matrs_nosplit, SP, EL_BEG, false, true)
TEST_CASE(rt_sp_matrs_norange_split_end,   rt_matrs_nosplit, SP, EL_END, false, true)
TEST_CASE(rt_sp_matrs_range_nosplit,       rt_matrs_nosplit, SP, EL_MID, true, false)
TEST_CASE(rt_sp_matrs_range_nosplit_beg,   rt_matrs_nosplit, SP, EL_BEG, true, false)
TEST_CASE(rt_sp_matrs_range_nosplit_end,   rt_matrs_nosplit, SP, EL_END, true, false)

TEST_CASE(rt_rr_matrs_norange_nopslit,     rt_matrs_nosplit, RR, EL_MID, false, false)
TEST_CASE(rt_rr_matrs_norange_nopslit_beg, rt_matrs_nosplit, RR, EL_BEG, false, false)
TEST_CASE(rt_rr_matrs_norange_nopslit_end, rt_matrs_nosplit, RR, EL_END, false, false)
TEST_CASE(rt_rr_matrs_norange_split,       rt_matrs_nosplit, RR, EL_MID, false, true)
TEST_CASE(rt_rr_matrs_norange_split_beg,   rt_matrs_nosplit, RR, EL_BEG, false, true)
TEST_CASE(rt_rr_matrs_norange_split_end,   rt_matrs_nosplit, RR, EL_END, false, true)
TEST_CASE(rt_rr_matrs_range_nosplit,       rt_matrs_nosplit, RR, EL_MID, true, false)
TEST_CASE(rt_rr_matrs_range_nosplit_beg,   rt_matrs_nosplit, RR, EL_BEG, true, false)
TEST_CASE(rt_rr_matrs_range_nosplit_end,   rt_matrs_nosplit, RR, EL_END, true, false)

TEST_CASE_METHOD(rt_matrs_sw_rr, TripType tripType, int ref)
{
    PREPARE_ROUND_TRIP(new TestElemRange)

    calc.calcRoundTrip(true);
    ASSERT_EQ_INT(calc.matrsT().size(), calc.roundTrip().size()+1)
    ASSERT_EQ_INT(calc.matrsS().size(), calc.roundTrip().size()+1)
    ASSERT_EQ_PTR(calc.matrsT().first(), ref_elem->pMt1())
    ASSERT_EQ_PTR(calc.matrsS().first(), ref_elem->pMs1())
    ASSERT_EQ_PTR(calc.matrsT().last(), ref_elem->pMt2())
    ASSERT_EQ_PTR(calc.matrsS().last(), ref_elem->pMs2())
}

TEST_CASE_METHOD(rt_matrs_sp, int ref)
{
    TripType tripType = SP;
    PREPARE_ROUND_TRIP(new TestElemRange)

    calc.calcRoundTrip(true);
    ASSERT_EQ_INT(calc.matrsT().size(), calc.roundTrip().size())
    ASSERT_EQ_INT(calc.matrsS().size(), calc.roundTrip().size())
    ASSERT_EQ_PTR(calc.matrsT().first(), ref_elem->pMt1())
    ASSERT_EQ_PTR(calc.matrsS().first(), ref_elem->pMs1())
    ASSERT_EQ_PTR(calc.matrsT().last(), calc.roundTrip().last()->pMt())
    ASSERT_EQ_PTR(calc.matrsS().last(), calc.roundTrip().last()->pMs())
}

TEST_CASE_METHOD(rt_matrs_sp_beg, int ref)
{
    TripType tripType = SP;
    PREPARE_ROUND_TRIP(new TestElemRange)

    calc.calcRoundTrip(true);
    ASSERT_EQ_INT(calc.matrsT().size(), 1)
    ASSERT_EQ_INT(calc.matrsS().size(), 1)
    ASSERT_EQ_PTR(calc.matrsT().first(), ref_elem->pMt1())
    ASSERT_EQ_PTR(calc.matrsS().first(), ref_elem->pMs1())
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

TEST_GROUP("Round-trip Calculator",
           ADD_TEST(rt_sw),
           ADD_TEST(rt_sw_beg),
           ADD_TEST(rt_sw_end),

           ADD_TEST(rt_sp),
           ADD_TEST(rt_sp_beg),
           ADD_TEST(rt_sp_end),

           ADD_TEST(rt_rr),
           ADD_TEST(rt_rr_beg),
           ADD_TEST(rt_rr_end),


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
)

} // namespace RoundTripCalculatorTests
} // namespace Tests
} // namespace Z

