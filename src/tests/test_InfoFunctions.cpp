#include "../math/InfoFunctions.h"
#include "../core/Schema.h"
#include "../core/Elements.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {
namespace InfoFunctionsTests {

//------------------------------------------------------------------------------

TEST_METHOD(InfoFuncRepetitionRate_RR_must_be_twice_of_SW)
{
    Schema rr;
    rr.setTripType(TripType::RR);
    rr.insertElements({new ElemEmptyRange}, -1, Arg::RaiseEvents(true));

    Schema sw;
    sw.setTripType(TripType::SW);
    sw.insertElements({new ElemEmptyRange}, -1, Arg::RaiseEvents(true));

    InfoFuncRepetitionRate rate_rr(&rr);
    rate_rr.calculate();

    InfoFuncRepetitionRate rate_sw(&sw);
    rate_sw.calculate();

    ASSERT_EQ_DBL(rate_rr.repetitonRate(), rate_sw.repetitonRate()*2)
}

TEST_METHOD(InfoFuncRepetitionRate_must_account_optical_path)
{
    Schema s1;
    auto e1 = new ElemEmptyRange;
    e1->paramLength()->setValue(Z::Value(100, Z::Units::mm()));
    s1.insertElements({e1}, -1, Arg::RaiseEvents(true));

    Schema s2;
    auto e2 = new ElemMediumRange;
    e2->paramLength()->setValue(Z::Value(100, Z::Units::mm()));
    e2->paramIor()->setValue(Z::Value(2, Z::Units::none()));
    s2.insertElements({e2}, -1, Arg::RaiseEvents(true));

    for (auto p : e1->params()) TEST_LOG(p->str())
    for (auto p : e2->params()) TEST_LOG(p->str())

    InfoFuncRepetitionRate rate1(&s1);
    rate1.calculate();

    InfoFuncRepetitionRate rate2(&s2);
    rate2.calculate();

    ASSERT_EQ_DBL(rate1.repetitonRate(), rate2.repetitonRate()*2)
}

//------------------------------------------------------------------------------

TEST_GROUP("Information functions",
           ADD_TEST(InfoFuncRepetitionRate_RR_must_be_twice_of_SW),
           ADD_TEST(InfoFuncRepetitionRate_must_account_optical_path),
)

} // namespace InfoFunctionsTests
} // namespace Tests
} // namespace Z

