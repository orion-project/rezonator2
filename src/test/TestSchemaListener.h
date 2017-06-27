#ifndef TEST_SCHEMA_LISTENER_H
#define TEST_SCHEMA_LISTENER_H

#include "../core/Schema.h"
#include "testing/OriTestBase.h"

class TestSchemaListener : public SchemaListener
{
public:
    void schemaCreated(Schema*s) { store(SchemaEvents::Created, s); }
    void schemaDeleted(Schema*s) { store(SchemaEvents::Deleted, s); }
    void schemaChanged(Schema*s) { store(SchemaEvents::Changed, s); }
    void schemaSaved(Schema*s) { store(SchemaEvents::Saved, s); }
    void schemaLoading(Schema*s) { store(SchemaEvents::Loading, s); }
    void schemaLoaded(Schema*s) { store(SchemaEvents::Loaded, s); }
    void elementCreated(Schema*s, Element*e) { store(SchemaEvents::ElemCreated, s, e); }
    void elementDeleting(Schema*s, Element*e) { store(SchemaEvents::ElemDeleting, s, e); }
    void elementDeleted(Schema*s, Element*e) { store(SchemaEvents::ElemDeleted, s, e); }
    void elementChanged(Schema*s, Element*e) { store(SchemaEvents::ElemChanged, s, e); }
    void schemaParamsChanged(Schema*s) { store(SchemaEvents::ParamsChanged, s); }
    void schemaLambdaChanged(Schema*s) { store(SchemaEvents::LambdaChanged, s); }

public:
    Schema *schema = nullptr;
    Element *element = nullptr;
    QVector<SchemaEvents::Event> events;

    void reset()
    {
        schema = nullptr;
        element = nullptr;
        events.clear();
    }

    bool checkEvents(QVector<SchemaEvents::Event> expected)
    {
        if (events.size() != expected.size()) return false;
        for (int i = 0; i < events.size(); i++)
            if (events.at(i) != expected.at(i)) return false;
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
    void store(SchemaEvents::Event event, Schema *s, Element *e = nullptr)
    {
        schema = s;
        if (e)
            element = e;
        events << event;
    }
};

// Listener should be created before schema, to be destroyed in last turn.
// ~Schema() accesses listener, so schema must be destroyed first.
#define SCHEMA_AND_LISTENER \
    TestSchemaListener listener; \
    Schema schema; \
    schema.registerListener(&listener);

#define EVENT(event) SchemaEvents::event
#define STATE(state) SchemaState::state

#define SCHEMA_RESET_STATE\
    schema.events().raise(SchemaEvents::Saved);

#define ASSERT_LISTENER(expected_elem, ...) { \
    ASSERT_IS_TRUE(listener.schema == &schema) \
    /*TEST_LOG_PTR(listener.element)*/\
    /*TEST_LOG_PTR(expected_elem)*/\
    ASSERT_IS_TRUE(listener.element == expected_elem) \
    TEST_LOG(listener.eventsStr())\
    ASSERT_IS_TRUE(listener.checkEvents({__VA_ARGS__})) \
}

#define ASSERT_LISTENER_EVENT(expected_event) \
    TEST_LOG(listener.eventsStr())\
    ASSERT_IS_TRUE(listener.events.contains(expected_event))

#define ASSERT_LISTENER_NO_EVENTS \
    TEST_LOG(listener.eventsStr())\
    ASSERT_IS_TRUE(listener.events.isEmpty())

#endif // TEST_SCHEMA_LISTENER_H

