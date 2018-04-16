#ifndef Z_PARAMETERS_H
#define Z_PARAMETERS_H

#include <QDebug>

#include "Units.h"
#include "Values.h"
#include "core/OriFilter.h"

namespace Z {

enum class ParamsEditor { None, List, ABCD };

class ParameterBase;

//------------------------------------------------------------------------------
/**
    Base class for objects which want to listen to parameter updates.
*/
class ParameterListener
{
public:
    /// Method is called when a new value has been assigned for a parameter.
    virtual void parameterChanged(ParameterBase*) {}
};

//------------------------------------------------------------------------------
/**
    Base class for schema parameters.
    Required type of parameter value should be defined by inherited class,
    @see ValuedParameter and @see PhysicalParameter for example.
*/
class ParameterBase
{
public:
    /// Storable name of parameter.
    const QString& alias() const { return _alias; }

    /// Label of parameter. E.g.: L, alpha, n<sub>0</sub>.
    /// Can contains some html tags (subscript or superscript).
    const QString& label() const { return _label; }

    /// Name of parameter. E.g.: Length, Inclination angle, etc.
    const QString& name() const { return _name; }

    /// Description of paramter. Short help string.
    const QString& description() const { return _description; }

    /// Parameter's category (group of parameters).
    const QString& category() const { return _category; }

    /// If parameter must be visible for editors.
    /// Internal or service parameters can be marked as invisible.
    /// TODO: use category and @a ParameterFilterCondition for this.
    bool visible() const { return _visible; }
    void setVisible(bool v) { _visible = v; }

    /// Returns string representation of parameter.
    virtual QString str() const = 0;

    void addListener(ParameterListener* listener) { _listeners.append(listener); }
    void removeListener(ParameterListener* listener) { _listeners.removeAll(listener); }

protected:
    ParameterBase() {}

    ParameterBase(const QString& alias,
                  const QString& label,
                  const QString& name,
                  const QString& description,
                  const QString& category,
                  bool visible) :
        _alias(alias),
        _label(label),
        _name(name),
        _description(description),
        _category(category),
        _visible(visible) {}

    virtual ~ParameterBase() {}

    void notifyListeners()
    {
        for (auto listener: _listeners)
            listener->parameterChanged(this);
    }

private:
    QString _alias, _label, _name, _description, _category;
    bool _visible = true;
    QVector<ParameterListener*> _listeners;
};

//------------------------------------------------------------------------------
/**
    Base class template for parameter value verifiers.
*/
template <typename TValue>
class ValueVerifierBase
{
public:
    /// When verifier is not enabled, parameters should not use it for verification.
    virtual bool enabled() const { return false; }

    /// Verify parameter value.
    /// Inherited classes should implement actual verification logic.
    /// Should return empty string if value is valid, or reason why one is invalid.
    virtual QString verify(const TValue& value) const { Q_UNUSED(value); return QString(); }
};

//------------------------------------------------------------------------------
/**
    Base class template for parameters having value.
*/
template <typename TValue>
class ValuedParameter : public ParameterBase
{
public:
    /// Get parameter value
    const Value& value() const { return _value; }

    /// Set parameter value
    void setValue(const Value& value)
    {
        _value = value;
        notifyListeners();
    }

    /// Verify parameter value.
    /// Should be called before value assignment.
    QString verify(const Value& value)
    {
        return _verifier && _verifier->enabled()? _verifier->verify(value): QString();
    }

    /// Use verifier for parameter.
    /// Parameter does not take ownership on verifier.
    void setVerifier(ValueVerifierBase<Value>* verifier) { _verifier = verifier; }

protected:
    ValuedParameter() : ParameterBase() {}

    ValuedParameter(const QString& alias,
                    const QString& label,
                    const QString& name,
                    const QString& description,
                    const QString& category,
                    bool visible) :
        ParameterBase(alias, label, name, description, category, visible) {}

private:
    Value _value;
    ValueVerifierBase<Value> *_verifier = nullptr;
};

//------------------------------------------------------------------------------
/**
    Parameter class representing physical values with units of measurement.
*/
class PhysicalParameter : public ValuedParameter<Value>
{
public:
    PhysicalParameter() : ValuedParameter() {}

    PhysicalParameter(Dim dim,
                      const QString& alias) :
        ValuedParameter(alias, "", "", "", "", true), _dim(dim) {}

    PhysicalParameter(Dim dim,
                      const QString& alias,
                      const QString& label,
                      const QString& name) :
        ValuedParameter(alias, label, name, "", "", true), _dim(dim) {}

    PhysicalParameter(Dim dim,
                      const QString& alias,
                      const QString& label,
                      const QString& name,
                      const QString& description) :
        ValuedParameter(alias, label, name, description, "", true), _dim(dim) {}

    PhysicalParameter(Dim dim,
                      const QString& alias,
                      const QString& label,
                      const QString& name,
                      const QString& description,
                      const QString& category,
                      bool visible = true) :
        ValuedParameter(alias, label, name, description, category, visible), _dim(dim) {}

    /// Measurement units of parameter.
    Dim dim() const { return _dim; }

    /// Returns string representation of parameter.
    QString str() const override
    {
        return label() % " = " % value().str();
    }

private:
    Dim _dim = Dims::none();
};

//------------------------------------------------------------------------------
/**
    Parameter link listen for source parameter and assigns its value to target parameter.
*/
template <typename TParam>
class ParameterLink : public ParameterListener
{
public:
    ParameterLink(TParam *source, TParam *target) : _source(source), _target(target)
    {
        _source->addListener(this);
        apply();
    }

    virtual ~ParameterLink()
    {
        _source->removeListener(this);
    }

    void parameterChanged(ParameterBase *param) override
    {
        if (param == _source) apply();
    }

    void apply() const
    {
        auto value = _source->value();
        auto res = _target->verify(value);
        if (res.isEmpty())
            _target->setValue(value);
        else
            qWarning() << "Param link" << str() << "Unable to set value to target, verification failed" << res;
    }

    QString str() const { return _source->alias() % " --> " % _target->alias(); }

    TParam* source() const { return _source; }
    TParam* target() const { return _target; }

private:
    TParam *_source, *_target;
};

//------------------------------------------------------------------------------
/**
    Generic container for list of parameter links.
    It's just vector with several additional methods.
*/
template <class TLink>
class ParameterLinksList : public QVector<TLink*>
{
public:
    ParameterLinksList(): QVector<TLink*>() {}

    TLink* bySource(void *source) const;
    TLink* byTarget(void *target) const;
};

//------------------------------------------------------------------------------
/**
    Generic container for list of parameters.
    It's just vector with several additional methods.
*/
template <class TParam>
class ParametersList : public QVector<TParam*>
{
public:
    ParametersList(): QVector<TParam*>() {}
    ParametersList(const QVector<TParam*>& other) : QVector<TParam*>(other) {}
    ParametersList(std::initializer_list<TParam*> args): QVector<TParam*>(args) {}

    TParam* byAlias(const QString& alias);
    TParam* byIndex(int index);
    TParam* byPointer(void *param);

    ParametersList mid(int pos, int len) const
    {
        return ParametersList(QVector<TParam*>::mid(pos, len));
    }

    QString str() const;
};

//------------------------------------------------------------------------------

typedef PhysicalParameter Parameter;
typedef ParametersList<Parameter> Parameters;
typedef ParameterLink<Parameter> ParamLink;
typedef ParameterLinksList<ParamLink> ParamLinks;
typedef ValueVerifierBase<Z::Value> ValueVerifier;

//------------------------------------------------------------------------------

class ParameterFilterCondition {
public:
    virtual ~ParameterFilterCondition() {}
    virtual bool check(Parameter*) = 0;
};

typedef Ori::Filter<Parameter*, ParameterFilterCondition> ParameterFilter;

class ParameterFilterVisible : public ParameterFilterCondition
{
public:
    bool check(Parameter* p) override { return p->visible(); }
};

} // namespace Z

#endif // Z_PARAMETERS_H
