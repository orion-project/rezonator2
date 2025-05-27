#ifndef TEST_SCHEMA_LISTENER_H
#define TEST_SCHEMA_LISTENER_H

#include "../core/Schema.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {

class TestSchemaListener : public SchemaListener
{
public:
    void schemaChanged(Schema*s) { store(SchemaEvents::Changed, s); }

    void schemaCreated(Schema*s) { store(SchemaEvents::Created, s); }
    void schemaDeleted(Schema*s) { store(SchemaEvents::Deleted, s); }
    void schemaSaved(Schema*s) { store(SchemaEvents::Saved, s); }
    void schemaLoading(Schema*s) { store(SchemaEvents::Loading, s); }
    void schemaLoaded(Schema*s) { store(SchemaEvents::Loaded, s); }
    void schemaRebuilt(Schema*s) { store(SchemaEvents::Rebuilt, s); }

    void elementCreated(Schema*s, Element*e) { store(SchemaEvents::ElemCreated, s, e); }
    void elementChanged(Schema*s, Element*e) { store(SchemaEvents::ElemChanged, s, e); }
    void elementDeleting(Schema*s, Element*e) { store(SchemaEvents::ElemDeleting, s, e); }
    void elementDeleted(Schema*s, Element*e) { store(SchemaEvents::ElemDeleted, s, e); }

    void elementsDeleting(Schema*s) { store(SchemaEvents::ElemsDeleting, s); }
    void elementsDeleted(Schema*s) { store(SchemaEvents::ElemsDeleted, s); }

    void schemaParamsChanged(Schema*s) { store(SchemaEvents::ParamsChanged, s); }
    void schemaLambdaChanged(Schema*s) { store(SchemaEvents::LambdaChanged, s); }

    void customParamCreated(Schema*s, Z::Parameter*p) { store(SchemaEvents::CustomParamCreated, s, p); }
    void customParamEdited(Schema*s, Z::Parameter*p) { store(SchemaEvents::CustomParamEdited, s, p); }
    void customParamChanged(Schema*s, Z::Parameter*p) { store(SchemaEvents::CustomParamChanged, s, p); }
    void customParamDeleting(Schema*s, Z::Parameter*p) { store(SchemaEvents::CustomParamDeleting, s, p); }
    void customParamDeleted(Schema*s, Z::Parameter*p) { store(SchemaEvents::CustomParamDeleted, s, p); }

    void pumpCreated(Schema*s, PumpParams*p) { store(SchemaEvents::PumpCreated, s, p); }
    void pumpChanged(Schema*s, PumpParams*p) { store(SchemaEvents::PumpChanged, s, p); }
    void pumpCustomized(Schema*s, PumpParams*p) { store(SchemaEvents::PumpCustomized, s, p); }
    void pumpDeleting(Schema*s, PumpParams*p) { store(SchemaEvents::PumpDeleting, s, p); }
    void pumpDeleted(Schema*s, PumpParams*p) { store(SchemaEvents::PumpDeleted, s, p); }

    void recalcRequired(Schema*s) { store(SchemaEvents::RecalRequred, s); }

public:
    Schema *schema = nullptr;
    union {
        void *rawEventParam = nullptr;
        Element *element;
        PumpParams *pump;
    };
    QVector<SchemaEvents::Event> events;
    QVector<void*> eventParams;

    void reset()
    {
        rawEventParam = nullptr;
        events.clear();
    }

    bool checkEvents(QVector<SchemaEvents::Event> expected)
    {
        if (events.size() != expected.size()) return false;
        for (int i = 0; i < events.size(); i++)
            if (events.at(i) != expected.at(i)) return false;
        return true;
    }

    bool checkEventParams(QVector<void*> expected)
    {
        if (eventParams.size() != expected.size()) return false;
        for (int i = 0; i < eventParams.size(); i++)
            if (eventParams.at(i) != expected.at(i)) return false;
        return true;
    }

    QString eventsStr() const
    {
        QStringList s;
        for (auto event : events)
            s << "    " % SchemaEvents::str(event);
        return "Captured events:\n" % (s.isEmpty()? "    (none)": s.join("\n")) % "\n";
    }

private:
    void store(SchemaEvents::Event event, Schema *s, void *p = nullptr)
    {
        schema = s;
        if (p)
            rawEventParam = p;
        events << event;
        eventParams << p;
    }
};

} // namespace Tests
} // namespace Z


// Listener should be created before schema, to be destroyed in last turn.
// ~Schema() accesses listener, so schema must be destroyed first.
#define SCHEMA_AND_LISTENER \
    TestSchemaListener listener; \
    Schema schema; \
    schema.registerListener(&listener);

#define EVENT(event) SchemaEvents::event
#define STATE(state) SchemaState::state

#define SCHEMA_RESET_STATE\
    schema.events().raise(SchemaEvents::Saved, "");

#define ASSERT_LISTENER_EVENTS(...)\
    TEST_LOG(listener.eventsStr())\
    ASSERT_IS_TRUE(listener.checkEvents({__VA_ARGS__}))

#define ASSERT_LISTENER_EVENT_PARAMS(...)\
    ASSERT_IS_TRUE(listener.checkEventParams({__VA_ARGS__}))

#define ASSERT_LISTENER_NO_EVENTS \
    TEST_LOG(listener.eventsStr())\
    ASSERT_IS_TRUE(listener.events.isEmpty())

#define ASSERT_SCHEMA_STATE(expected_state)\
    ASSERT_EQ_INT(int(schema.state().current()), int(expected_state))

#define ASSERT_SCHEMA_PTR_STATE(expected_state)\
    ASSERT_EQ_INT(int(schema->state().current()), int(expected_state))

#endif // TEST_SCHEMA_LISTENER_H
