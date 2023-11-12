#include "../tests/TestSchemaListener.h"
#include "../windows/PumpWindow.h"

#include "helpers/OriDialogs.h"

#include <QMessageBox>

#pragma GCC diagnostic ignored "-Wpadded"

namespace Z {
namespace Tests {
namespace PumpWindowTests {

#define ASSERT_LISTENER(expected_pump, ...) { \
    ASSERT_IS_TRUE(listener.schema == &schema) \
    ASSERT_IS_TRUE(listener.pump == expected_pump) \
    TEST_LOG(listener.eventsStr())\
    ASSERT_IS_TRUE(listener.checkEvents({__VA_ARGS__})) \
}

TEST_METHOD(activate_ori_dlg_mock) { Q_UNUSED(test); Ori::Dlg::Mock::setActive(true); }
TEST_METHOD(deactivate_ori_dlg_mock) { Q_UNUSED(test); Ori::Dlg::Mock::setActive(false); }
TEST_METHOD(reset_ori_dlg_resut) { Q_UNUSED(test); Ori::Dlg::Mock::resetLastDialog(); }

//------------------------------------------------------------------------------

namespace {
class TestPumpWindow : public PumpWindow
{
public:
    TestPumpWindow(Schema* schema): PumpWindow(schema) {}

    PumpParams *pump;
    bool dlgResult = true;

protected:
    PumpParams* makeNewPumpDlg() override { return pump; }
    bool editPumpDlg(PumpParams*) override { return dlgResult; }
    PumpParams* selectedPump() const override { return pump; }
};
}

//------------------------------------------------------------------------------

TEST_METHOD(createPump__first_pump_must_be_active)
{
    Schema schema;
    TestPumpWindow window(&schema);
    window.pump = PumpMode_Waist::instance()->makePump();

    ASSERT_IS_NULL(schema.activePump())
    window.createPump();
    ASSERT_EQ_PTR(schema.activePump(), window.pump);
}

TEST_METHOD(createPump__must_raise_event)
{
    SCHEMA_AND_LISTENER
    TestPumpWindow window(&schema);
    window.pump = PumpMode_Waist::instance()->makePump();

    window.createPump();

    ASSERT_SCHEMA_STATE(STATE(Modified))
    ASSERT_LISTENER(window.pump, EVENT(PumpCreated), EVENT(Changed), EVENT(RecalRequred))
}

//------------------------------------------------------------------------------

TEST_METHOD(editPump__must_do_nothing_when_dialog_canceled)
{
    SCHEMA_AND_LISTENER;

    TestPumpWindow window(&schema);
    window.pump = PumpMode_Waist::instance()->makePump();
    window.dlgResult = false;
    window.editPump();

    ASSERT_SCHEMA_STATE(STATE(New));
    ASSERT_LISTENER_NO_EVENTS;
}

TEST_METHOD(editPump__must_raise_events)
{
    SCHEMA_AND_LISTENER;

    TestPumpWindow window(&schema);
    window.pump = PumpMode_Waist::instance()->makePump();
    window.editPump();

    ASSERT_SCHEMA_STATE(STATE(Modified));
    ASSERT_LISTENER(window.pump, EVENT(PumpChanged), EVENT(Changed));
}

TEST_METHOD(editPump__must_recalc_SP_schema_when_pump_is_active)
{
    SCHEMA_AND_LISTENER;

    schema.events().disable();
    schema.setTripType(TripType::SP);
    schema.events().enable();

    TestPumpWindow window(&schema);
    window.pump = PumpMode_Waist::instance()->makePump();
    window.pump->activate(true);
    window.editPump();

    ASSERT_SCHEMA_STATE(STATE(Modified));
    ASSERT_LISTENER(window.pump, EVENT(PumpChanged), EVENT(Changed), EVENT(RecalRequred));
}

TEST_METHOD(editPump__must_not_recalc_non_SP_schema)
{
    SCHEMA_AND_LISTENER;

    TestPumpWindow window(&schema);
    window.pump = PumpMode_Waist::instance()->makePump();
    window.pump->activate(true);
    window.editPump();

    ASSERT_LISTENER(window.pump, EVENT(PumpChanged), EVENT(Changed));
}

//------------------------------------------------------------------------------

TEST_METHOD(deletePump__must_not_delete_the_only_pump_from_SP_schema)
{
    auto pump = PumpMode_Waist::instance()->makePump();

    SCHEMA_AND_LISTENER;
    schema.events().disable();
    schema.setTripType(TripType::SP);
    schema.pumps()->append(pump);
    schema.events().enable();

    TestPumpWindow window(&schema);
    window.pump = pump;
    window.deletePump();

    ASSERT_EQ_INT(Ori::Dlg::Mock::getLastDialog(), Ori::Dlg::Mock::DialogKind::info);
    ASSERT_EQ_INT(schema.pumps()->size(), 1);
    ASSERT_SCHEMA_STATE(STATE(New));
    ASSERT_LISTENER_NO_EVENTS;
}

TEST_METHOD(deletePump__can_delete_the_only_pump_from_non_SP_schema)
{
    Ori::Dlg::Mock::setNextResult(QMessageBox::Ok);

    class TestPumpParams : public PumpParams_Waist
    {
    public:
        ~TestPumpParams() { SET_TEST_DATA("pump was deleted", true); }
        Ori::Testing::TestBase* test = nullptr;
    };

    auto pump = new TestPumpParams;
    pump->test = test;

    SCHEMA_AND_LISTENER;
    schema.events().disable();
    schema.setTripType(TripType::SW);
    schema.pumps()->append(pump);
    schema.events().enable();

    TestPumpWindow window(&schema);
    window.pump = pump;
    window.deletePump();

    ASSERT_EQ_INT(Ori::Dlg::Mock::getLastDialog(), Ori::Dlg::Mock::DialogKind::ok);
    ASSERT_EQ_INT(schema.pumps()->size(), 0);
    ASSERT_SCHEMA_STATE(STATE(Modified))
    // There should not be RecalRequred event when pump was not active
    ASSERT_LISTENER(pump, EVENT(PumpDeleting), EVENT(PumpDeleted), EVENT(Changed))
    ASSERT_EQ_DATA("pump was deleted", true)
}

TEST_METHOD(deletePump__must_do_nothing_when_not_confirmed)
{
    Ori::Dlg::Mock::setNextResult(QMessageBox::Cancel);

    auto pump = PumpMode_Waist::instance()->makePump();

    SCHEMA_AND_LISTENER;
    schema.events().disable();
    schema.setTripType(TripType::SW);
    schema.pumps()->append(pump);
    schema.events().enable();

    TestPumpWindow window(&schema);
    window.pump = pump;
    window.deletePump();

    ASSERT_EQ_INT(Ori::Dlg::Mock::getLastDialog(), Ori::Dlg::Mock::DialogKind::ok);
    ASSERT_EQ_INT(schema.pumps()->size(), 1);
    ASSERT_SCHEMA_STATE(STATE(New));
    ASSERT_LISTENER_NO_EVENTS;
}

TEST_METHOD(deletePump__must_activate_the_first_pump_when_active_pump_was_deleted)
{
    Ori::Dlg::Mock::setNextResult(QMessageBox::Ok);

    auto pump1 = PumpMode_Waist::instance()->makePump(); TEST_LOG_PTR(pump1);
    auto pump2 = PumpMode_Waist::instance()->makePump(); TEST_LOG_PTR(pump2);
    auto pump3 = PumpMode_Waist::instance()->makePump(); TEST_LOG_PTR(pump3);
    pump3->activate(true);

    SCHEMA_AND_LISTENER;
    schema.events().disable();
    schema.setTripType(TripType::SP);
    schema.pumps()->append(pump1);
    schema.pumps()->append(pump2);
    schema.pumps()->append(pump3);
    schema.events().enable();

    ASSERT_EQ_PTR(schema.activePump(), pump3);

    TestPumpWindow window(&schema);
    window.pump = pump3;
    window.deletePump();

    ASSERT_EQ_INT(schema.pumps()->size(), 2);
    ASSERT_SCHEMA_STATE(STATE(Modified));
    ASSERT_LISTENER(pump1, // pump1 is about PumpChanged event,
                    // in listener, it overrides pump3 of PumpDeleting event
                    EVENT(PumpDeleting), EVENT(PumpDeleted), EVENT(Changed),
                    EVENT(PumpChanged), EVENT(Changed), EVENT(RecalRequred));
    ASSERT_EQ_PTR(schema.activePump(), pump1);
    ASSERT_IS_TRUE(pump1->isActive());
}

//------------------------------------------------------------------------------

TEST_METHOD(activatePump__must_do_nothing_when_pump_is_already_active)
{
    auto pump1 = PumpMode_Waist::instance()->makePump(); TEST_LOG_PTR(pump1);
    auto pump2 = PumpMode_Waist::instance()->makePump(); TEST_LOG_PTR(pump2);
    pump2->activate(true);

    SCHEMA_AND_LISTENER;
    schema.events().disable();
    schema.setTripType(TripType::SP);
    schema.pumps()->append(pump1);
    schema.pumps()->append(pump2);
    schema.events().enable();

    TestPumpWindow window(&schema);
    window.pump = pump2;
    window.activatePump();

    ASSERT_SCHEMA_STATE(STATE(New));
    ASSERT_LISTENER_NO_EVENTS;
    ASSERT_EQ_PTR(schema.activePump(), pump2);
}

TEST_METHOD(activatePump__must_deactivate_previous_active_pump)
{
    auto pump1 = PumpMode_Waist::instance()->makePump(); TEST_LOG_PTR(pump1);
    auto pump2 = PumpMode_Waist::instance()->makePump(); TEST_LOG_PTR(pump2);
    pump2->activate(true);

    SCHEMA_AND_LISTENER;
    schema.events().disable();
    schema.setTripType(TripType::SP);
    schema.pumps()->append(pump1);
    schema.pumps()->append(pump2);
    schema.events().enable();

    TestPumpWindow window(&schema);
    window.pump = pump1;
    window.activatePump();

    ASSERT_SCHEMA_STATE(STATE(Modified));
    ASSERT_LISTENER(pump1,
                    EVENT(PumpChanged), EVENT(Changed), // <- pump2 deactivated
                    EVENT(PumpChanged), EVENT(Changed), // <- pump1 activated
                    EVENT(RecalRequred))
    ASSERT_EQ_PTR(schema.activePump(), pump1);
    ASSERT_IS_FALSE(pump2->isActive());
}

TEST_METHOD(activatePump__must_not_recalc_non_SP_schema)
{
    auto pump1 = PumpMode_Waist::instance()->makePump(); TEST_LOG_PTR(pump1);
    auto pump2 = PumpMode_Waist::instance()->makePump(); TEST_LOG_PTR(pump2);
    pump2->activate(true);

    SCHEMA_AND_LISTENER;
    schema.events().disable();
    schema.setTripType(TripType::SW);
    schema.pumps()->append(pump1);
    schema.pumps()->append(pump2);
    schema.events().enable();

    TestPumpWindow window(&schema);
    window.pump = pump1;
    window.activatePump();

    ASSERT_SCHEMA_STATE(STATE(Modified));
    ASSERT_LISTENER(pump1,
                    EVENT(PumpChanged), EVENT(Changed), // <- pump2 deactivated
                    EVENT(PumpChanged), EVENT(Changed)) // <- pump1 activated
    ASSERT_EQ_PTR(schema.activePump(), pump1);
    ASSERT_IS_FALSE(pump2->isActive());
}

//------------------------------------------------------------------------------

TEST_GROUP("PumpWindow",
    BEFORE_ALL(activate_ori_dlg_mock),
    BEFORE_EACH(reset_ori_dlg_resut),
    ADD_TEST(createPump__first_pump_must_be_active),
    ADD_TEST(createPump__must_raise_event),
    ADD_TEST(editPump__must_do_nothing_when_dialog_canceled),
    ADD_TEST(editPump__must_raise_events),
    ADD_TEST(editPump__must_recalc_SP_schema_when_pump_is_active),
    ADD_TEST(editPump__must_not_recalc_non_SP_schema),
    ADD_TEST(deletePump__must_not_delete_the_only_pump_from_SP_schema),
    ADD_TEST(deletePump__can_delete_the_only_pump_from_non_SP_schema),
    ADD_TEST(deletePump__must_do_nothing_when_not_confirmed),
    ADD_TEST(deletePump__must_activate_the_first_pump_when_active_pump_was_deleted),
    ADD_TEST(activatePump__must_do_nothing_when_pump_is_already_active),
    ADD_TEST(activatePump__must_deactivate_previous_active_pump),
    ADD_TEST(activatePump__must_not_recalc_non_SP_schema),
    AFTER_ALL(deactivate_ori_dlg_mock),
)

} // namespace PumpWindowTests
} // namespace Tests
} // namespace Z
