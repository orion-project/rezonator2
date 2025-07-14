#ifndef SCHEMA_LAYOUT_LENS_H
#define SCHEMA_LAYOUT_LENS_H

#include "SchemaLayout.h"
#include "SchemaLayoutDefs.h"
#include "../core/Elements.h"

#include <QtMath>

//------------------------------------------------------------------------------
namespace CurvedElementLayout {
    enum CurvedForm {
        FormUnknown,
        Plate,              //      ||
        ConvexLens,         //      ()
        ConcaveLens,        //      )(
        ConvexConcaveLens,  //      ((
        ConcaveConvexLens,  //      ))
        PlanoConvexMirror,  //      |)
        PlanoConvexLens,    //      |)
        PlanoConcaveMirror, //      |(
        PlanoConcaveLens,   //      |(
        ConvexPlanoMirror,  //      (|
        ConvexPlanoLens,    //      (|
        ConcavePlanoMirror, //      )|
        ConcavePlanoLens    //      )|
    };

    LAYOUT_BEGIN
        QBrush brush;
        CurvedForm paintMode = FormUnknown;

    PAINT {
        painter->setBrush(brush.style() == Qt::NoBrush ? getGlassBrush() : brush);
        painter->setPen(getGlassPen());

        const qreal ROC = 100;
        const qreal sagitta = ROC - qSqrt(Sqr(ROC) - Sqr(HH));
        const qreal startAngle = qRadiansToDegrees(qAsin(HH / ROC));
        const qreal sweepAngle = 2*startAngle;

        QPainterPath path;
        QRectF rightSurface;
        QRectF leftSurface;

        switch (paintMode) {
        case FormUnknown:
            break;
            
        case Plate:
        path.addRect(boundingRect());
            break;

        case ConvexLens: // ()
            rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
            leftSurface = QRectF(-HW, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW - sagitta, HH);
            path.arcTo(rightSurface, 360-startAngle, sweepAngle);
            path.lineTo(-HW + sagitta, -HH);
            path.arcTo(leftSurface, 180-startAngle, sweepAngle);
            break;

        case ConcaveLens: // )(
            rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
            leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW, -HH);
            path.arcTo(rightSurface, 180-startAngle, sweepAngle);
            path.lineTo(-HW, HH);
            path.arcTo(leftSurface, 360-startAngle, sweepAngle);
            break;

        case ConvexConcaveLens: // ((
            leftSurface = QRectF(-HW, -ROC, 2*ROC, 2*ROC);
            rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW + sagitta, -HH);
            path.arcTo(leftSurface, 180-startAngle, sweepAngle);
            path.lineTo(HW, HH);
            path.arcTo(rightSurface, 180+startAngle, -sweepAngle);
            break;

        case ConcaveConvexLens: // ))
            leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW, -HH);
            path.arcTo(leftSurface, startAngle, -sweepAngle);
            path.lineTo(HW - sagitta, HH);
            path.arcTo(rightSurface, -startAngle, sweepAngle);
            break;

        case PlanoConvexMirror:
            painter->setBrush(getMirrorBrush());
            rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW - sagitta, HH);
            path.arcTo(rightSurface, 360-startAngle, sweepAngle);
            path.lineTo(-HW, -HH);
            path.lineTo(-HW, HH);
            break;

        case PlanoConvexLens:
            rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW - sagitta, HH);
            path.arcTo(rightSurface, 360-startAngle, sweepAngle);
            path.lineTo(-HW, -HH);
            path.lineTo(-HW, HH);
            break;

        case PlanoConcaveMirror:
            painter->setBrush(getMirrorBrush());
            rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW, -HH);
            path.arcTo(rightSurface, 180-startAngle, sweepAngle);
            path.lineTo(-HW, HH);
            path.lineTo(-HW, -HH);
            break;

        case PlanoConcaveLens:
            rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
            path.moveTo(HW, -HH);
            path.arcTo(rightSurface, 180-startAngle, sweepAngle);
            path.lineTo(-HW, HH);
            path.lineTo(-HW, -HH);
            break;

        case ConcavePlanoMirror:
            painter->setBrush(getMirrorBrush());
            leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW, HH);
            path.arcTo(leftSurface, 360-startAngle, sweepAngle);
            path.lineTo(HW, -HH);
            path.lineTo(HW, HH);
            break;

        case ConcavePlanoLens:
            leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW, HH);
            path.arcTo(leftSurface, 360-startAngle, sweepAngle);
            path.lineTo(HW, -HH);
            path.lineTo(HW, HH);
            break;

        case ConvexPlanoMirror:
            painter->setBrush(getMirrorBrush());
            leftSurface = QRectF(-HW, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW + sagitta, -HH);
            path.arcTo(leftSurface, 180-startAngle, sweepAngle);
            path.lineTo(HW, HH);
            path.lineTo(HW, -HH);
            break;

        case ConvexPlanoLens:
            leftSurface = QRectF(-HW, -ROC, 2*ROC, 2*ROC);
            path.moveTo(-HW + sagitta, -HH);
            path.arcTo(leftSurface, 180-startAngle, sweepAngle);
            path.lineTo(HW, HH);
            path.lineTo(HW, -HH);
            break;
        }
        path.closeSubpath();

        slopePainter(painter);
        painter->drawPath(path);

        switch (paintMode) {
        case PlanoConvexMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(rightSurface, int(16*(360-startAngle)), int(16*sweepAngle));
            break;

        case PlanoConcaveMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(rightSurface, int(16*(180-startAngle)), int(16*sweepAngle));
            break;

        case ConcavePlanoMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(leftSurface, int(16*(360-startAngle)), int(16*sweepAngle));
            break;

        case ConvexPlanoMirror:
            painter->setPen(getMirrorPen());
            painter->drawArc(leftSurface, int(16*(180-startAngle)), int(16*sweepAngle));
            break;

        default: break;
        }
    }

    void paintCornerMark(QPainter *painter, const QString& mark) const {
        auto p = boundingRect().bottomRight();
        p.setX(p.x()-qreal(HW)/2.0);
        painter->setFont(getMarkTSFont());
        painter->drawText(p, mark);
    }

    LAYOUT_END
}

//------------------------------------------------------------------------------
namespace ElemCurveMirrorLayout {
    LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;

    INIT {
        HW = 10; HH = 40;
        ElemCurveMirror *mirror = dynamic_cast<ElemCurveMirror*>(_element);
        if (!mirror || !_element->owner()) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr, _owner));
        layout->setHalfSize(HW, HH);
        layout->setSlope(mirror->alpha());
        auto pos = _element->owner()->position(_element);
        if (pos == ElementOwner::PositionAtLeft)
            layout->paintMode = mirror->radius() > 0
                ? CurvedElementLayout::PlanoConcaveMirror
                : CurvedElementLayout::PlanoConvexMirror;
        else if (pos == ElementOwner::PositionAtRight)
            layout->paintMode = mirror->radius() > 0
                ? CurvedElementLayout::ConcavePlanoMirror
                : CurvedElementLayout::ConvexPlanoMirror;
        else
            layout->paintMode = mirror->radius() > 0
                ? CurvedElementLayout::ConvexLens
                : CurvedElementLayout::ConcaveLens;
        layout->init();
    }

    PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }

    OVERRIDE_SELECTED(layout)

    LAYOUT_END
}

//------------------------------------------------------------------------------
namespace ElemThinLensLayout {
    LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;


    INIT {
        HW = 10; HH = 40;
        auto lens = dynamic_cast<ElemThinLens*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr, _owner));
        layout->setHalfSize(HW, HH);
        layout->setSlope(lens->alpha());
        layout->paintMode = lens->focus() > 0
                       ? CurvedElementLayout::ConvexLens
                       : CurvedElementLayout::ConcaveLens;
        layout->init();
    }

    PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }

    OVERRIDE_SELECTED(layout)

    LAYOUT_END
}

//------------------------------------------------------------------------------
namespace ElemCylinderLensTLayout {
    LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;

    INIT {
        HW = 10; HH = 40;
        auto lens = dynamic_cast<ElemCylinderLensT*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr, _owner));
        layout->setHalfSize(HW, HH);
        layout->setSlope(lens->alpha());
        layout->paintMode = lens->focus() > 0
                           ? CurvedElementLayout::ConvexLens
                           : CurvedElementLayout::ConcaveLens;
        layout->init();
    }

    PAINT {
        if (layout) {
            layout->paint(painter, nullptr, nullptr);
            layout->paintCornerMark(painter, QStringLiteral("T"));
        }
    }

    OVERRIDE_SELECTED(layout)

    LAYOUT_END
}

//------------------------------------------------------------------------------
namespace ElemCylinderLensSLayout {
    LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;

    INIT {
        HW = 10; HH = 40;
        auto lens = dynamic_cast<ElemCylinderLensS*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr, _owner));
        layout->setHalfSize(HW, HH);
        layout->setSlope(lens->alpha());
        layout->paintMode = lens->focus() > 0
                ? CurvedElementLayout::ConvexLens
                : CurvedElementLayout::ConcaveLens;
        layout->init();
    }

    PAINT {
        if (layout) {
            layout->paint(painter, nullptr, nullptr);
            layout->paintCornerMark(painter, QStringLiteral("S"));
        }
    }

    OVERRIDE_SELECTED(layout)

    LAYOUT_END
}

//------------------------------------------------------------------------------
namespace ElemThickLensLayout {
    LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;

    INIT {
        HW = _element->layoutOptions.drawAlt ? 10 : 25;
        HH = 40;
        auto lens = dynamic_cast<ElemThickLens*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr, _owner));
        layout->setHalfSize(HW, HH);
        const double R1 = -lens->radius1();
        const double R2 = -lens->radius2();
        const bool flat1 = qIsInf(R1);
        const bool flat2 = qIsInf(R2);
        if (flat1 && flat2)
            layout->paintMode = CurvedElementLayout::Plate;
        else if (flat1 && R2 > 0) // |)
            layout->paintMode = CurvedElementLayout::PlanoConvexLens;
        else if (flat1 && R2 < 0) // |(
            layout->paintMode = CurvedElementLayout::PlanoConcaveLens;
        else if (flat2 && R1 > 0) // )|
            layout->paintMode = CurvedElementLayout::ConcavePlanoLens;
        else if (flat2 && R1 < 0) // (|
            layout->paintMode = CurvedElementLayout::ConvexPlanoLens;
        else if (R1 < 0 && R2 > 0) // ()
            layout->paintMode = CurvedElementLayout::ConvexLens;
        else if (R1 > 0 && R2 < 0) // )(
            layout->paintMode = CurvedElementLayout::ConcaveLens;
        else if (R1 < 0 && R2 < 0) // ((
            layout->paintMode = CurvedElementLayout::ConvexConcaveLens;
        else if (R1 > 0 && R2 > 0) // ))
            layout->paintMode = CurvedElementLayout::ConcaveConvexLens;
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
namespace ElemGaussApertureLensLayout {
    LAYOUT_BEGIN
        QSharedPointer<CurvedElementLayout::Layout> layout;

    INIT {
        HW = 10; HH = 40;
        auto lens = dynamic_cast<ElemGaussApertureLens*>(_element);
        if (!lens) return;
        layout.reset(new CurvedElementLayout::Layout(nullptr, _owner));
        layout->setHalfSize(HW, HH);
        layout->brush = getGrinBrush(HH);
        layout->paintMode = lens->focusT() > 0
                       ? CurvedElementLayout::PlanoConvexLens // |)
                       : CurvedElementLayout::PlanoConcaveLens; // |(
        layout->init();
    }

    PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }

    OVERRIDE_SELECTED(layout)

    LAYOUT_END
}

#endif // SCHEMA_LAYOUT_LENS_H
