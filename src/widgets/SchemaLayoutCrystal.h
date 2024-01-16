#ifndef SCHEMA_LAYOUT_CRYSTAL_H
#define SCHEMA_LAYOUT_CRYSTAL_H

#include "SchemaLayout.h"
#include "SchemaLayoutDefs.h"
#include "../core/Elements.h"

#include <QtMath>

//------------------------------------------------------------------------------
namespace CrystalElementLayout {
    LAYOUT_BEGIN

    PAINT {
        qreal cline = HH * qTan(qDegreesToRadians(_slopeAngle));

        QPainterPath path;
        switch (_slope)
        {
        case SlopeNone:
            path.addRect(boundingRect());
            break;

        case SlopePlus:
            path.moveTo(-HW + cline, -HH);
            path.lineTo(HW + cline, -HH);
            path.lineTo(HW - cline, HH);
            path.lineTo(-HW - cline, HH);
            path.closeSubpath();
            break;

        case SlopeMinus:
            path.moveTo(-HW - cline, -HH);
            path.lineTo(HW - cline, -HH);
            path.lineTo(HW + cline, HH);
            path.lineTo(-HW + cline, HH);
            path.closeSubpath();
            break;
        }

        painter->setBrush(getGlassBrush());
        painter->setPen(getGlassPen());
        painter->drawPath(path);
    }

    LAYOUT_END
}

//------------------------------------------------------------------------------
namespace ElemTiltedCrystalLayout {
    LAYOUT_BEGIN
        QSharedPointer<CrystalElementLayout::Layout> layout;

    INIT {
        HW = 15; HH = 40;
        auto crystal = dynamic_cast<ElemTiltedCrystal*>(_element);
        if (!crystal) return;
        layout.reset(new CrystalElementLayout::Layout(nullptr));
        layout->setSlope(crystal->alpha());
        layout->setHalfSize(HW, HH);
        layout->init();
    }

    PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }

    OVERRIDE_SELECTED(layout)

    LAYOUT_END
}

//------------------------------------------------------------------------------
namespace ElemTiltedPlateLayout {
    LAYOUT_BEGIN

    INIT {
        HW = _element->layoutOptions.drawAlt ? 7 : 15;
        HH = 40;
        auto plate = dynamic_cast<ElemTiltedPlate*>(_element);
        if (!plate) return;
        setSlope(plate->alpha());
    }

    PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        slopePainter(painter);
        painter->drawRect(boundingRect());
    }

    LAYOUT_END

    OPTIONS_BEGIN
        HAS_ALT_VERSION
    OPTIONS_END
}

//------------------------------------------------------------------------------
namespace ElemBrewsterCrystalLayout {
    LAYOUT_BEGIN
        QSharedPointer<CrystalElementLayout::Layout> layout;

    INIT {
        HW = _element->layoutOptions.drawAlt ? 20 : 30;
        HH = 30;
        layout.reset(new CrystalElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        layout->setSlopeAngle(40);
        layout->setSlope(SlopePlus);
        layout->init();
    }

    PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }

    OVERRIDE_SELECTED(layout)

    LAYOUT_END

    OPTIONS_BEGIN
        HAS_ALT_VERSION
    OPTIONS_END
}

//------------------------------------------------------------------------------
namespace ElemBrewsterPlateLayout {
    LAYOUT_BEGIN

    INIT {
        HW = _element->layoutOptions.drawAlt ? 7 : 15;
        HH = 40;
        _slopeAngle = 40; _slope = SlopePlus;
    }

    PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        slopePainter(painter);
        painter->drawRect(boundingRect());
    }

    LAYOUT_END

    OPTIONS_BEGIN
        HAS_ALT_VERSION
    OPTIONS_END
}

#endif // SCHEMA_LAYOUT_CRYSTAL_H
