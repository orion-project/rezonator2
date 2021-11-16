#ifndef SCHEMA_LAYOUT_ELEMS_H
#define SCHEMA_LAYOUT_ELEMS_H

#include "SchemaLayout.h"

using namespace ElementLayoutProps;

//------------------------------------------------------------------------------
namespace ElemEmptyRangeLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = _element->layoutOptions.drawNarrow ? 15 : 50;
        HH = 5;
    }

    ELEMENT_LAYOUT_PAINT {
        Q_UNUSED(painter)
    }
}

//------------------------------------------------------------------------------
namespace ElemMediumRangeLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = _element->layoutOptions.drawNarrow ? 15 : 30;
        HH = 40;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->fillRect(boundingRect(), getGlassBrush());
        painter->setPen(getGlassPen());
        painter->drawLine(QLineF(-HW, -HH, HW, -HH));
        painter->drawLine(QLineF(-HW, HH, HW, HH));
    }
}

//------------------------------------------------------------------------------
namespace ElemPlateLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = _element->layoutOptions.drawNarrow ? 15 : 25;
        HH = 40;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        painter->drawRect(boundingRect());
    }
}

//------------------------------------------------------------------------------
namespace ElemFlatMirrorLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        enum {
            PlaceMiddle,
            PlaceLeft,
            PlaceRight
        } place = PlaceMiddle;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 7; HH = 40;
        if (!_element->owner()) return;
        auto pos = _element->owner()->position(_element);
        if (pos == ElementOwner::PositionAtLeft)
            place = PlaceLeft;
        else if (pos == ElementOwner::PositionAtRight)
            place = PlaceRight;
    }

    ELEMENT_LAYOUT_PAINT {
        qreal x = 0;
        switch (place) {
        case PlaceLeft:
        case PlaceRight:
            painter->setBrush(getMirrorBrush());
            painter->setPen(getGlassPen());
            painter->drawRect(boundingRect());
            painter->setPen(getMirrorPen());
            x = (place == PlaceLeft)? HW: -HW;
            break;

        case PlaceMiddle:
            painter->setPen(getPlanePen());
            break;
        }
        painter->drawLine(QLineF(x, -HH, x, HH));
    }
}

//------------------------------------------------------------------------------
namespace ElemMatrixLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HH = 15; HW = 15;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->fillRect(boundingRect(), getMirrorBrush());
        painter->drawRect(QRectF(-HW, -HH, 2*HW, 2*HH));
        painter->drawLine(QLineF(0, -HH, 0, HH));
        painter->drawLine(QLineF(-HW, 0, HW, 0));
    }
}

//------------------------------------------------------------------------------
namespace ElemPointLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 5; HH = 5;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(Qt::black);
        painter->drawEllipse(-3, -3, 6, 6);
    }
}

//------------------------------------------------------------------------------
namespace ElemGrinLensLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = _element->layoutOptions.drawNarrow ? 15 : 25;
        HH = 40;
    }

    ELEMENT_LAYOUT_PAINT {
        QPainterPath path;
        path.addRect(boundingRect());
        painter->setBrush(getGrinBrush(HH));
        painter->setPen(getGlassPen());
        painter->drawPath(path);
    }
}

//------------------------------------------------------------------------------
namespace ElemGrinMediumLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = _element->layoutOptions.drawNarrow ? 15 : 30;
        HH = 40;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->fillRect(boundingRect(), getGrinBrush(HH));
        painter->setPen(getGlassPen());
        painter->drawLine(QLineF(-HW, -HH, HW, -HH));
        painter->drawLine(QLineF(-HW, HH, HW, HH));
    }
}

//------------------------------------------------------------------------------
namespace ElemGaussApertureLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 5;
        HH = 40;
    }

    ELEMENT_LAYOUT_PAINT {
        QPainterPath path;
        path.addRect(boundingRect());
        painter->setBrush(getGrinBrush(HH));
        painter->setPen(getGlassPen());
        painter->drawPath(path);
    }
}

#endif // SCHEMA_LAYOUT_ELEMS_H
