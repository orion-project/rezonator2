#ifndef SCHEMA_LAYOUT_CRYSTAL_H
#define SCHEMA_LAYOUT_CRYSTAL_H

#include "SchemaLayout.h"

#include <QtMath>

using namespace ElementLayoutProps;

//------------------------------------------------------------------------------
namespace CrystalElementLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {}

    ELEMENT_LAYOUT_PAINT {
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
}

//------------------------------------------------------------------------------
namespace ElemTiltedCrystalLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CrystalElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 15; HH = 40;
        auto crystal = dynamic_cast<ElemTiltedCrystal*>(_element);
        if (!crystal) return;
        layout.reset(new CrystalElementLayout::Layout(nullptr));
        layout->setSlope(crystal->alpha());
        layout->setHalfSize(HW, HH);
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemTiltedPlateLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = _element->layoutOptions.drawNarrow ? 7 : 15;
        HH = 40;
        auto plate = dynamic_cast<ElemTiltedPlate*>(_element);
        if (!plate) return;
        setSlope(plate->alpha());
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        slopePainter(painter);
        painter->drawRect(boundingRect());
    }
}

//------------------------------------------------------------------------------
namespace ElemBrewsterCrystalLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<CrystalElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = _element->layoutOptions.drawNarrow ? 20 : 30;
        HH = 30;
        layout.reset(new CrystalElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        layout->setSlopeAngle(40);
        layout->setSlope(SlopePlus);
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemBrewsterPlateLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = _element->layoutOptions.drawNarrow ? 7 : 15;
        HH = 40;
        _slopeAngle = 40; _slope = SlopePlus;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->setBrush(getGlassBrush());
        slopePainter(painter);
        painter->drawRect(boundingRect());
    }
}

#endif // SCHEMA_LAYOUT_CRYSTAL_H
