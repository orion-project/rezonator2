#ifndef SCHEMA_H
#define SCHEMA_H

#include <QApplication>
#include <QDebug>
#include <QMap>

#include "Element.h"
#include "Parameters.h"
#include "Pump.h"
#include "SchemaClient.h"

class Schema;

//------------------------------------------------------------------------------
/**
    Schema listener interface.
    Any object who wants "listen" any schema changes or other events should
    implements this interface and registers himself as listener in a schema
    through the method Schema::registerListener(). Schema will call appropriate
    method of interface when something occures.
*/
class SchemaListener : public SchemaClient
{
public:
    virtual void schemaCreated(Schema*) {}
    virtual void schemaDeleted(Schema*) {}
    virtual void schemaChanged(Schema*) {}
    virtual void schemaSaved(Schema*) {}
    virtual void schemaLoading(Schema*) {}
    virtual void schemaLoaded(Schema*) {}
    virtual void elementCreated(Schema*, Element*) {}
    virtual void elementChanged(Schema*, Element*) {}
    virtual void elementDeleting(Schema*, Element*) {}
    virtual void elementDeleted(Schema*, Element*) {}
    virtual void schemaParamsChanged(Schema*) {}
    virtual void schemaLambdaChanged(Schema*) {}
};

//------------------------------------------------------------------------------

class SchemaState
{
public:
    enum State { None, New, Loading, Modified };
    static const int Current = -1;

    State current() const { return _current; }

    bool isNew() const { return _current == New; }
    bool isLoading() const { return _current == Loading; }
    bool isModified() const { return _current == Modified; }

    QString str() const;

private:
    State _current;

    // Schema can change state only due to events
    void set(State state);
    friend class SchemaEvents;
};

//------------------------------------------------------------------------------

class SchemaEvents
{
public:
    enum Event
    {
        Created, Deleted, Changed, Saved, Loading, Loaded,
        ElemCreated, ElemChanged, ElemDeleting, ElemDeleted,
        ParamsChanged, LambdaChanged
    };

    void raise(Event event, Element* element = nullptr) const;

    void enable() { _enabled = true; }
    void disable() { _enabled = false; }

    static QString str(Event event) { return propsOf(event).name; }

private:
    bool _enabled = true;

    Schema *_schema;
    friend class Schema;

    struct EventProps
    {
        QString name;
        bool shouldRaiseChanged; ///< Should also raise event 'changed'
        SchemaState::State nextState; ///< New state which schema obtains with this event
    };
    static const EventProps& propsOf(Event event);

    void notify(SchemaListener* listener, SchemaEvents::Event event, Element* element) const;
};

//------------------------------------------------------------------------------
/**
    Interface of element selector.
    A widged displaying schema elements can allow user to select one or several
    elements by mouse or keyboard (like SchemaTable does). This widged can
    implement this interface and register itself as SchemaCursor in a schema
    Then any others will be able to obtain selected element(s)
    without any redundant knowledge about who exactly made this selection.
*/
class ElementSelector
{
public:
    virtual Element* selected() const { return nullptr; }
    virtual Elements selection() const { return Elements(); }
};

/**
    Safe-guard for ElementSelector.
    Allows to register/unregister a selector and provides null-pointer check when taking selection.
*/
class SchemaSelection
{
public:
    void registerSelector(ElementSelector* selector) { _selector = selector; }
    void unregisterSelector(ElementSelector* selector) { if (_selector == selector) _selector = nullptr; }

    Element* element() const { return _selector ? _selector->selected() : nullptr; }
    Elements elements() const { return _selector ? _selector->selection() : Elements(); }

private:
    ElementSelector* _selector = nullptr;
};

//------------------------------------------------------------------------------

class Schema : public ElementOwner, public Z::ParameterOwner
{
public:
    enum TripType
    {
        SW, ///< Standing wave system
        RR, ///< Ring resonator
        SP  ///< Single pass system
    };

public:
    Schema();
    virtual ~Schema();

    const QString& fileName() const { return _fileName; }
    void setFileName(const QString& fileName) { _fileName = fileName; }

    Z::Parameter& wavelength() { return _wavelength; }

    TripType tripType() const { return _tripType; }
    void setTripType(TripType value) { _tripType = value; }
    inline bool isSW() const { return _tripType == SW; }
    inline bool isRR() const { return _tripType == RR; }
    inline bool isSP() const { return _tripType == SP; }
    inline bool isResonator() const { return _tripType == SW || _tripType == RR; }

    const Z::Pump::Params& pump() const { return _pump; }
    void setPump(const Z::Pump::Params& pump);

    int count() const { return _items.size(); }
    int enabledCount() const;
    const Elements& elements() const { return _items; }
    Element* element(int index) const;
    Element* elementById(int id) const;
    int indexOf(Element *elem) const { return _items.indexOf(elem); }
    bool isEmpty() const { return _items.size() == 0; }

    SchemaEvents& events() { return _events; }
    SchemaState& state() { return _state; }
    bool modified() const { return _state.current() == SchemaState::Modified; }

    SchemaClients& clients() { return _clients; }
    void registerListener(SchemaListener* listener) { _clients.append(listener); }
    void unregisterListener(SchemaListener* listener) { _clients.remove(listener); }

    void insertElement(Element* elem, int index = -1, bool event = true);
    void deleteElement(Element* elem, bool event = true);
    void deleteElement(int index, bool event = true);

    SchemaSelection& selection() { return _selection; }
    Element* selectedElement() const { return _selection.element(); }
    Elements selectedElements() const { return _selection.elements(); }

private:
    Elements _items;
    SchemaEvents _events;
    SchemaState _state;
    SchemaClients _clients;
    SchemaSelection _selection;
    QString _fileName;
    QString _title, _comment;
    TripType _tripType = SW;
    Z::Parameter _wavelength;
    Z::Pump::Params _pump;

    ///// inherits from ElementOwner
    void elementChanged(Element *elem) override { _events.raise(SchemaEvents::ElemChanged, elem); }

    ///// inherits from ParameterOwner
    void parameterChanged(Z::ParameterBase *param) override;

    inline bool isValid(int index) const { return index >= 0 && index < _items.size(); }
};


#endif // SCHEMA_H
