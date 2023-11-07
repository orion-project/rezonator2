#include "../core/Report.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {
namespace ReportTests {

////////////////////////////////////////////////////////////////////////////////

TEST_METHOD(warning)
{
    Z::Report r;
    ASSERT_IS_FALSE(r.hasErrors())
    ASSERT_IS_FALSE(r.hasWarnings())

    r.warning("test warning");
    TEST_LOG(r.str())
    ASSERT_IS_FALSE(r.hasErrors())
    ASSERT_IS_TRUE(r.hasWarnings())
}

TEST_METHOD(error)
{
    Z::Report r;
    ASSERT_IS_FALSE(r.hasErrors())
    ASSERT_IS_FALSE(r.hasWarnings())

    r.error("test warning");
    TEST_LOG(r.str())
    ASSERT_IS_TRUE(r.hasErrors())
    ASSERT_IS_FALSE(r.hasWarnings())
}

////////////////////////////////////////////////////////////////////////////////

TEST_GROUP("Report",
    ADD_TEST(warning),
    ADD_TEST(error),
)

} // namespace ReportTests
} // namespace Tests
} // namespace Z
