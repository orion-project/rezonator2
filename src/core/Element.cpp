#include "Element.h"

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

void Element::parameterChanged(Z::ParameterBase*)
{
    if (_calcMatrixLocked)
        _calcMatrixNeeded = true;
    else
        calcMatrix("Element::parameterChanged");

    if (!_eventsLocked && _owner)
        _owner->elementChanged(this);
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
    _mt_inv.unity();
    _ms_inv.unity();
}

void Element::setLabel(const QString& value)
{
    _label = value;
    if (!_eventsLocked && _owner)
        _owner->elementChanged(this);
}

void Element::setTitle(const QString& value)
{
    _title = value;
    if (!_eventsLocked && _owner)
        _owner->elementChanged(this);
}

void Element::setDisabled(bool value)
{
    _disabled = value;
    if (!_eventsLocked && _owner)
        _owner->elementChanged(this);
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
//                            ElementInterface
//------------------------------------------------------------------------------

ElementInterface::ElementInterface()
{
    _ior1 = new Z::Parameter(Z::Dims::none(),
                            QStringLiteral("n1"), QStringLiteral("n1"),
                            qApp->translate("Param", "Index of refraction (left medium)"));
    _ior2 = new Z::Parameter(Z::Dims::none(),
                            QStringLiteral("n2"), QStringLiteral("n2"),
                            qApp->translate("Param", "Index of refraction (right medium)"));

    // These parameters can't be directly assigned,
    // their values are taked from neighboub range elements
    _ior1->setVisible(false);
    _ior2->setVisible(false);

    _ior1->setValue(1);
    _ior2->setValue(2);

    addParam(_ior1);
    addParam(_ior2);

    setOption(Element_Asymmetrical);

    layoutOptions.showLabel = false;
}

//------------------------------------------------------------------------------
//                                ElementDynamic
//------------------------------------------------------------------------------

void ElementDynamic::calcMatrixInternal()
{
    _mt.unity();
    _ms.unity();
    _mt_inv.unity();
    _ms_inv.unity();
    _mt_dyn.unity();
    _ms_dyn.unity();
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

ParameterFilter* defaultParamFilter()
{
    static ParameterFilter filter({ new ParameterFilterVisible });
    return &filter;
}

} // namespace Utils
} // namespace Z

