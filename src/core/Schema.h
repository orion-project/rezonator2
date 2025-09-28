#ifndef SCHEMA_H
#define SCHEMA_H

#include "CommonTypes.h"
#include "Element.h"
#include "Formula.h"
#include "Parameters.h"
#include "Pump.h"

#include "core/OriTemplates.h"
#include "core/OriArg.h"

#include <QApplication>
#include <QDebug>
#include <QImage>
#include <QMap>
#include <QPointer>

class Schema;

//------------------------------------------------------------------------------

namespace Arg {

using RaiseEvents = Ori::Argument<bool, struct RaiseEventsTag>;
using FreeElem = Ori::Argument<bool, struct FreeElemTag>;

} // namespace Arg

//------------------------------------------------------------------------------
/**
    Schema listener interface.
    Any object who wants "listen" any schema changes or other events should
    implement this interface and registers himself as a listener in a schema
    through the method @a Schema::registerListener(). The schema will call
    appropriate method of the interface when something occurs.
*/
class SchemaListener
{
public:
    virtual ~SchemaListener();

    virtual void schemaChanged(Schema*) {}

    virtual void schemaCreated(Schema*) {}
    virtual void schemaDeleted(Schema*) {}
    virtual void schemaSaved(Schema*) {}
    virtual void schemaLoading(Schema*) {}
    virtual void schemaLoaded(Schema*) {}
    virtual void schemaRebuilt(Schema*) {}

    virtual void elementCreated(Schema*, Element*) {}
    virtual void elementChanged(Schema*, Element*) {}
    virtual void elementDeleting(Schema*, Element*) {}
    virtual void elementDeleted(Schema*, Element*) {}

    virtual void elementsDeleting(Schema*) {}
    virtual void elementsDeleted(Schema*) {}

    virtual void schemaParamsChanged(Schema*) {}
    virtual void schemaLambdaChanged(Schema*) {}

    virtual void globalParamCreated(Schema*, Z::Parameter*) {}
    virtual void globalParamEdited(Schema*, Z::Parameter*) {}
    virtual void globalParamChanged(Schema*, Z::Parameter*) {}
    virtual void globalParamDeleting(Schema*, Z::Parameter*) {}
    virtual void globalParamDeleted(Schema*, Z::Parameter*) {}

    virtual void pumpCreated(Schema*, PumpParams*) {}
    virtual void pumpChanged(Schema*, PumpParams*) {}
    virtual void pumpCustomized(Schema*, PumpParams*) {}
    virtual void pumpDeleting(Schema*, PumpParams*) {}
    virtual void pumpDeleted(Schema*, PumpParams*) {}

    virtual void recalcRequired(Schema*) {}
};

//------------------------------------------------------------------------------

class SchemaState
{
public:
    enum State { None, New, Loading, Modified };
    static constexpr std::optional<State> Current = {};

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
        Changed,       ///< A generic event when something has been changed.
                       ///< Can be called after another changing event, 
                       ///< if the such is configured (@a SchemaEvents::propsOf()).
                       ///< Note that it can be raised many times caused by other events
                       ///< (ElemChanged, CustomParamChanged, etc) so there should not be
                       ///< a hard work done in a handler of this event, only something simple
                       ///< like UI updates. When a change requires recalulation, there is
                       ///< the dedicated event RecalRequred that must be raised explicitly
                       ///< in critical points (e.g. @a AdjusterWidget::changeValue())

        Created,       ///< Schema just was created, called from the constructor
        Deleted,       ///< Schema was deleted, called from destructor
        Saved,         ///< Schema was saved
        Loading,       ///< Schema is turned into Loading state
        Loaded,        ///< Loading is completed
        Rebuilt,       ///< Elements were rearranged

        ElemCreated,   ///< New element was added to schema
        ElemChanged,   ///< Element's params changed
        ElemDeleting,  ///< Element is being deleted from schema (per element event)
        ElemDeleted,   ///< Element was deleted from schema (per element event)

        ElemsDeleting, ///< Elements will be deleted from schema (group event)
        ElemsDeleted,  ///< Elements was deleted from schema (group event)

        ParamsChanged, ///< Some schema parameter was changed (e.g. TripType)
        LambdaChanged, ///< Schema wavelength was changed

        GlobalParamCreated, ///< New global parameter was created
        GlobalParamEdited,  ///< Global parameter was edited (e.g. description changed, but not value)
        GlobalParamChanged, ///< Value of global parameter was changed
        GlobalParamDeleting,///< Global param is about to be deleted
        GlobalParamDeleted, ///< Global parameter was deleted

        PumpCreated,   ///< New pump was created
        PumpChanged,   ///< Pump parameters were changed
        PumpCustomized,///< Pump color was changed
        PumpDeleting,  ///< Pump is about to be deleted
        PumpDeleted,   ///< Pump was deleted

        RecalRequred,  ///< Something is done that requires recalculation of all functions
    };

    void raise(Event event, const char* reason) const { raise(event, nullptr, reason); }
    void raise(Event event, void* param, const char* reason) const;

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
        
        /// Should also raise event 'changed'
        bool shouldRaiseChanged;
        
        /// New state which schema obtains with this event.
        /// If not set, then state is not changed
        std::optional<SchemaState::State> nextState;
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

class ISchemaMemoEditor
{
public:
    /// Save the current editor content to schema properties.
    /// The method is called before schema saving in order not to lose the latest editing.
    virtual void saveMemo() = 0;
};

struct SchemaMemo
{
    QString text;
    QMap<QString, QImage> images;

    /// Smart pointer to memo editor window is used for saving memo content
    /// when the window is still opened. The window saves its text into schema
    /// when it closes, but opened window can contain unsaved changes.
    QPointer<QWidget> editor;
};

//------------------------------------------------------------------------------

class Schema : public ElementOwner, public Z::ParameterListener, public Ori::Notifier<SchemaListener>
{
public:
    Schema(const QString& alias = QString());
    ~Schema() override;

    const QString& fileName() const { return _fileName; }
    void setFileName(const QString& fileName) { _fileName = fileName; }

    const QString& title() const { return _title; }
    void setTitle(const QString& title) { _title = title; }

    const QString& notes() const { return _notes; }
    void setNotes(const QString& notes) { _notes = notes; }

    const QString& alias() const { return _alias; }

    Z::Parameter& wavelength() { return _wavelength; }
    double wavelenSi() const { return _wavelength.value().toSi(); }

    TripType tripType() const { return _tripType; }
    void setTripType(TripType value);
    inline bool isSW() const { return _tripType == TripType::SW; }
    inline bool isRR() const { return _tripType == TripType::RR; }
    inline bool isSP() const { return _tripType == TripType::SP; }
    inline bool isResonator() const { return _tripType == TripType::SW || _tripType == TripType::RR; }

    int count() const override { return _items.size(); }
    const Elements& elements() const { return _items; }
    int activeCount() const;
    Elements activeElements() const;
    Element* element(int index) const;
    Element* element(const QString& label) const { return elementByLabel(label); }
    Element* elementById(int id) const;
    Element* elementByLabel(const QString& label) const;
    int indexOf(Element *elem) const override { return _items.indexOf(elem); }
    Position position(Element*) const override;

    SchemaEvents& events() { return _events; }
    SchemaState& state() { return _state; }
    bool modified() const { return _state.current() == SchemaState::Modified; }
    
    void insertElements(const Elements& elems, int beforeIndex, Arg::RaiseEvents events);
    void deleteElements(const Elements& elems, Arg::RaiseEvents events, Arg::FreeElem free);

    SchemaSelection& selection() { return _selection; }
    Element* selectedElement() const { return _selection.element(); }
    Elements selectedElements() const { return _selection.elements(); }

    /// Additional params that can be added by user and used in formulas.
    const Z::Parameters* globalParams() { return &_globalParams->params(); }
    /// Additional params returned as elements for using in elem-and-param selectors
    const Element* globalParamsAsElem() { return _globalParams; }
    void addGlobalParam(Z::Parameter*);
    void removeGlobalParam(Z::Parameter*);

    /// Linst of all links which bind elements' parameter to custom parameters.
    Z::ParamLinks* paramLinks() { return &_paramLinks; }
    Z::ParamLink* addParamLink(Z::Parameter *source, Z::Parameter *target, int options = 0);

    /// List of all formulas operated on schema parameters.
    Z::Formulas* formulas() { return &_formulas; }

    /// Returns a list of global params that can be used as link sources or formula dependencies.
    /// It contains custom params and some of the built-in parameters.
    /// This list is not stored in a schema and collect parameters at each call.
    Z::Parameters availableDependencySources() const;
    
    Z::Parameter* param(const QString &alias) { return _globalParams->params().byAlias(alias); }

    PumpsList* pumps() { return &_pumps; }
    PumpParams* activePump();
    PumpParams* findPump(const QString& label);

    void moveElementUp(Element* elem);
    void moveElementDown(Element* elem);
    void flip();

    void markModified(const char* reason);

    void relinkInterfaces();

    SchemaMemo* memo = nullptr;

private:
    Elements _items;
    SchemaEvents _events;
    SchemaState _state;
    SchemaSelection _selection;
    QString _fileName;
    QString _title, _notes, _alias;
    TripType _tripType = TripType::SW;
    Z::Parameter _wavelength;
    Element* _globalParams;
    Z::ParamLinks _paramLinks;
    Z::Formulas _formulas;
    PumpsList _pumps;

    // inherits from ElementOwner
    void elementChanged(Element *elem, const QString &reason) override;

    // inherits from ParameterListener
    void parameterChanged(Z::ParameterBase *param) override;

    inline bool isValid(int index) const { return index >= 0 && index < _items.size(); }

    /// Remove links driving this elements' params
    void removeParamLinks(Element* elem);

    void shiftElement(int index, const std::function<int(int)> &getTargetIndex);
};


namespace Z {
namespace Utils {

/// Makes an automatic label for the given element.
/// Automatical label consist of a prefix like `M`, `L`, etc. and index.
void generateLabel(const Elements& elements, Element* elem, const QString& labelPrefix = QString());

/// Makes an automatic label with the given prefix
/// respecting all existing elements with the same prefix.
QString generateLabel(Schema* schema, const QString& prefix);

/// Makes an automatic label for the given pump: `P1`, `P2`, etc.
void generateLabel(Schema* schema, PumpParams* pump);

/// Returns an element owning the given parameter or null.
Element* findElemByParam(Schema* schema, Parameter* param);

} // namespace Utils
} // namespace Z

#endif // SCHEMA_H
