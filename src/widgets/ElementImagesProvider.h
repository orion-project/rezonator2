#ifndef ELEMENT_IMAGES_PROVIDER_H
#define ELEMENT_IMAGES_PROVIDER_H

#include "../core/Element.h"
#include "core/OriTemplates.h"

#include <QSize>

class Element;

class ElementImagesProvider: public Singleton<ElementImagesProvider>
{
public:
    QSize iconSize() const { return QSize(28, 28); }
    QString iconPath(const QString& elemType) const { return ":/elems28/" % elemType; }
    QString drawingPath(const QString& elemType) const { return ":/elems/" % elemType; }

protected:
    ElementImagesProvider() {}

private:
    friend class Singleton<ElementImagesProvider>;
};

#endif // ELEMENT_IMAGES_PROVIDER_H
