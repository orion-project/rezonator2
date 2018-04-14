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
TParam* ParametersList<TParam>::byAlias(const QString& alias)
{
    for (int i = 0; i < this->size(); i++)
        if (this->at(i)->alias() == alias)
            return (*this)[i];
    return nullptr;
}

template <class TParam>
TParam* ParametersList<TParam>::byIndex(int index)
{
    return (index >= 0 && index < this->size())? (*this)[index]: nullptr;
}

template <class TParam>
TParam* ParametersList<TParam>::byPointer(void *param)
{
    for (int i = 0; i < this->size(); i++)
        if (this->at(i) == param)
            return (*this)[i];
    return nullptr;
}

// Template implementations for ParametersList<PhysicalParameter> aka Parameters
template QString Parameters::str() const;
template Parameter* Parameters::byAlias(const QString& alias);
template Parameter* Parameters::byIndex(int index);
template Parameter* Parameters::byPointer(void *param);

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
