#include "Element.h"

#include <QApplication>

//------------------------------------------------------------------------------
//                                Element

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

int Element::addParam(Z::Parameter *param, const double& value, Z::Unit unit)
{
    param->setValue(Z::Value(value, unit));
    param->addListener(this); // do it after setValue() to suppress parameterChanged()
    _params.append(param);
    return _params.size()-1;
}

void Element::parameterChanged(Z::ParameterBase*)
{
    calcMatrix();
    if (!_locked && _owner)
        _owner->elementChanged(this);
}

void Element::calcMatrix()
{
    if (_disabled)
    {
        _mt.unity();
        _ms.unity();
    }
    else calcMatrixInternal();
}

void Element::calcMatrixInternal()
{
    _mt.unity();
    _ms.unity();
}

void Element::lock()
{
    _locked = true;
}

void Element::unlock(bool recalc)
{
    _locked = false;
    if (recalc)
        calcMatrix();
}

void Element::setLabel(const QString& value)
{
    _label = value;
    if (!_locked && _owner)
        _owner->elementChanged(this);
}

void Element::setTitle(const QString& value)
{
    _title = value;
    if (!_locked && _owner)
        _owner->elementChanged(this);
}

void Element::setDisabled(bool value)
{
    _disabled = value;
    calcMatrix();
    if (!_locked && _owner)
        _owner->elementChanged(this);
}

//------------------------------------------------------------------------------
//                               ElementRange

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

    addParam(_length, 100, Z::Units::mm());
    addParam(_ior, 1, Z::Units::none());
}

//------------------------------------------------------------------------------
//                            ElementInterface

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

    addParam(_ior1, 1, Z::Units::none());
    addParam(_ior2, 1, Z::Units::none());
}

//------------------------------------------------------------------------------
//                              ElementsNamer

QString ElementsNamer::generateLabel(const QString& prefix)
{
    int index = _autoLabels[prefix];
    _autoLabels.insert(prefix, ++index);
    return prefix + QString::number(index);
}

void ElementsNamer::reset()
{
    _autoLabels.clear();
}

//------------------------------------------------------------------------------
//                                Z::Utils
namespace Z {
namespace Utils {

void generateLabel(Element *elem)
{
    elem->setLabel(ElementsNamer::instance().generateLabel(elem->labelPrefix()));
}

ParameterFilter* defaultParamFilter()
{
    static ParameterFilter filter({ new ParameterFilterVisible });
    return &filter;
}

} // namespace Utils
} // namespace Z

