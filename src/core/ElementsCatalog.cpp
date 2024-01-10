#include "ElementsCatalog.h"
#include "Elements.h"

#include <QApplication>

ElementsCatalog::ElementsCatalog()
{
    auto categoryCommon = qApp->translate("Elements", "Common elements");
    registerElement(categoryCommon, new ElemEmptyRange);
    registerElement(categoryCommon, new ElemPlate);
    registerElement(categoryCommon, new ElemFlatMirror);
    registerElement(categoryCommon, new ElemCurveMirror);
    registerElement(categoryCommon, new ElemThinLens);
    registerElement(categoryCommon, new ElemTiltedCrystal);
    registerElement(categoryCommon, new ElemTiltedPlate);
    registerElement(categoryCommon, new ElemBrewsterCrystal);
    registerElement(categoryCommon, new ElemBrewsterPlate);

    auto categoryAux = qApp->translate("Elements", "Additional elements");
    registerElement(categoryAux, new ElemMatrix);
    registerElement(categoryAux, new ElemMatrix1);
    //registerElement(categoryAux, new ElemFormula);
    registerElement(categoryAux, new ElemPoint);
    registerElement(categoryAux, new ElemThickLens);
    registerElement(categoryAux, new ElemCylinderLensT);
    registerElement(categoryAux, new ElemCylinderLensS);
    registerElement(categoryAux, new ElemGrinLens);
    registerElement(categoryAux, new ElemThermoLens);
    registerElement(categoryAux, new ElemAxiconMirror);
    registerElement(categoryAux, new ElemAxiconLens);

    auto categoryIntf = qApp->translate("Elements", "Media, interfaces");
    registerElement(categoryIntf, new ElemMediumRange);
    registerElement(categoryIntf, new ElemGrinMedium);
    registerElement(categoryIntf, new ElemThermoMedium);
    registerElement(categoryIntf, new ElemNormalInterface);
    registerElement(categoryIntf, new ElemBrewsterInterface);
    registerElement(categoryIntf, new ElemTiltedInterface);
    registerElement(categoryIntf, new ElemSphericalInterface);

    auto categoryCplx = qApp->translate("Elements", "Complex elements");
    registerElement(categoryCplx, new ElemGaussAperture);
    registerElement(categoryCplx, new ElemGaussApertureLens);
    registerElement(categoryCplx, new ElemGaussDuctMedium);
    registerElement(categoryCplx, new ElemGaussDuctSlab);
}

ElementsCatalog::~ElementsCatalog()
{
    qDeleteAll(_elements);
}

void ElementsCatalog::registerElement(const QString &category, Element *elem)
{
    foreach (Element *e, _elements)
        if (e->type() == elem->type())
            return;

    if (!_categories.contains(category))
        _categories.append(category);

    if (!_elements.contains(elem))
        _elements.append(elem);

    if (!_elemsCategorized[category].contains(elem))
        _elemsCategorized[category].append(elem);
}

Element* ElementsCatalog::create(const QString& type) const
{
    for (Element* maker : _elements)
        if (maker->type() == type)
            return maker->create();
    return nullptr;
}

Element* ElementsCatalog::create(const Element* sample, bool copyParams) const
{
    auto newElem = create(sample->type());

    if (copyParams)
        Z::Utils::copyParamValues(sample, newElem, "ElementsCatalog: create elem from sample");

    return newElem;
}

Elements ElementsCatalog::elements(const QString& category) const
{
    return _elemsCategorized.contains(category)? _elemsCategorized[category]: Elements();
}

QStringList ElementsCatalog::getMediumTypeNames() const
{
    QStringList res;
    for (auto elem : _elements)
        if (Z::Utils::isMedium(elem))
            res << elem->typeName();
    return res;
}

QStringList ElementsCatalog::getInterfaceTypeNames() const
{
    QStringList res;
    for (auto elem : _elements)
        if (Z::Utils::isInterface(elem))
            res << elem->typeName();
    return res;
}
