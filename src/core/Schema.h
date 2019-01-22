#ifndef SCHEMA_H
#define SCHEMA_H

#include <QApplication>
#include <QDebug>
#include <QMap>

#include "CommonTypes.h"
#include "Element.h"
#include "Formula.h"
#include "Parameters.h"
#include "Pump.h"
#include "SchemaClient.h"

class Schema;

//------------------------------------------------------------------------------
/**
    Schema listener interface.
    Any object who wants "listen" any schema changes or other events should
    implement this interface and registers himself as a listener in a schema
    through the method @a Schema::registerListener(). The schema will call
    appropriate method of the interface when something occurs.
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
    virtual void customParamCreated(Schema*, Z::Parameter*) {}
    virtual void customParamEdited(Schema*, Z::Parameter*) {}
    virtual void customParamChanged(Schema*, Z::Parameter*) {}
    virtual void customParamDeleting(Schema*, Z::Parameter*) {}
    virtual void customParamDeleted(Schema*, Z::Parameter*) {}
    virtual void pumpCreated(Schema*, Z::PumpParams*) {}
    virtual void pumpChanged(Schema*, Z::PumpParams*) {}
    virtual void pumpDeleting(Schema*, Z::PumpParams*) {}
    virtual void pumpDeleted(Schema*, Z::PumpParams*) {}
    virtual void recalcRequired(Schema*) {}
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

/**
    Schema event system.

    Steps to add a new event:
    1. Add a new member to @a SchemaEvents enum.
    2. Add new notification method to @a SchemaListener interface.
    3. Add calling of that method to @a SchemaEvents::notify() method.
    4. Define props of new event in @a SchemaEvents::propsOf() method.
*/
class SchemaEvents
{
public:
    /// Event types that can be sent to schema listeners.
    enum Event
    {
        Created,       ///< Schema just was created, called from the constructor
        Deleted,       ///< Schema was deleted, called from destructor
        Changed,       ///< The general event when something was changed,
                       ///< can be called after another changing event if it configured.
        Saved,         ///< Schema was saved
        Loading,       ///< Schema is turned into Loading state
        Loaded,        ///< Loading is completed

        ElemCreated,   ///< New element was added to schema
        ElemChanged,   ///< Element's params changed
        ElemDeleting,  ///< Element is being deleted from schema
        ElemDeleted,   ///< Element was deleted from schema

        ParamsChanged, ///< Some schema parameter was changed (e.g. TripType)
        LambdaChanged, ///< Schema wavelength was changed

        CustomParamCreated, ///< New custom parameter was created
        CustomParamEdited,  ///< Custom parameter was edited (e.g. description changed, but not value)
        CustomParamChanged, ///< Value of custom parameter was changed
        CustomParamDeleting,///< Custom param is about to be deleted
        CustomParamDeleted, ///< Custom parameter was deleted

        PumpCreated,   ///< New pump was created
        PumpChanged,   ///< Pump parameters were changed
        PumpDeleting,  ///< Pump is about to be deleted
        PumpDeleted,   ///< Pump was deleted

        RecalRequred,  ///< Something is done that requires recalculation of all functions
    };

    void raise(Event event, void* param = nullptr) const;

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

    void notify(SchemaListener* listener, SchemaEvents::Event event, void* param) const;
};

//------------------------------------------------------------------------------
/**
    The interface of element selector.
    A widget displaying schema elements can allow a user to select one or several
    elements by mouse or keyboard (like @a SchemaElemsTable does). This widget can implement this interface
    and register itself in a schema via `schema->selection().registerSelector(selecting_widget)`.
    Then any others will be able to obtain selected element(s) via `schema->selectedElements()`
    without any redundant knowledge about who exactly made this selection.
*/
class ElementSelector
{
public:
    virtual ~ElementSelector();
    virtual Element* selected() const { return nullptr; }
    virtual Elements selection() const { return Elements(); }
};

/**
    Safe-guard for ElementSelector.
    Allows to register/unregister a selector and provides null-pointer check when making a selection.
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

class Schema : public ElementOwner, public Z::ParameterListener
{
public:
    Schema();
    ~Schema() override;

    const QString& fileName() const { return _fileName; }
    void setFileName(const QString& fileName) { _fileName = fileName; }

    const QString& title() const { return _title; }
    void setTitle(const QString& title) { _title = title; }

    Z::Parameter& wavelength() { return _wavelength; }

    TripType tripType() const { return _tripType; }
    void setTripType(TripType value);
    inline bool isSW() const { return _tripType == TripType::SW; }
    inline bool isRR() const { return _tripType == TripType::RR; }
    inline bool isSP() const { return _tripType == TripType::SP; }
    inline bool isResonator() const { return _tripType == TripType::SW || _tripType == TripType::RR; }

    int count() const override { return _items.size(); }
    int enabledCount() const;
    const Elements& elements() const { return _items; }
    Element* element(int index) const;
    Element* elementById(int id) const;
    Element* elementByLabel(const QString& label) const;
    int indexOf(Element *elem) const override { return _items.indexOf(elem); }
    bool isEmpty() const { return _items.size() == 0; }

    SchemaEvents& events() { return _events; }
    SchemaState& state() { return _state; }
    bool modified() const { return _state.current() == SchemaState::Modified; }

    SchemaClients& clients() { return _clients; }
    void registerListener(SchemaListener* listener) { _clients.append(listener); }
    void unregisterListener(SchemaListener* listener) { _clients.remove(listener); }

    void insertElement(Element* elem, int index = -1, bool event = true);
    void deleteElement(Element* elem, bool event = true, bool free = true);
    void deleteElement(int index, bool event = true, bool free = true);

    SchemaSelection& selection() { return _selection; }
    Element* selectedElement() const { return _selection.element(); }
    Elements selectedElements() const { return _selection.elements(); }

    /// Adiitional params that can be added by user and used in formulas.
    Z::Parameters* customParams() { return &_customParams; }

    /// Linst of all links which bind elements' parameter to schema parameters.
    Z::ParamLinks* paramLinks() { return &_paramLinks; }

    /// List of all formulas operated on schema parameters.
    Z::Formulas* formulas() { return &_formulas; }

    /// Returns a list of global params that can be used as link sources or formula dependencies.
    /// It contains custom params and some of the built-in parameters.
    /// This list is not stored in a schema and collect parameters at each call.
    Z::Parameters globalParams() const;

    Z::PumpsList* pumps() { return &_pumps; }
    Z::PumpParams* activePump();

    /// Makes automatic label for given element.
    /// Automatical label consist of a prefix like 'M', 'L', etc. and index.
    void generateLabel(Element* elem);

private:
    Elements _items;
    SchemaEvents _events;
    SchemaState _state;
    SchemaClients _clients;
    SchemaSelection _selection;
    QString _fileName;
    QString _title, _comment;
    TripType _tripType = TripType::SW;
    Z::Parameter _wavelength;
    Z::Parameters _customParams;
    Z::ParamLinks _paramLinks;
    Z::Formulas _formulas;
    Z::PumpsList _pumps;

    // inherits from ElementOwner
    void elementChanged(Element *elem) override { _events.raise(SchemaEvents::ElemChanged, elem); }

    // inherits from ParameterListener
    void parameterChanged(Z::ParameterBase *param) override;

    inline bool isValid(int index) const { return index >= 0 && index < _items.size(); }

    /// Remove links driving this elements' params
    void removeParamLinks(Element* elem);

    void relinkInterfaces();
};

#endif // SCHEMA_H
