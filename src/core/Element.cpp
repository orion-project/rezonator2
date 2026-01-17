#include "Element.h"

#include "Formula.h"
#include "Perf.h"
#include "Protocol.h"

#include <QApplication>

//------------------------------------------------------------------------------
//                                ElementOwner
//------------------------------------------------------------------------------

ElementOwner::~ElementOwner()
{
}

//------------------------------------------------------------------------------
//                                Element
//------------------------------------------------------------------------------

Element::Element()
{
    static int id = 0;
    _id = ++id;
    
    _matrs[MatrixKind::InvT] = Z::Matrix();
    _matrs[MatrixKind::InvS] = Z::Matrix();
}

Element::~Element()
{
    qDeleteAll(_params);

    setOwner(nullptr);
}

void Element::setOwner(ElementOwner *owner)
{
    _owner = owner;
}

QString Element::displayLabel()
{
    if (!_label.isEmpty())
        return _label;
    if (_owner)
        return QString("#%1").arg(_owner->indexOf(this)+1);
    return typeName();
}

QString Element::displayTitle()
{
    if (!_title.isEmpty())
        return _title;
    return displayLabel();
}

QString Element::displayLabelTitle()
{
    if (!_label.isEmpty())
    {
        if (!_title.isEmpty())
            return QString("%1 (%2)").arg(_label, _title);
        return _label;
    }
    if (!_title.isEmpty())
        return _title;
    if (_owner)
        return QString("#%1 (%2)").arg(_owner->indexOf(this)+1).arg(typeName());
    return typeName();
}

void Element::addParam(Z::Parameter *param, int index)
{
    param->addListener(this);
    if (index < 0 || index >= _params.size())
        _params.append(param);
    else _params.insert(index, param);
}

void Element::removeParam(Z::Parameter *param, bool free)
{
    param->removeListener(this);
    _params.removeOne(param);
    if (free) delete param;
}

void Element::parameterChanged(Z::ParameterBase *p)
{
    Z_PERF_BEGIN("Element::parameterChanged_1")
    if (_calcMatrixLocked)
        _calcMatrixNeeded = true;
    else
        calcMatrix("Element::parameterChanged");
    Z_PERF_END

    Z_PERF_BEGIN("Element::parameterChanged_2")
    if (!_eventsLocked && _owner)
        _owner->elementChanged(this, p, QStringLiteral("Element::parameterChanged(%1)").arg(p->alias()));
    Z_PERF_END
}

void Element::parameterFailed(Z::ParameterBase *p)
{
    if (!_eventsLocked && _owner)
        _owner->elementChanged(this, p, QStringLiteral("Element::parameterFailed(%1)").arg(p->alias()));
}

void Element::calcMatrix(const char *reason)
{
    Q_UNUSED(reason)
    //qDebug() << "Calc matrix" << type() << displayLabel() << reason;
    calcMatrixInternal();
}

void Element::calcMatrixInternal()
{
    _mt.unity();
    _ms.unity();
    for (auto it = _matrs.begin(); it != _matrs.end(); it++)
        it->second.unity();
}

void Element::setLabel(const QString& value)
{
    _label = value;
    if (!_eventsLocked && _owner)
        _owner->elementChanged(this, nullptr, "Element::setLabel");
}

void Element::setTitle(const QString& value)
{
    _title = value;
    if (!_eventsLocked && _owner)
        _owner->elementChanged(this, nullptr, "Element::setTitle");
}

void Element::setDisabled(bool value)
{
    _disabled = value;
    if (!_eventsLocked && _owner)
        _owner->elementChanged(this, nullptr, "Element::setDisabled");
}

bool Element::failed() const
{
    for (auto p : std::as_const(_params))
        if (p->failed())
            return true;
    return false;
}

QString Element::failReason() const
{
    for (auto p : std::as_const(_params))
        if (p->failed())
            return qApp->tr("Parameter %1 failed: %2").arg(p->displayLabel(), p->error());
    return QString();
}

std::optional<ElemAsDynamic> Element::asDynamic()
{
    if (_kind == ElementKind::Dynamic)
        return ElemAsDynamic{ .elem = this };
    return {};
}

std::optional<ElemAsInterface> Element::asInterface()
{
    if (_kind == ElementKind::Interface)
        return ElemAsInterface{ .elem = this };
    return {};
}

//------------------------------------------------------------------------------
//                               ElementRange
//------------------------------------------------------------------------------

ElementRange::ElementRange()
{
    _length =  new Z::Parameter(Z::Dims::linear(),
                                QStringLiteral("L"), QStringLiteral("L"),
                                qApp->translate("Param", "Length"));
    _ior = new Z::Parameter(Z::Dims::none(),
                            QStringLiteral("n"), QStringLiteral("n"),
                            qApp->translate("Param", "Index of refraction"));

    // It is internal parameter by default,
    // and should be explicitly revealed by derived elements
    _ior->setVisible(false);

    _length->setValue(100_mm);
    _ior->setValue(1);

    addParam(_length);
    addParam(_ior);
}

void ElementRange::setSubRange(const Z::Value& value)
{
    double v = value.toSi();
    if (v < 0) v = 0;
    else
    {
        double len = axisLengthSI();
        if (v > len) v = len;
    }
    setSubRangeSI(v);
}

Z::Value ElementRange::subRangeLf() const
{
    auto unit = paramLength()->value().unit();
    return {unit->fromSi(subRangeSI()), unit};
}

Z::Value ElementRange::subRangeRt() const
{
    auto unit = paramLength()->value().unit();
    return {unit->fromSi(axisLengthSI() - subRangeSI()), unit};
}

Z::Value ElementRange::axisLen() const
{
    auto unit = paramLength()->value().unit();
    return {unit->fromSi(axisLengthSI()), unit};
}

//------------------------------------------------------------------------------
//                               ElementEventsLocker
//------------------------------------------------------------------------------

ElementEventsLocker::ElementEventsLocker(Element* elem, const char *reason): _reason(reason)
{
    _elems << elem;
    elem->_eventsLocked = true;
    //qDebug() << "Lock events" << elem->displayLabel() << reason;
}

ElementEventsLocker::ElementEventsLocker(Z::Parameter* param, const char *reason): _reason(reason)
{
    collectElems(param);
    //qDebug() << "Lock events" << Z::Utils::displayStr(_elems) << reason;
}

ElementEventsLocker::~ElementEventsLocker()
{
    //qDebug() << "Unlock events" << Z::Utils::displayStr(_elems) << _reason;
    for (auto elem : std::as_const(_elems))
        elem->_eventsLocked = false;
}

void ElementEventsLocker::collectElems(Z::Parameter *param)
{
    for (auto listener : param->listeners()) {
        if (auto elem = dynamic_cast<Element*>(listener); elem) {
            _elems << elem;
            elem->_eventsLocked = true;
        }
        else if (auto link = dynamic_cast<Z::ParamLink*>(listener); link) {
            for (auto listener : link->target()->listeners())
                if (auto elem = dynamic_cast<Element*>(listener); elem) {
                    _elems << elem;
                    elem->_eventsLocked = true;
                }
        } else if (auto formula = dynamic_cast<Z::Formula*>(listener); formula) {
            collectElems(formula->target());
        }
    }
}

//------------------------------------------------------------------------------
//                                ElementParamsBackup
//------------------------------------------------------------------------------

ElementParamsBackup::ElementParamsBackup(Element *elem, const char *reason) : _elem(elem), _reason(reason)
{
    //qDebug() << "Backup elem params" << elem->displayLabel() << reason;
    for (auto p : elem->params())
        _backup.insert(p, std::shared_ptr<Z::ParamValueBackup>(new Z::ParamValueBackup(p, reason)));
}

ElementParamsBackup::~ElementParamsBackup()
{
    //qDebug() << "Restore elem params" << _elem->displayLabel() << _reason;
}

//------------------------------------------------------------------------------
//                                Z::Utils
//------------------------------------------------------------------------------

namespace Z {
namespace Utils {

void setElemWavelen(Element* elem, const Z::Value& lambda)
{
    QString paramName = QStringLiteral("Lambda");
    auto param = elem->params().byAlias(paramName);
    if (!param)
    {
        Z_WARNING("Element" << elem->displayLabel() << "is marked as wavelength requiring but doesn't provide parameter" << paramName)
        qWarning() << "Element" << elem->displayLabel() << "is marked as wavelength requiring but doesn't provide parameter" << paramName;
        return;
    }
    if (param->dim() != Z::Dims::linear())
    {
        Z_WARNING("Element" << elem->displayLabel() << "is marked as wavelength requiring "
            "but its parameter" << paramName << "has invalid dimension" << param->dim()->name() <<
            "while it should be" << Z::Dims::linear()->name())
        qWarning() << "Element" << elem->displayLabel() << "is marked as wavelength requiring "
            "but its parameter" << paramName << "has invalid dimension" << param->dim()->name() <<
            "while it should be" << Z::Dims::linear()->name();
        return;
    }
    param->setValue(lambda);
}

ParameterFilterPtr defaultParamFilter()
{
    static ParameterFilterPtr filter = std::make_shared<ParameterFilter>(
       std::initializer_list<ParameterFilterCondition*> { new ParameterFilterVisible });
    return filter;
}

void copyParamValues(const Element* source, Element* target, const char* reason)
{
    if (source->params().size() != target->params().size())
    {
        qWarning() << "copyParamValues" << reason << "elements have different parameters count";
        return;
    }
    ElementMatrixLocker matrixLocker(target, reason);
    auto params = source->params();
    for (int i = 0; i < params.count(); i++)
        target->params().at(i)->setValue(params.at(i)->value());
}

void copyParamValuesByName(const Element* source, Element* target, const char* reason)
{
    ElementMatrixLocker matrixLocker(target, reason);
    QMap<QString, Parameter*> targetParams;
    for (auto p : target->params())
        targetParams[p->alias()] = p;
    for (auto p : source->params())
    {
        auto p1 = targetParams.value(p->alias());
        if (p1 && p1->dim() == p->dim())
            p1->setValue(p->value());
    }
}

QString displayStr(const Elements &elems)
{
    QStringList s;
    for (const auto &elem : elems)
        s << elem->displayLabel();
    return s.join(',');
}

} // namespace Utils
} // namespace Z

