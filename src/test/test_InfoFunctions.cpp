#include "testing/OriTestBase.h"
#include "../funcs/InfoFunctions.h"
#include "../core/Schema.h"
#include "../core/Elements.h"

namespace Z {
namespace Test {
namespace InfoFunctionsTests {

//------------------------------------------------------------------------------

TEST_METHOD(InfoFuncRepetitionRate_RR_must_be_twice_of_SW)
{
    Schema rr;
    rr.setTripType(Schema::RR);
    rr.insertElement(new ElemEmptyRange);

    Schema sw;
    sw.setTripType(Schema::SW);
    sw.insertElement(new ElemEmptyRange);

    InfoFuncRepetitionRate rate_rr(&rr);
    rate_rr.calculate();

    InfoFuncRepetitionRate rate_sw(&sw);
    rate_sw.calculate();

    ASSERT_EQ_DBL(rate_rr.result(), rate_sw.result()*2)
}

TEST_METHOD(InfoFuncRepetitionRate_must_account_optical_path)
{
    Schema s1;
    auto e1 = new ElemEmptyRange;
    e1->paramLength()->setValue(Z::Value(100, Z::Units::mm()));
    s1.insertElement(e1);

    Schema s2;
    auto e2 = new ElemMediumRange;
    e2->paramLength()->setValue(Z::Value(100, Z::Units::mm()));
    e2->paramIor()->setValue(Z::Value(2, Z::Units::none()));
    s2.insertElement(e2);

    for (auto p : e1->params()) TEST_LOG(p->str())
    for (auto p : e2->params()) TEST_LOG(p->str())

    InfoFuncRepetitionRate rate1(&s1);
    rate1.calculate();

    InfoFuncRepetitionRate rate2(&s2);
    rate2.calculate();

    ASSERT_EQ_DBL(rate1.result(), rate2.result()*2)
}

//------------------------------------------------------------------------------

TEST_GROUP("Information functions",
           ADD_TEST(InfoFuncRepetitionRate_RR_must_be_twice_of_SW),
           ADD_TEST(InfoFuncRepetitionRate_must_account_optical_path),
)

} // namespace InfoFunctionsTests
} // namespace Test
} // namespace Z

