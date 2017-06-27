#include "Format.h"
#include "Parameters.h"

namespace Z {

//////////////////////////////////////////////////////////////////////////////////
//                                ParameterBase
//////////////////////////////////////////////////////////////////////////////////

ParameterBase::ParameterBase() : _dim(Z::Dims::none()), _visible(false)
{
}

ParameterBase::ParameterBase(Z::Dim dim, const QString& alias, const QString& label, const QString& name,
                             const QString& description, const QString& category, bool visible) :
    _dim(dim), _alias(alias), _label(label), _name(name), _description(description), _category(category), _visible(visible)
{
}

//////////////////////////////////////////////////////////////////////////////////
//                            ValuedParameter<TValue>
//////////////////////////////////////////////////////////////////////////////////

template <class TValue>
void ValuedParameter<TValue>::setValue(const TValue& value)
{
    if (!verify(value).isEmpty())
        return;

    _value = value;

    if (_owner)
        _owner->parameterChanged(this);
}

template <class TValue>
QString ValuedParameter<TValue>::verify(const TValue& value)
{
    return _verifier.enabled()? _verifier.verify(value): QString();
}

template <class TValue>
QString ValuedParameter<TValue>::str() const
{
    return label() % " = " % _value.str();
}

template void Parameter::setValue(const Z::Value& value);
template QString Parameter::verify(const Z::Value& value);
template QString Parameter::str() const;

//////////////////////////////////////////////////////////////////////////////////
//                               ParametersList<TParam>
//////////////////////////////////////////////////////////////////////////////////

template <class TParam> QString ParametersList<TParam>::str() const
{
    QStringList s;
    for (int i = 0; i < QVector<TParam*>::size(); i++)
    {
        auto p = QVector<TParam*>::at(i);
        if (p->visible()) s << p->str();
    }
    return s.join("; ");
}


//Parameter* Parameters::operator[](int index)
//{
//    if (index < 0 || index >= this->size()) return nullptr;
//    return const_cast<Parameter*>(&(this->at(index)));
//}

template <class TParam> TParam* ParametersList<TParam>::byAlias(const QString& alias)
{
    for (int i = 0; i < this->size(); i++)
        if (this->at(i)->alias() == alias)
            return const_cast<TParam*>(this->at(i));
    return nullptr;
}

//NamedValues Parameters::getValues()
//{
//    NamedValues values;
//    for (const Z::Parameter& param : *this)
//        values.insert(param.name, param.value);
//    return values;
//}

//void Parameters::setValues(const NamedValues& values)
//{
//    for (Z::Parameter& param : *this)
//        if (values.contains(param.name))
//            param.value = values[param.name];
//}

template QString Parameters::str() const;
template Parameter* Parameters::byAlias(const QString& alias);

} // namespace Z
