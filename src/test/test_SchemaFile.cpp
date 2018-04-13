#include "testing/OriTestBase.h"
#include "TestSchemaListener.h"
#include "TestUtils.h"

namespace Z {
namespace Test {
namespace SchemaFileTests {

////////////////////////////////////////////////////////////////////////////////

TEST_METHOD(SchemaLoadingProcess_must_assign_file_name)
{
/*
    Schema schema;
    ASSERT_IS_TRUE(schema.fileName().isEmpty())
    {
        SchemaLoadingProcess p(&schema, "test_file");
        ASSERT_IS_TRUE(schema.fileName().isEmpty())
    }
    ASSERT_EQ_STR(schema.fileName(), "test_file")
*/
}

TEST_METHOD(SchemaLoadingProcess_must_raise_events)
{
/*
    SCHEMA_AND_LISTENER
    {
        SchemaLoadingProcess p(&schema, "");

        ASSERT_LISTENER_EVENT(EVENT(Loading))
        listener.reset();
    }
    ASSERT_LISTENER_EVENT(EVENT(Loaded))
*/
}

TEST_METHOD(SchemaLoadingProcess_must_block_events)
{
/*
    SCHEMA_AND_LISTENER
    {
        SchemaLoadingProcess p(&schema, "");
        listener.reset();
        ASSERT_LISTENER_NO_EVENTS

        schema.events().raise(EVENT(Changed));
        ASSERT_LISTENER_NO_EVENTS
    }
    listener.reset();
    ASSERT_LISTENER_NO_EVENTS

    schema.events().raise(EVENT(Changed));
    ASSERT_LISTENER_EVENT(EVENT(Changed))
*/
}

////////////////////////////////////////////////////////////////////////////////

TEST_GROUP("SchemaFile",
    ADD_TEST(SchemaLoadingProcess_must_assign_file_name),
    ADD_TEST(SchemaLoadingProcess_must_raise_events),
    ADD_TEST(SchemaLoadingProcess_must_block_events),
)

} // namespace SchemaFileTests
} // namespace Test
} // namespace Z
