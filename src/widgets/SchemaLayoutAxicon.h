#ifndef SCHEMA_LAYOUT_AXICON_H
#define SCHEMA_LAYOUT_AXICON_H

#include "SchemaLayout.h"
#include "SchemaLayoutDefs.h"
#include "../core/Elements.h"

using namespace ElementLayoutProps;

//------------------------------------------------------------------------------
namespace AxiconElementLayout {
    enum AxiconForm {
        FormUnknown,
        ConvexLens,         //      <>
        ConcaveLens,        //      ><
        PlanoConvexMirror,  //      |>
        PlanoConvexLens,    //      |>
        PlanoConcaveMirror, //      |<
        PlanoConcaveLens,   //      |<
        ConvexPlanoMirror,  //      <|
        ConcavePlanoMirror, //      >|
    };

    LAYOUT_BEGIN
        AxiconForm paintMode = FormUnknown;

    PAINT {
        const qreal sagitta = HW * 0.85;

        painter->setPen(getGlassPen());

        switch (paintMode) {
        case FormUnknown:
            break;
        case ConvexLens: // <>
        case ConcaveLens: // )(
        case PlanoConvexLens: // |>
        case PlanoConcaveLens: // |<
            painter->setBrush(getGlassBrush());
            break;
        case PlanoConvexMirror: // |>
        case PlanoConcaveMirror: // |<
        case ConvexPlanoMirror:  // <|
        case ConcavePlanoMirror: // >|
            painter->setBrush(getMirrorBrush());
            break;
        }

        QPainterPath path;

        switch (paintMode) {
        case FormUnknown:
            break;

        case ConvexLens: // <>
            path.moveTo(HW, 0);
            path.lineTo(HW - sagitta, HH);
            path.lineTo(-HW + sagitta, HH);
            path.lineTo(-HW, 0);
            path.lineTo(-HW + sagitta, -HH);
            path.lineTo(HW - sagitta, -HH);
            break;

        case ConcaveLens: // )(
            path.moveTo(HW - sagitta, 0);
            path.lineTo(HW, HH);
            path.lineTo(-HW, HH);
            path.lineTo(-HW + sagitta, 0);
            path.lineTo(-HW, -HH);
            path.lineTo(HW, -HH);
            break;

        case PlanoConvexMirror: // |>
        case PlanoConvexLens: // |>
            path.moveTo(-HW * 0.5, HH);
            path.lineTo(HW - sagitta, HH);
            path.lineTo(HW, 0);
            path.lineTo(HW - sagitta, -HH);
            path.lineTo(-HW * 0.5, -HH);
            break;

        case PlanoConcaveMirror: // |<
        case PlanoConcaveLens: // |<
            path.moveTo(0, 0);
            path.lineTo(HW, HH);
            path.lineTo(-HW * 0.5, HH);
            path.lineTo(-HW * 0.5, -HH);
            path.lineTo(HW, -HH);
            break;

        case ConvexPlanoMirror:  // <|
            path.moveTo(HW * 0.5, HH);
            path.lineTo(-HW + sagitta, HH);
            path.lineTo(-HW, 0);
            path.lineTo(-HW + sagitta, -HH);
            path.lineTo(HW * 0.5, -HH);
            break;

        case ConcavePlanoMirror: // >|
            path.moveTo(HW * 0.5, HH);
            path.lineTo(-HW, HH);
            path.lineTo(0, 0);
            path.lineTo(-HW, -HH);
            path.lineTo(HW * 0.5, -HH);
            break;
        }
        path.closeSubpath();

        slopePainter(painter);
        painter->drawPath(path);

        painter->setPen(getMirrorPen());
        painter->setBrush(Qt::NoBrush);
        QPainterPath path1;
        switch (paintMode) {
        case PlanoConvexMirror: // |>
            path1.moveTo(HW - sagitta, HH);
            path1.lineTo(HW, 0);
            path1.lineTo(HW - sagitta, -HH);
            break;

        case PlanoConcaveMirror: // |<
            path1.moveTo(HW, HH);
            path1.lineTo(HW - sagitta, 0);
            path1.lineTo(HW, -HH);
            break;

        case ConvexPlanoMirror:  // <|
            path1.moveTo(-HW + sagitta, HH);
            path1.lineTo(-HW, 0);
            path1.lineTo(-HW + sagitta, -HH);
            break;

        case ConcavePlanoMirror: // >|
            path1.moveTo(-HW, HH);
            path1.lineTo(-HW + sagitta, 0);
            path1.lineTo(-HW, -HH);
            break;

        default: break;
        }
        painter->drawPath(path1);
    }

    LAYOUT_END
}

//------------------------------------------------------------------------------
namespace ElemAxiconMirrorLayout {
    LAYOUT_BEGIN
        QSharedPointer<AxiconElementLayout::Layout> layout;

    INIT {
        HW = 12; HH = 40;
        auto mirror = dynamic_cast<ElemAxiconMirror*>(_element);
        if (!mirror || !_element->owner()) return;
        layout.reset(new AxiconElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        layout->setSlope(mirror->alpha());
        auto pos = _element->owner()->position(_element);
        if (pos == ElementOwner::PositionAtLeft)
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::PlanoConcaveMirror
                : AxiconElementLayout::PlanoConvexMirror;
        else if (pos == ElementOwner::PositionAtRight)
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::ConcavePlanoMirror
                : AxiconElementLayout::ConvexPlanoMirror;
        else
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::ConvexLens
                : AxiconElementLayout::ConcaveLens;
        layout->init();
    }

    PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }

    LAYOUT_END
}

//------------------------------------------------------------------------------
namespace ElemAxiconLensLayout {
    LAYOUT_BEGIN
        QSharedPointer<AxiconElementLayout::Layout> layout;

    INIT {
        HW = 12; HH = 40;
        auto mirror = dynamic_cast<ElemAxiconLens*>(_element);
        if (!mirror || !_element->owner()) return;
        layout.reset(new AxiconElementLayout::Layout(nullptr));
        layout->setHalfSize(HW, HH);
        layout->setSlope(mirror->alpha());
        auto pos = _element->owner()->position(_element);
        if (pos == ElementOwner::PositionAtLeft)
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::PlanoConcaveMirror
                : AxiconElementLayout::PlanoConvexMirror;
        else if (pos == ElementOwner::PositionAtRight)
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::ConcavePlanoMirror
                : AxiconElementLayout::ConvexPlanoMirror;
        else
            layout->paintMode = mirror->theta() > 0
                ? AxiconElementLayout::PlanoConvexLens
                : AxiconElementLayout::PlanoConcaveLens;
        layout->init();
    }

    PAINT {
        if (layout) layout->paint(painter, nullptr, nullptr);
    }

    LAYOUT_END
}

#endif // SCHEMA_LAYOUT_AXICON_H
