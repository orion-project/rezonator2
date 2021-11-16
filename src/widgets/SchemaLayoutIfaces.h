#ifndef SCHEMA_LAYOUT_IFACES_H
#define SCHEMA_LAYOUT_IFACES_H

#include "SchemaLayout.h"

#include <QtMath>

using namespace ElementLayoutProps;

//------------------------------------------------------------------------------
namespace IfaceLayoutHepers {

struct IfacePosition {
    enum {
        BetweenRanges,
        BetweenMedia,
        BeforeMedium,
        AfterMedium,
    } placement;

    enum {
        OptionNone = 0x00,
        OptionGrin = 0x01,
    } options;
};

enum class IfaceNeighbour {
    Unknown,
    Air,
    Medium,
    Grin,
};

IfaceNeighbour getNeighbourKind(Schema* schema, Element* elem) {
    if (elem) {
        if (dynamic_cast<ElemGrinMedium*>(elem))
            return IfaceNeighbour::Grin;

        if (dynamic_cast<ElemThermoMedium*>(elem))
            return IfaceNeighbour::Grin;

        if (dynamic_cast<ElemMediumRange*>(elem))
            return IfaceNeighbour::Medium;

        if (dynamic_cast<ElemEmptyRange*>(elem))
            return IfaceNeighbour::Air;
    }
    else if (schema->isSP())
        return IfaceNeighbour::Air;
    return IfaceNeighbour::Unknown;
}

// Check on these files how interfaces are drawn with combination with different media:
// $$_PRO_FILE_PWD_/bin/test_files/draw_interfaces_grin.rez
// $$_PRO_FILE_PWD_/bin/test_files/draw_interfaces_media.rez
// $$_PRO_FILE_PWD_/bin/test_files/draw_interfaces_grin_media.rez
IfacePosition getIfacePosition(Element* elem) {
    auto schema = dynamic_cast<Schema*>(elem->owner());
    if (!schema) return {IfacePosition::BetweenMedia, IfacePosition::OptionNone};

    auto left = getNeighbourKind(schema, schema->leftElement(elem));
    auto right = getNeighbourKind(schema, schema->rightElement(elem));

    if (left == IfaceNeighbour::Air && right == IfaceNeighbour::Air)
        return {IfacePosition::BetweenRanges, IfacePosition::OptionNone};

    if (left == IfaceNeighbour::Medium && right == IfaceNeighbour::Medium)
        return {IfacePosition::BetweenMedia, IfacePosition::OptionNone};

    if (left == IfaceNeighbour::Grin && right == IfaceNeighbour::Grin)
        return {IfacePosition::BetweenMedia, IfacePosition::OptionGrin};

    if (left == IfaceNeighbour::Air && right == IfaceNeighbour::Medium)
        return {IfacePosition::BeforeMedium, IfacePosition::OptionNone};

    if (left == IfaceNeighbour::Air && right == IfaceNeighbour::Grin)
        return {IfacePosition::BeforeMedium, IfacePosition::OptionGrin};

    if (left == IfaceNeighbour::Medium && right == IfaceNeighbour::Air)
        return {IfacePosition::AfterMedium, IfacePosition::OptionNone};

    if (left == IfaceNeighbour::Grin && right == IfaceNeighbour::Air)
        return {IfacePosition::AfterMedium, IfacePosition::OptionGrin};

    return {IfacePosition::BetweenMedia, IfacePosition::OptionNone};
}

} // namespace IfaceLayoutHepers
using namespace IfaceLayoutHepers;

//------------------------------------------------------------------------------
// Base layout for ElemBrewsterInterfaceLayout and ElemTiltedInterfaceLayout interface.
// Normal interface is painted by separate and simpler ElemNormalInterfaceLayout.
namespace PlaneInterfaceElementLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        IfacePosition placement;
        QBrush getBrush() const;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        placement = getIfacePosition(_element);
    }

    QBrush Layout::getBrush() const {
        if (placement.options & IfacePosition::OptionGrin)
            return getGrinBrush(HH);
        return getGlassBrush();
    }

    ELEMENT_LAYOUT_PAINT {
        switch (placement.placement) {
        case IfacePosition::BetweenRanges:
            painter->setPen(getPlanePen());
            painter->drawLine(QLineF(HW, -HH, -HW, HH));
            break;

        case IfacePosition::BetweenMedia:
            painter->fillRect(boundingRect(), getBrush());
            painter->setPen(getGlassPen());
            painter->drawLine(QLineF(-HW, -HH, HW, -HH));
            painter->drawLine(QLineF(HW, -HH, -HW, HH));
            painter->drawLine(QLineF(-HW, HH, HW, HH));
            break;

        case IfacePosition::AfterMedium: {
                QPainterPath path;
                path.moveTo(-HW, -HH);
                path.lineTo(HW, -HH);
                path.lineTo(-HW, HH);
                path.closeSubpath();
                painter->fillPath(path, getBrush());

                painter->setPen(getGlassPen());
                painter->drawLine(QLineF(-HW, -HH, HW, -HH));
                painter->drawLine(QLineF(HW, -HH, -HW, HH));
            }
            break;

        case IfacePosition::BeforeMedium: {
                QPainterPath path;
                path.moveTo(HW, -HH);
                path.lineTo(-HW, HH);
                path.lineTo(HW, HH);
                path.closeSubpath();
                painter->fillPath(path, getBrush());

                painter->setPen(getGlassPen());
                painter->drawLine(QLineF(HW, -HH, -HW, HH));
                painter->drawLine(QLineF(-HW, HH, HW, HH));
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
namespace ElemBrewsterInterfaceLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<PlaneInterfaceElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 30; HH = 40;
        layout.reset(new PlaneInterfaceElementLayout::Layout(_element));
        layout->setHalfSize(HW, HH);
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemTiltedInterfaceLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
        QSharedPointer<PlaneInterfaceElementLayout::Layout> layout;
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 15; HH = 40;
        layout.reset(new PlaneInterfaceElementLayout::Layout(_element));
        layout->setHalfSize(HW, HH);
        layout->init();
    }

    ELEMENT_LAYOUT_PAINT {
        layout->paint(painter, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
namespace ElemSphericalInterfaceLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    private:
        IfacePosition _placement;
        QRectF _surface;
        qreal _startAngle;
        qreal _sweepAngle;
        QBrush getBrush() const;
    DECLARE_ELEMENT_LAYOUT_END

    QBrush Layout::getBrush() const {
        if (_placement.options & IfacePosition::OptionGrin)
            return getGrinBrush(HH);
        return getGlassBrush();
    }

    ELEMENT_LAYOUT_INIT {
        HW = 15; HH = 40;
        _placement = getIfacePosition(_element);
        auto intf = dynamic_cast<ElemSphericalInterface*>(_element);
        const qreal ROC = 100;
        const qreal sagitta = ROC - qSqrt(Sqr(ROC) - Sqr(HH));
        const qreal startAngle = qRadiansToDegrees(qAsin(HH / ROC));
        if (intf && intf->radius() < 0) {
            _surface = QRectF(-sagitta, -ROC, 2*ROC, 2*ROC);
            _startAngle = 180 - startAngle;
            _sweepAngle = 2*startAngle;
        }
        else {
            _surface = QRectF(sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            _startAngle = startAngle;
            _sweepAngle = -2*startAngle;
        }
    }

    ELEMENT_LAYOUT_PAINT {
        switch (_placement.placement) {
        case IfacePosition::BetweenRanges:
            painter->setPen(getPlanePen());
            painter->drawArc(_surface, int(_startAngle)*16, int(_sweepAngle)*16);
            break;

        case IfacePosition::BetweenMedia:
            painter->fillRect(boundingRect(), getBrush());
            painter->setPen(getGlassPen());
            painter->drawLine(QLineF(-HW, -HH, HW, -HH));
            painter->drawLine(QLineF(-HW, HH, HW, HH));
            painter->drawArc(_surface, int(_startAngle)*16, int(_sweepAngle)*16);
            break;

        case IfacePosition::AfterMedium: {
                QPainterPath path;
                path.moveTo(-HW, -HH);
                path.lineTo(0, -HH);
                path.arcTo(_surface, _startAngle, _sweepAngle);
                path.lineTo(-HW, HH);
                path.closeSubpath();
                painter->fillPath(path, getBrush());

                painter->setPen(getGlassPen());
                QPainterPath path1;
                path1.moveTo(-HW, -HH);
                path1.lineTo(0, -HH);
                path1.arcTo(_surface, _startAngle, _sweepAngle);
                path1.lineTo(-HW, HH);
                painter->drawPath(path1);
            }
            break;

        case IfacePosition::BeforeMedium: {
                QPainterPath path;
                path.moveTo(HW, -HH);
                path.lineTo(0, -HH);
                path.arcTo(_surface, _startAngle, _sweepAngle);
                path.lineTo(HW, HH);
                path.closeSubpath();
                painter->fillPath(path, getBrush());

                painter->setPen(getGlassPen());
                QPainterPath path1;
                path1.moveTo(HW, -HH);
                path1.lineTo(0, -HH);
                path1.arcTo(_surface, _startAngle, _sweepAngle);
                path1.lineTo(HW, HH);
                painter->drawPath(path1);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
namespace ElemNormalInterfaceLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
        HW = 1.5; HH = 40;
    }

    ELEMENT_LAYOUT_PAINT {
        painter->setPen(getGlassPen());
        painter->drawLine(QLineF(0, -HH, 0, HH));
    }
}

#endif // SCHEMA_LAYOUT_IFACES_H
