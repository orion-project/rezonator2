#ifndef SCHEMA_LAYOUT_ELEMS_H
#define SCHEMA_LAYOUT_ELEMS_H

#include "SchemaLayout.h"
#include "SchemaLayoutDefs.h"

//------------------------------------------------------------------------------
namespace ElemEmptyRangeLayout {
    LAYOUT_BEGIN

    INIT {
        HW = _element->layoutOptions.drawAlt ? 15 : 50;
        HH = 5;
    }

    PAINT {
        Q_UNUSED(painter)
    }

    LAYOUT_END

    OPTIONS_BEGIN
        HAS_ALT_VERSION
    OPTIONS_END
}

//------------------------------------------------------------------------------
namespace ElemMediumRangeLayout {
    LAYOUT_BEGIN

    INIT {
        HW = _element->layoutOptions.drawAlt ? 15 : 30;
        HH = 40;
    }

    PAINT {
        painter->fillRect(boundingRect(), getGlassBrush());
        painter->setPen(getGlassPen());
        painter->drawLine(QLineF(-HW, -HH, HW, -HH));
        painter->drawLine(QLineF(-HW, HH, HW, HH));
    }

    LAYOUT_END

    OPTIONS_BEGIN
        HAS_ALT_VERSION
    OPTIONS_END
}

//------------------------------------------------------------------------------
namespace ElemPlateLayout {
    LAYOUT_BEGIN

    INIT {
        HW = _element->layoutOptions.drawAlt ? 15 : 25;
        HH = 40;
    }

    PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        painter->drawRect(boundingRect());
    }
    LAYOUT_END

    OPTIONS_BEGIN
        HAS_ALT_VERSION
    OPTIONS_END
}

//------------------------------------------------------------------------------
namespace ElemFlatMirrorLayout {
    LAYOUT_BEGIN
        enum {
            PlaceMiddle,
            PlaceLeft,
            PlaceRight
        } place = PlaceMiddle;

    INIT {
        HW = 7; HH = 40;
        if (!_element->owner()) return;
        auto pos = _element->owner()->position(_element);
        if (pos == ElementOwner::PositionAtLeft)
            place = PlaceLeft;
        else if (pos == ElementOwner::PositionAtRight)
            place = PlaceRight;
    }

    PAINT {
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

    LAYOUT_END
}

//------------------------------------------------------------------------------
namespace ElemMatrixLayout {
    LAYOUT_BEGIN

    INIT {
        HH = 15; HW = 15;
    }

    PAINT {
        painter->setPen(getGlassPen());
        painter->fillRect(boundingRect(), getMirrorBrush());
        painter->drawRect(QRectF(-HW, -HH, 2*HW, 2*HH));
        painter->drawLine(QLineF(0, -HH, 0, HH));
        painter->drawLine(QLineF(-HW, 0, HW, 0));
    }

    LAYOUT_END
}

//------------------------------------------------------------------------------
namespace ElemPointLayout {
    LAYOUT_BEGIN

    INIT {
        HW = 5;
        HH = _element->layoutOptions.drawAlt ? 40 : 5;
    }

    PAINT {
        if (_element->layoutOptions.drawAlt) {
            painter->setPen(getPlanePen());
            painter->drawLine(QLineF(0, -HH, 0, HH));
        } else {
            painter->setPen(getGlassPen());
            painter->setBrush(Qt::black);
            painter->drawEllipse(-3, -3, 6, 6);
        }
    }

    LAYOUT_END

    OPTIONS_BEGIN
        HAS_ALT_VERSION
        ALT_VERSION_OPTION_TITLE(QT_TRANSLATE_NOOP("LayoutOptions", "Draw as plane"))
    OPTIONS_END
}

//------------------------------------------------------------------------------
namespace ElemGrinLensLayout {
    LAYOUT_BEGIN

    INIT {
        HW = _element->layoutOptions.drawAlt ? 15 : 25;
        HH = 40;
    }

    PAINT {
        QPainterPath path;
        path.addRect(boundingRect());
        painter->setBrush(getGrinBrush(HH));
        painter->setPen(getGlassPen());
        painter->drawPath(path);
    }

    LAYOUT_END

    OPTIONS_BEGIN
        HAS_ALT_VERSION
    OPTIONS_END
}

//------------------------------------------------------------------------------
namespace ElemGrinMediumLayout {
    LAYOUT_BEGIN

    INIT {
        HW = _element->layoutOptions.drawAlt ? 15 : 30;
        HH = 40;
    }

    PAINT {
        painter->fillRect(boundingRect(), getGrinBrush(HH));
        painter->setPen(getGlassPen());
        painter->drawLine(QLineF(-HW, -HH, HW, -HH));
        painter->drawLine(QLineF(-HW, HH, HW, HH));
    }

    LAYOUT_END

    OPTIONS_BEGIN
        HAS_ALT_VERSION
    OPTIONS_END
}

//------------------------------------------------------------------------------
namespace ElemGaussApertureLayout {
    LAYOUT_BEGIN

    INIT {
        HW = 5;
        HH = 40;
    }

    PAINT {
        QPainterPath path;
        path.addRect(boundingRect());
        painter->setBrush(getGrinBrush(HH));
        painter->setPen(getGlassPen());
        painter->drawPath(path);
    }

    LAYOUT_END
}

#endif // SCHEMA_LAYOUT_ELEMS_H
