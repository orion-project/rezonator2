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
    Base class for objects who want to own parameters.
*/
class ParameterOwner
{
public:
    /// Check parameter value before it actually will be assigned.
    //virtual QString checkParameter(ParameterBase*, double) const { return QString(); }

    /// Method is called when a new value has been assigned for a parameter.
    virtual void parameterChanged(ParameterBase*) {}
};

//------------------------------------------------------------------------------

class ParameterBase
{
public:
    /// Measurement units of parameter.
    Z::Dim dim() const { return _dim; }

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
    bool visible() const { return _visible; }
    void setVisible(bool v) { _visible = v; }

    virtual QString str() const = 0;

    void setOwner(ParameterOwner* owner) { _owner = owner; }
    ParameterOwner* owner() const { return _owner; }

protected:
    ParameterBase();

    ParameterBase(Z::Dim dim,
                  const QString& alias,
                  const QString& label,
                  const QString& name,
                  const QString& description,
                  const QString& category,
                  bool visible = true);

    virtual ~ParameterBase() {}

    ParameterOwner* _owner = nullptr;

private:
    Z::Dim _dim;
    QString _alias, _label, _name, _description, _category;
    bool _visible;
};

//------------------------------------------------------------------------------

template <typename TValue> class ValueVerifierBase
{
public:
    virtual bool enabled() const { return false; }
    virtual QString verify(const TValue& value) const { Q_UNUSED(value); return QString(); }
};

//------------------------------------------------------------------------------

template <typename TValue> class ValuedParameter : public ParameterBase
{
public:
    ValuedParameter() : ParameterBase() {}

    ValuedParameter(Z::Dim dim, const QString& alias, const QString& label, const QString& name) :
        ParameterBase(dim, alias, label, name, "", "") {}

    ValuedParameter(Z::Dim dim, const QString& alias, const QString& label, const QString& name, const QString& description) :
        ParameterBase(dim, alias, label, name, description, "") {}

    ValuedParameter(Z::Dim dim, const QString& alias, const QString& label, const QString& name, const QString& description, const QString& category, bool visible = true) :
        ParameterBase(dim, alias, label, name, description, category, visible) {}

    const TValue& value() const { return _value; }
    void setValue(const TValue& value);

    QString verify(const TValue& value);
    void setVerifier(ValueVerifierBase<TValue>* verifier) { _verifier = verifier; }

    QString str() const override;

private:
    TValue _value;
    ValueVerifierBase<TValue> *_verifier = nullptr;
};

//------------------------------------------------------------------------------
/**
    Container for list of parameters.
*/
template <class TParam> class ParametersList : public QVector<TParam*>
{
public:
    ParametersList(): QVector<TParam*>() {}
    ParametersList(const QVector<TParam*>& other) : QVector<TParam*>(other) {}
    ParametersList(std::initializer_list<TParam*> args): QVector<TParam*>(args) {}

    TParam* byAlias(const QString& alias);

    ParametersList mid(int pos, int len) const
    {
        return ParametersList(QVector<TParam*>::mid(pos, len));
    }

    QString str() const;
};

//------------------------------------------------------------------------------

typedef ValuedParameter<Z::Value> Parameter;
typedef ValueVerifierBase<Z::Value> ValueVerifier;
typedef ParametersList<Parameter> Parameters;

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
