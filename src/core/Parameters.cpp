#include "Parameters.h"

namespace Z {

//--------------------------------------------------------------------------------
//                               ParametersList<TParam>
//--------------------------------------------------------------------------------

template <class TParam>
QString ParametersList<TParam>::str() const
{
    QStringList s;
    for (int i = 0; i < this->size(); i++)
    {
        auto p = this->at(i);
        if (p->visible()) s << p->str();
    }
    return s.join("; ");
}

template <class TParam>
QString ParametersList<TParam>::displayStr() const
{
    QStringList s;
    for (int i = 0; i < this->size(); i++)
    {
        auto p = this->at(i);
        if (p->visible()) s << p->displayStr();
    }
    return s.join("; ");
}

template <class TParam>
TParam* ParametersList<TParam>::byAlias(const QString& alias) const
{
    for (int i = 0; i < this->size(); i++)
        if (this->at(i)->alias() == alias)
            return (*this)[i];
    return nullptr;
}

template <class TParam>
TParam* ParametersList<TParam>::byIndex(int index) const
{
    return (index >= 0 && index < this->size())? (*this)[index]: nullptr;
}

template <class TParam>
TParam* ParametersList<TParam>::byPointer(void *param) const
{
    for (int i = 0; i < this->size(); i++)
        if (this->at(i) == param)
            return (*this)[i];
    return nullptr;
}

template QString Parameters::str() const;
template QString Parameters::displayStr() const;
template Parameter* Parameters::byAlias(const QString& alias) const;
template Parameter* Parameters::byIndex(int index) const;
template Parameter* Parameters::byPointer(void *param) const;

template QString ParametersBase::str() const;
template QString ParametersBase::displayStr() const;
template ParameterBase* ParametersBase::byAlias(const QString& alias) const;
template ParameterBase* ParametersBase::byIndex(int index) const;
template ParameterBase* ParametersBase::byPointer(void *param) const;

template QString ParametersTS::str() const;
template QString ParametersTS::displayStr() const;
template ParameterTS* ParametersTS::byAlias(const QString& alias) const;
template ParameterTS* ParametersTS::byIndex(int index) const;
template ParameterTS* ParametersTS::byPointer(void *param) const;

//--------------------------------------------------------------------------------
//                           ParameterLinksList<TLink>
//--------------------------------------------------------------------------------

template <class TLink>
TLink* ParameterLinksList<TLink>::bySource(void *source) const
{
    for (TLink *link : *this)
        if (link->source() == source)
            return link;
    return nullptr;
}

template <class TLink>
TLink* ParameterLinksList<TLink>::byTarget(void *target) const
{
    for (TLink *link : *this)
        if (link->target() == target)
            return link;
    return nullptr;
}

// Template implementations for ParameterLinksList<ParamLink> ParamLinks;
template ParamLink* ParamLinks::bySource(void*) const;
template ParamLink* ParamLinks::byTarget(void*) const;

} // namespace Z
