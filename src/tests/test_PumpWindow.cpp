#include "TestSchemaListener.h"
#include "../PumpWindow.h"

#include "helpers/OriDialogs.h"

namespace Z {
namespace Tests {
namespace PumpWindowTests {

#define ASSERT_LISTENER(expected_pump, ...) { \
    ASSERT_IS_TRUE(listener.schema == &schema) \
    ASSERT_IS_TRUE(listener.pump == expected_pump) \
    TEST_LOG(listener.eventsStr())\
    ASSERT_IS_TRUE(listener.checkEvents({__VA_ARGS__})) \
}

TEST_METHOD(activate_ori_dlg_mock) { Ori::Dlg::Mock::setActive(true); }
TEST_METHOD(deactivate_ori_dlg_mock) { Ori::Dlg::Mock::setActive(false); }

//------------------------------------------------------------------------------

namespace {
class TestPumpWindow : public PumpWindow
{
public:
    TestPumpWindow(Schema* schema): PumpWindow(schema) {}

    PumpParams *pump;

protected:
    PumpParams* makeNewPumpDlg() override { return pump; }
    Z::PumpParams* selectedPump() const override { return pump; }
};
}

//------------------------------------------------------------------------------

TEST_METHOD(createPump__first_pump_must_be_active)
{
    Schema schema;
    TestPumpWindow window(&schema);
    PumpParams_Front p;
    window.pump = &p;

    ASSERT_IS_NULL(schema.activePump())
    window.createPump();
    ASSERT_EQ_PTR(schema.activePump(), &p);
}

TEST_METHOD(createPump__must_raise_event)
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

TEST_METHOD(deletePump__must_not_delete_the_last_pump_from_SP_schema)
{
}

TEST_METHOD(deletePump__can_delete_the_last_pump_from_non_SP_schema)
{

}

//------------------------------------------------------------------------------

TEST_GROUP("PumpWindow",
    BEFORE_ALL(activate_ori_dlg_mock),
    ADD_TEST(createPump__first_pump_must_be_active),
    ADD_TEST(createPump__must_raise_event),
    ADD_TEST(deletePump__must_not_delete_the_last_pump_from_SP_schema),
    ADD_TEST(deletePump__can_delete_the_last_pump_from_non_SP_schema),
    AFTER_ALL(deactivate_ori_dlg_mock),
)

} // namespace PumpWindowTests
} // namespace Tests
} // namespace Z
