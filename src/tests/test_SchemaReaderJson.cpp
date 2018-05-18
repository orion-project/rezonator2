#include "../core/Schema.h"
#include "../io/SchemaReaderJson.h"
#include "TestUtils.h"

#include <QApplication>
#include <QFile>

namespace Z {
namespace Tests {
namespace SchemaReaderJsonTests {

TEST_METHOD(read_pumps)
{
    // TODO
    ASSERT_FAIL("TODO")
}

//------------------------------------------------------------------------------

TEST_GROUP("SchemaReaderJson",
    ADD_TEST(read_pumps),
)

} // namespace SchemaReaderJsonTests
} // namespace Tests
} // namespace Z
