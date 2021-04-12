#ifndef ELEMENTS_CATALOG_H
#define ELEMENTS_CATALOG_H

#include "Element.h"
#include "core/OriTemplates.h"

class ElementsCatalog : public Ori::Singleton<ElementsCatalog>
{
public:
    const QStringList& categories() const { return _categories; }

    /// Returns elements of specific category
    Elements elements(const QString &category) const;

    /// Returns all elements
    const Elements& elements() const { return _elements; }

    /// Creates a new element of the specific type.
    Element* create(const QString& type) const;

    /// Creates a new element of the same type as the sample
    /// and copies all parameter values to the target element, if required.
    Element* create(const Element* sample, bool copyParams = false) const;

    /// Registeres element in the catalog at the specific category.
    void registerElement(const QString& category, Element *elem);

    QStringList getMediumTypeNames() const;
    QStringList getInterfaceTypeNames() const;

protected:
    ElementsCatalog();
    ~ElementsCatalog();

private:
    Elements _elements;
    QStringList _categories;
    QMap<QString, Elements> _elemsCategorized;

    friend class Ori::Singleton<ElementsCatalog>;
};

#endif // ELEMENTS_CATALOG_H
