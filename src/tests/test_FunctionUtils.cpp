#include "../core/Schema.h"
#include "../math/FunctionUtils.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {
namespace FunctionUtilsTests {

DECLARE_ELEMENT(TestElement, Element)
DECLARE_ELEMENT_END

TEST_METHOD(prevElem)
{
    auto e1 = new TestElement;
    auto e2 = new TestElement;
    auto e3 = new TestElement;
    auto e4 = new TestElement;
    Schema s;
    s.insertElements({e1, e2, e3, e4}, -1, Arg::RaiseEvents(false));
    e2->setDisabled(true);
    
    s.setTripType(TripType::SP);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e1), 0);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e2), 0);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e3), e1);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e4), e3);

    s.setTripType(TripType::SW);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e1), e3);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e2), 0);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e3), e1);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e4), e3);

    s.setTripType(TripType::RR);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e1), e4);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e2), 0);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e3), e1);
    ASSERT_EQ_PTR(FunctionUtils::prevElem(&s, e4), e3);
}

TEST_METHOD(nextElem)
{
    auto e1 = new TestElement;
    auto e2 = new TestElement;
    auto e3 = new TestElement;
    auto e4 = new TestElement;
    Schema s;
    s.insertElements({e1, e2, e3, e4}, -1, Arg::RaiseEvents(false));
    e2->setDisabled(true);
    
    s.setTripType(TripType::SP);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e1), e3);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e2), 0);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e3), e4);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e4), 0);

    s.setTripType(TripType::SW);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e1), e3);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e2), 0);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e3), e4);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e4), e3);

    s.setTripType(TripType::RR);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e1), e3);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e2), 0);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e3), e4);
    ASSERT_EQ_PTR(FunctionUtils::nextElem(&s, e4), e1);
}

TEST_METHOD(ior)
{

}

//------------------------------------------------------------------------------

TEST_GROUP("Function Utils",
    ADD_TEST(prevElem),
    ADD_TEST(nextElem),
    ADD_TEST(ior),
)

} // namespace FunctionUtilsTests
} // namespace Tests
} // namespace Z
