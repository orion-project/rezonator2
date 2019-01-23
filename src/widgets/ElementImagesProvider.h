#ifndef ELEMENT_IMAGES_PROVIDER_H
#define ELEMENT_IMAGES_PROVIDER_H

#include "../core/Element.h"
#include "core/OriTemplates.h"

#include <QSize>

class Element;

// TODO remove this class to make things simpler, use utils functions in Element.h
class ElementImagesProvider: public Singleton<ElementImagesProvider>
{
public:
    QSize iconSize() const { return QSize(24, 24); }
    QString iconPath(const QString& elemType) const { return ":/elem_icon/" % elemType; }
    QString drawingPath(const QString& elemType) const { return ":/elem_drawing/" % elemType; }

protected:
    ElementImagesProvider() {}

private:
    friend class Singleton<ElementImagesProvider>;
};

#endif // ELEMENT_IMAGES_PROVIDER_H
