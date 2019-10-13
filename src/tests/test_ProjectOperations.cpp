#include "TestSchemaListener.h"

#include "../AppSettings.h"
#include "../ProjectOperations.h"

namespace Z {
namespace Tests {
namespace ProjectOperationsTests {

namespace {
class TestProjectOperations : public ProjectOperations
{
public:
    TestProjectOperations(Schema* s) : ProjectOperations(s, nullptr, nullptr) {}
    PumpParams* editedPump = nullptr;
    bool setupPumpDlgResult = true;
    bool selectTripTypeDlgResult = true;
    TripType selectedTripType = TripType::SW;
protected:
    bool editPumpDlg(PumpParams* pump) override {
        editedPump = pump;
        return setupPumpDlgResult;
    }
    bool selectTripTypeDlg(TripType* tripType) override {
        *tripType = selectedTripType;
        return selectTripTypeDlgResult;
    }
};
}

#define SCHEMA_PTR_AND_LISTENER(tripType)\
    TestSchemaListener listener;\
    QSharedPointer<Schema> schema(ProjectOperations::createDefaultSchema(tripType));\
    schema->registerListener(&listener);\
    TestProjectOperations op(schema.data());

//------------------------------------------------------------------------------

TEST_METHOD(createDefaultPump__must_add_active_pump)
{
    Schema schema;

    ASSERT_EQ_INT(schema.pumps()->size(), 0);

    ProjectOperations::createDefaultPump(&schema);

    ASSERT_EQ_INT(schema.pumps()->size(), 1);
    ASSERT_IS_TRUE(schema.pumps()->first()->isActive());
}

TEST_METHOD(createDefaultPump__must_raise_events)
{
    SCHEMA_AND_LISTENER;

    ASSERT_SCHEMA_STATE(STATE(New));

    ProjectOperations::createDefaultPump(&schema);

    ASSERT_SCHEMA_STATE(STATE(Modified));
    ASSERT_LISTENER_EVENTS(EVENT(PumpCreated), EVENT(Changed));
}

TEST_METHOD(createDefaultPump__must_generate_label_when_required)
{
    Schema schema;

    Settings::instance().pumpAutoLabel = true;
    ProjectOperations::createDefaultPump(&schema);

    ASSERT_IS_FALSE(schema.pumps()->first()->label().isEmpty());
}

TEST_METHOD(createDefaultPump__can_skip_label_generation)
{
    Schema schema;

    Settings::instance().pumpAutoLabel = false;
    ProjectOperations::createDefaultPump(&schema);

    ASSERT_IS_TRUE(schema.pumps()->first()->label().isEmpty());
}

//------------------------------------------------------------------------------

TEST_CASE_METHOD(createDefaultSchema__must_assign_trip_type, TripType tripType)
{
    QSharedPointer<Schema> s(ProjectOperations::createDefaultSchema(tripType));
    ASSERT_EQ_INT(s->tripType(), tripType);
    ASSERT_EQ_INT(s->state().current(), SchemaState::New);
}
TEST_CASE(createDefaultSchema__must_assign_trip_type_SW, createDefaultSchema__must_assign_trip_type, TripType::SW);
TEST_CASE(createDefaultSchema__must_assign_trip_type_RR, createDefaultSchema__must_assign_trip_type, TripType::RR);
TEST_CASE(createDefaultSchema__must_assign_trip_type_SP, createDefaultSchema__must_assign_trip_type, TripType::SP);

TEST_METHOD(createDefaultSchema__must_create_pump_for_SP_schema)
{
    QSharedPointer<Schema> s(ProjectOperations::createDefaultSchema(TripType::SP));
    ASSERT_EQ_INT(s->pumps()->size(), 1);
}

//------------------------------------------------------------------------------

TEST_CASE_METHOD(setupPump__must_do_nothing_for_non_SP_schema, TripType tripType)
{
    SCHEMA_PTR_AND_LISTENER(tripType);

    auto pump = PumpMode_Waist::instance()->makePump();
    pump->activate(true);
    schema->pumps()->append(pump);

    ASSERT_IS_NOT_NULL(schema->activePump());
    op.setupPump();

    ASSERT_SCHEMA_PTR_STATE(STATE(New));
    ASSERT_LISTENER_NO_EVENTS;
    ASSERT_IS_NULL(op.editedPump);
}
TEST_CASE(setupPump__must_do_nothing_for_SW_schema, setupPump__must_do_nothing_for_non_SP_schema, TripType::SW);
TEST_CASE(setupPump__must_do_nothing_for_RR_schema, setupPump__must_do_nothing_for_non_SP_schema, TripType::RR);

TEST_METHOD(setupPump__must_do_nothing_when_no_active_pump)
{
    SCHEMA_PTR_AND_LISTENER(TripType::SP);
    schema->pumps()->first()->activate(false);

    ASSERT_IS_NULL(schema->activePump());
    op.setupPump();

    ASSERT_SCHEMA_PTR_STATE(STATE(New));
    ASSERT_LISTENER_NO_EVENTS;
    ASSERT_IS_NULL(op.editedPump);
}

TEST_METHOD(setupPump__must_do_nothing_when_dialog_canceled)
{
    SCHEMA_PTR_AND_LISTENER(TripType::SP);

    op.setupPumpDlgResult = false;
    op.setupPump();

    ASSERT_SCHEMA_PTR_STATE(STATE(New));
    ASSERT_LISTENER_NO_EVENTS;
    ASSERT_EQ_PTR(op.editedPump, schema->activePump());
}

TEST_METHOD(setupPump__must_raise_events)
{
    SCHEMA_PTR_AND_LISTENER(TripType::SP);

    op.setupPump();

    ASSERT_SCHEMA_PTR_STATE(STATE(Modified));
    ASSERT_LISTENER_EVENTS(EVENT(PumpChanged), EVENT(Changed), EVENT(RecalRequred));
    ASSERT_EQ_PTR(op.editedPump, schema->activePump());
}

TEST_METHOD(setupPump__must_create_pump_when_no_pumps)
{
    SCHEMA_PTR_AND_LISTENER(TripType::SP);
    delete schema->pumps()->at(0);
    schema->pumps()->removeAt(0);

    ASSERT_EQ_INT(schema->pumps()->size(), 0);
    Settings::instance().pumpAutoLabel = true;
    op.setupPump();

    ASSERT_SCHEMA_PTR_STATE(STATE(Modified));
    ASSERT_LISTENER_EVENTS(EVENT(PumpCreated), EVENT(Changed),
                           EVENT(PumpChanged), EVENT(Changed),
                           EVENT(RecalRequred));
    ASSERT_EQ_INT(schema->pumps()->size(), 1);
    ASSERT_EQ_PTR(op.editedPump, schema->activePump());
    ASSERT_IS_FALSE(schema->activePump()->label().isEmpty());
}

//------------------------------------------------------------------------------

TEST_METHOD(setupTripType__must_do_nothing_when_dialog_canceled)
{
    SCHEMA_PTR_AND_LISTENER(TripType::SW);

    op.selectedTripType = TripType::RR;
    op.selectTripTypeDlgResult = false;
    op.setupTripType();

    ASSERT_SCHEMA_PTR_STATE(STATE(New));
    ASSERT_LISTENER_NO_EVENTS;
}

TEST_METHOD(setupTripType__must_do_nothing_when_trip_type_the_same)
{
    SCHEMA_PTR_AND_LISTENER(TripType::SW);

    op.selectedTripType = TripType::SW;
    op.selectTripTypeDlgResult = true;
    op.setupTripType();

    ASSERT_SCHEMA_PTR_STATE(STATE(New));
    ASSERT_LISTENER_NO_EVENTS;
}

TEST_METHOD(setupTripType__must_raise_events)
{
    SCHEMA_PTR_AND_LISTENER(TripType::SW);

    op.selectedTripType = TripType::RR;
    op.setupTripType();

    ASSERT_SCHEMA_PTR_STATE(STATE(Modified));
    ASSERT_LISTENER_EVENTS(EVENT(ParamsChanged), EVENT(Changed), EVENT(RecalRequred));
}

TEST_METHOD(setupTripType__must_create_active_pump_for_SP_schema)
{
    SCHEMA_PTR_AND_LISTENER(TripType::SW);

    ASSERT_EQ_INT(schema->pumps()->size(), 0);
    Settings::instance().pumpAutoLabel = true;
    op.selectedTripType = TripType::SP;
    op.setupTripType();

    ASSERT_EQ_INT(schema->pumps()->size(), 1);
    ASSERT_IS_NOT_NULL(schema->activePump());
    ASSERT_IS_FALSE(schema->activePump()->label().isEmpty());
    ASSERT_SCHEMA_PTR_STATE(STATE(Modified));
    ASSERT_LISTENER_EVENTS(EVENT(PumpCreated), EVENT(Changed),
                           EVENT(ParamsChanged), EVENT(Changed),
                           EVENT(RecalRequred));
}

//------------------------------------------------------------------------------

TEST_GROUP("Project Operations",
    ADD_TEST(createDefaultPump__must_add_active_pump),
    ADD_TEST(createDefaultPump__must_raise_events),
    ADD_TEST(createDefaultPump__must_generate_label_when_required),
    ADD_TEST(createDefaultPump__can_skip_label_generation),
    ADD_TEST(createDefaultSchema__must_assign_trip_type_SW),
    ADD_TEST(createDefaultSchema__must_assign_trip_type_RR),
    ADD_TEST(createDefaultSchema__must_assign_trip_type_SP),
    ADD_TEST(createDefaultSchema__must_create_pump_for_SP_schema),
    ADD_TEST(setupPump__must_do_nothing_for_SW_schema),
    ADD_TEST(setupPump__must_do_nothing_for_RR_schema),
    ADD_TEST(setupPump__must_do_nothing_when_no_active_pump),
    ADD_TEST(setupPump__must_do_nothing_when_dialog_canceled),
    ADD_TEST(setupPump__must_raise_events),
    ADD_TEST(setupPump__must_create_pump_when_no_pumps),
    ADD_TEST(setupTripType__must_do_nothing_when_dialog_canceled),
    ADD_TEST(setupTripType__must_do_nothing_when_trip_type_the_same),
    ADD_TEST(setupTripType__must_raise_events),
    ADD_TEST(setupTripType__must_create_active_pump_for_SP_schema),
)

} // namespace ProjectOperationsTests
} // namespace Tests
} // namespace Z
