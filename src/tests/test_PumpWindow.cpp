#include "TestSchemaListener.h"
#include "../PumpWindow.h"

namespace Z {
namespace Tests {
namespace PumpWindowTests {

#define ASSERT_LISTENER(expected_pump, ...) { \
    ASSERT_IS_TRUE(listener.schema == &schema) \
    ASSERT_IS_TRUE(listener.pump == expected_pump) \
    TEST_LOG(listener.eventsStr())\
    ASSERT_IS_TRUE(listener.checkEvents({__VA_ARGS__})) \
}

//------------------------------------------------------------------------------

class TestPumpWindow : public PumpWindow
{
public:
    TestPumpWindow(Schema* schema): PumpWindow(schema) {}

    PumpParams *pump;

protected:
    PumpParams* makeNewPumpDlg() override { return pump; }
};

//------------------------------------------------------------------------------

TEST_METHOD(createPump_first_pump_must_be_active)
{
    Schema schema;
    TestPumpWindow window(&schema);
    PumpParams_Front p;
    window.pump = &p;

    ASSERT_IS_NULL(schema.activePump())
    window.createPump();
    ASSERT_EQ_PTR(schema.activePump(), &p);
}

TEST_METHOD(createPump_must_raise_event)
{
    SCHEMA_AND_LISTENER
    TestPumpWindow window(&schema);
    PumpParams_Front p;
    window.pump = &p;

    window.createPump();

    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(&p, EVENT(PumpCreated), EVENT(Changed))
}

//------------------------------------------------------------------------------

TEST_GROUP("PumpWindow",
    ADD_TEST(createPump_first_pump_must_be_active),
    ADD_TEST(createPump_must_raise_event),
)

} // namespace PumpWindowTests
} // namespace Tests
} // namespace Z
