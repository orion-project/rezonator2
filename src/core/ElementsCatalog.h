#ifndef ELEMENTS_CATALOG_H
#define ELEMENTS_CATALOG_H

#include "Element.h"
#include "core/OriTemplates.h"

class ElementsCatalog : public Singleton<ElementsCatalog>
{
public:
    const QStringList& categories() const { return _categories; }

    /// Returns elements of specific category
    Elements elements(const QString &category) const;

    /// Returns all elements
    const Elements& elements() const { return _elements; }

    /// Creates new element of the specific type.
    Element* create(const QString& type) const;

    /// Registeres element in the catalog at the specific category.
    void registerElement(const QString& category, Element *elem);

protected:
    ElementsCatalog();
    ~ElementsCatalog();

private:
    Elements _elements;
    QStringList _categories;
    QMap<QString, Elements> _elemsCategorized;

    friend class Singleton<ElementsCatalog>;
};

#endif // ELEMENTS_CATALOG_H
