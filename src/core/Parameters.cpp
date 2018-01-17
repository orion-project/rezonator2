#include "Format.h"
#include "Parameters.h"

namespace Z {

//--------------------------------------------------------------------------------
//                                ParameterBase
//--------------------------------------------------------------------------------

ParameterBase::ParameterBase() : _dim(Z::Dims::none()), _visible(false)
{
}

ParameterBase::ParameterBase(Z::Dim dim, const QString& alias, const QString& label, const QString& name,
                             const QString& description, const QString& category, bool visible) :
    _dim(dim), _alias(alias), _label(label), _name(name), _description(description), _category(category), _visible(visible)
{
}

void ParameterBase::notifyListeners()
{
    for (auto listener: _listeners)
        listener->parameterChanged(this);
}

//--------------------------------------------------------------------------------
//                            ValuedParameter<TValue>
//--------------------------------------------------------------------------------

template <class TValue>
void ValuedParameter<TValue>::setValue(const TValue& value)
{
    _value = value;
    notifyListeners();
}

template <class TValue>
QString ValuedParameter<TValue>::verify(const TValue& value)
{
    return _verifier && _verifier->enabled()? _verifier->verify(value): QString();
}

template <class TValue>
QString ValuedParameter<TValue>::str() const
{
    return label() % " = " % _value.str();
}

template void Parameter::setValue(const Z::Value& value);
template QString Parameter::verify(const Z::Value& value);
template QString Parameter::str() const;

//--------------------------------------------------------------------------------
//                               ParametersList<TParam>
//--------------------------------------------------------------------------------

template <class TParam> QString ParametersList<TParam>::str() const
{
    QStringList s;
    for (int i = 0; i < this->size(); i++)
    {
        auto p = this->at(i);
        if (p->visible()) s << p->str();
    }
    return s.join("; ");
}

template <class TParam> TParam* ParametersList<TParam>::byAlias(const QString& alias)
{
    for (int i = 0; i < this->size(); i++)
        if (this->at(i)->alias() == alias)
            return (*this)[i];
    return nullptr;
}

template <class TParam> TParam* ParametersList<TParam>::byIndex(int index)
{
    return (index >= 0 && index < this->size())? (*this)[index]: nullptr;
}

// Template implementations for ParametersList<ValuedParameter<Value> > aka Parameters
template QString Parameters::str() const;
template Parameter* Parameters::byAlias(const QString& alias);
template Parameter* Parameters::byIndex(int index);

} // namespace Z
