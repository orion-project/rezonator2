#include "Format.h"
#include "Protocol.h"
#include "Schema.h"

//------------------------------------------------------------------------------
//                                SchemaState
//------------------------------------------------------------------------------

QString SchemaState::str() const
{
    switch (_current)
    {
    case None: return "SchemaState: None";
    case New: return "SchemaState: New";
    case Loading: return "SchemaState: Loading";
    case Modified: return "SchemaState: Modified";
    }
    return QString();
}

void SchemaState::set(State state)
{
    _current = state;

    Z_REPORT(str());
}

//------------------------------------------------------------------------------
//                                SchemaEvents
//------------------------------------------------------------------------------

#define INIT_EVENT(event, raise_changed, next_state)\
    {event, EventProps{QString(# event), raise_changed, (SchemaState::State)next_state}}

void SchemaEvents::raise(Event event, Element* element) const
{
    if (!_enabled) return;

    const EventProps& eventProps = propsOf(event);
    Z_REPORT("SchemaEvent:" << eventProps.name)

    if (int(eventProps.nextState) != SchemaState::Current)
        _schema->state().set(eventProps.nextState);

    auto listeners = _schema->clients().get<SchemaListener>();

    for (SchemaListener* listener : listeners)
        notify(listener, event, element);

    if (eventProps.shouldRaiseChanged)
    {
        Z_REPORT("SchemaEvent:" << propsOf(Changed).name)
        for (SchemaListener* listener : listeners)
            notify(listener, Changed, nullptr);
    }
}

const SchemaEvents::EventProps& SchemaEvents::propsOf(Event event)
{
    static QMap<Event, EventProps> _props(
    {
        //                      | Should also | New state which
        //                      | raise event | schema obtains
        //                      | 'Changed'   | with this event
        INIT_EVENT(Created,       true,         SchemaState::New      ),
        INIT_EVENT(Deleted,       false,        SchemaState::Current  ),
        INIT_EVENT(Changed,       false,        SchemaState::Modified ),
        INIT_EVENT(Saved,         true,         SchemaState::None     ),
        INIT_EVENT(Loading,       false,        SchemaState::Loading  ),
        INIT_EVENT(Loaded,        true,         SchemaState::None     ),
        INIT_EVENT(ElemCreated,   true,         SchemaState::Modified ),
        INIT_EVENT(ElemChanged,   true,         SchemaState::Modified ),
        INIT_EVENT(ElemDeleting,  false,        SchemaState::Current  ),
        INIT_EVENT(ElemDeleted,   true,         SchemaState::Modified ),
        INIT_EVENT(ParamsChanged, true,         SchemaState::Modified ),
        INIT_EVENT(LambdaChanged, true,         SchemaState::Modified )
    });
    return _props[event];
}

void SchemaEvents::notify(SchemaListener* listener, SchemaEvents::Event event, Element* element) const
{
    switch (event)
    {
    case Created: listener->schemaCreated(_schema); break;
    case Deleted: listener->schemaDeleted(_schema); break;
    case Changed: listener->schemaChanged(_schema); break;
    case Saved: listener->schemaSaved(_schema); break;
    case Loading: listener->schemaLoading(_schema); break;
    case Loaded: listener->schemaLoaded(_schema); break;
    case ElemCreated: listener->elementCreated(_schema, element); break;
    case ElemChanged: listener->elementChanged(_schema, element); break;
    case ElemDeleting: listener->elementDeleting(_schema, element); break;
    case ElemDeleted: listener->elementDeleted(_schema, element); break;
    case ParamsChanged: listener->schemaParamsChanged(_schema); break;
    case LambdaChanged: listener->schemaLambdaChanged(_schema); break;
    };
}

//------------------------------------------------------------------------------
//                                 Schema
//------------------------------------------------------------------------------

Schema::Schema()
{
    _wavelength = Z::Parameter(Z::Dims::linear(), "lambda", Z::Strs::lambda(), /*qApp->translate("Param", "Wavelength")*/QString());
    _wavelength.setValue(Z::Value(980, Z::Units::nm()));
    _wavelength.addListener(this);

    // TODO:NEXT-VER init default pump

    _events._schema = this;
    _events.raise(SchemaEvents::Created);
}

Schema::~Schema()
{
    _events.raise(SchemaEvents::Deleted);

    qDeleteAll(_items);
    qDeleteAll(_params);
}

// TODO:NEXT-VER make test
void Schema::setPump(const Z::Pump::Params& pump)
{
    _pump = pump;
    _events.raise(SchemaEvents::ParamsChanged);
}

int Schema::enabledCount() const
{
    int count = 0;
    for (int i= 0; i < _items.size(); i++)
        if (!_items.at(i)->disabled())
            count++;
    return count;
}

Element* Schema::element(int index) const
{
    return isValid(index)? _items.at(index): nullptr;
}

Element* Schema::elementById(int id) const
{
    foreach (Element *elem, _items)
        if (elem->id() == id)
            return elem;
    return nullptr;
}

Element* Schema::elementByLabel(const QString& label) const
{
    foreach (Element *elem, _items)
        if (elem->label() == label)
            return elem;
    return nullptr;
}

void Schema::insertElement(Element* elem, int index, bool event)
{
    if (isValid(index))
        _items.insert(index, elem);
    else
        _items.append(elem);

    elem->setOwner(this);

    if (event)
        _events.raise(SchemaEvents::ElemCreated, elem);
}

void Schema::deleteElement(Element* elem, bool event)
{
    deleteElement(_items.indexOf(elem), event);
}

void Schema::deleteElement(int index, bool event)
{
    if (!isValid(index)) return;

    Element *elem = _items.at(index);

    if (event)
        _events.raise(SchemaEvents::ElemDeleting, elem);

    _items.remove(index);
    elem->setOwner(nullptr);

    if (event)
        _events.raise(SchemaEvents::ElemDeleted, elem);

    // TODO delete elem? rename to extractElem or smth like, if no real deletion needed
}

void Schema::parameterChanged(Z::ParameterBase *param)
{
    if (param == &_wavelength)
        _events.raise(SchemaEvents::LambdaChanged);
}

void Schema::setTripType(TripType value)
{
    if (_tripType == value) return;
    _tripType = value;
    _events.raise(SchemaEvents::ParamsChanged);
}
