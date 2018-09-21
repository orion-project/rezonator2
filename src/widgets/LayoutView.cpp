#include "LayoutView.h"

#include "Appearance.h"

#include <QtMath>
#include <QDebug>

#define RadToDeg(angle) (angle * 180.0 / M_PI)
#define DegToRad(angle) (angle / 180.0 * M_PI)
#define Sqr(x) (x*x)

//------------------------------------------------------------------------------

namespace {

QBrush getGlassBrush()
{
    static QBrush b = QBrush(QPixmap(":/misc32/glass_pattern"));
    return b;
}

QPen getGlassPen()
{
    static QPen p = QPen(Qt::black, 1.5);
    return p;
}

QBrush getMirrorBrush()
{
    static QBrush b = QBrush(Qt::black, Qt::BDiagPattern);
    return b;
}

QPen getMirrorPen()
{
    static QPen p = QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap);
    return p;
}

QPen getPlanePen()
{
    static QPen p = QPen(Qt::black, 1, Qt::DashLine);
    return p;
}

const QFont& getMarkTSFont()
{
    static QFont f = QFont("Arial", 8, QFont::Bold);
    return f;
}

} // namespace

//------------------------------------------------------------------------------
//                             ElementView
//------------------------------------------------------------------------------

void ElementView::slopePainter(QPainter *painter)
{
    if (_slope != SlopeNone)
    {
        qreal angle = (_slope == SlopePlus)? _slopeAngle: -_slopeAngle;
        QTransform t = painter->transform();
        t.rotate(angle);
        painter->setTransform(t);

        t.rotate(-2*angle);
        QBrush b = painter->brush();
        b.setTransform(t);
        painter->setBrush(b);
    }
}

//------------------------------------------------------------------------------
//                             ElementRangeView
//------------------------------------------------------------------------------

void ElementRangeView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    Q_UNUSED(painter)

    //painter->setBrush(QColor(127, 127, 127, 50));
    //painter->drawRect(boundingRect());
}


//------------------------------------------------------------------------------
//                               ElemPlateView
//------------------------------------------------------------------------------

void ElemPlateView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(getGlassPen());
    painter->setBrush(getGlassBrush());

    slopePainter(painter);

    painter->drawRect(boundingRect());
}


//------------------------------------------------------------------------------
//                           ElemTiltedCrystalView
//------------------------------------------------------------------------------

void ElemTiltedCrystalView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    qreal cline = HH * qTan(DegToRad(_slopeAngle));

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


//------------------------------------------------------------------------------
//                             ElemPointView
//------------------------------------------------------------------------------

void ElemPointView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(getGlassPen());
    painter->setBrush(Qt::black);
    painter->drawEllipse(boundingRect());
}

//------------------------------------------------------------------------------
//                           ElemMatrixView
//------------------------------------------------------------------------------

void ElemMatrixView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(getGlassPen());
    painter->fillRect(boundingRect(), getMirrorBrush());
    painter->drawRect(QRectF(-HW, -HH, 2*HW, 2*HH));
    painter->drawLine(QLineF(0, -HH, 0, HH));
    painter->drawLine(QLineF(-HW, 0, HW, 0));
}

//------------------------------------------------------------------------------
//                           ElemMediumRangeView
//------------------------------------------------------------------------------

void ElemMediumRangeView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->fillRect(boundingRect(), getGlassBrush());
    painter->setPen(getGlassPen());
    painter->drawLine(QLineF(-HW, -HH, HW, -HH));
    painter->drawLine(QLineF(-HW, HH, HW, HH));
}


//------------------------------------------------------------------------------
//                             ElemFlatMirrorView
//------------------------------------------------------------------------------

void ElemFlatMirrorView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    qreal x = 0;

    switch (_place)
    {
    case PlaceLeft:
    case PlaceRight:
        painter->setBrush(getMirrorBrush());
        painter->setPen(getGlassPen());
        painter->drawRect(boundingRect());
        painter->setPen(getMirrorPen());
        x = (_place == PlaceLeft)? HW: -HW;
        break;

    case PlaceMiddle:
        painter->setPen(getPlanePen());
        break;
    }
    painter->drawLine(QLineF(x, -HH, x, HH));
}


//------------------------------------------------------------------------------
//                             ElemCurveMirrorView
//------------------------------------------------------------------------------

void CurvedElementView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setBrush(getGlassBrush());
    painter->setPen(getGlassPen());

    qreal sagitta = ROC - qSqrt(Sqr(ROC) - Sqr(HH));
    qreal startAngle = RadToDeg(qAsin(HH / ROC));
    qreal sweepAngle = 2*startAngle;

    QPainterPath path;
    QRectF rightSurface;
    QRectF leftSurface;

    switch (_paintMode)
    {
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

    case PlaneConvexMirror:
        painter->setBrush(getMirrorBrush());
        rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
        path.moveTo(HW - sagitta, HH);
        path.arcTo(rightSurface, 360-startAngle, sweepAngle);
        path.lineTo(-HW, -HH);
        path.lineTo(-HW, HH);
        break;

    case PlaneConvexLens:
        rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
        path.moveTo(HW - sagitta, HH);
        path.arcTo(rightSurface, 360-startAngle, sweepAngle);
        path.lineTo(-HW, -HH);
        path.lineTo(-HW, HH);
        break;

    case PlaneConcaveMirror:
        painter->setBrush(getMirrorBrush());
        rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
        path.moveTo(HW, -HH);
        path.arcTo(rightSurface, 180-startAngle, sweepAngle);
        path.lineTo(-HW, HH);
        path.lineTo(-HW, -HH);
        break;

    case PlaneConcaveLens:
        rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
        path.moveTo(HW, -HH);
        path.arcTo(rightSurface, 180-startAngle, sweepAngle);
        path.lineTo(-HW, HH);
        path.lineTo(-HW, -HH);
        break;

    case ConcavePlaneMirror:
        painter->setBrush(getMirrorBrush());
        leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
        path.moveTo(-HW, HH);
        path.arcTo(leftSurface, 360-startAngle, sweepAngle);
        path.lineTo(HW, -HH);
        path.lineTo(HW, HH);
        break;

    case ConcavePlaneLens:
        leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
        path.moveTo(-HW, HH);
        path.arcTo(leftSurface, 360-startAngle, sweepAngle);
        path.lineTo(HW, -HH);
        path.lineTo(HW, HH);
        break;

    case ConvexPlaneMirror:
        painter->setBrush(getMirrorBrush());
        leftSurface = QRectF(-HW, -ROC, 2*ROC, 2*ROC);
        path.moveTo(-HW + sagitta, -HH);
        path.arcTo(leftSurface, 180-startAngle, sweepAngle);
        path.lineTo(HW, HH);
        path.lineTo(HW, -HH);
        break;

    case ConvexPlaneLens:
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

    switch (_paintMode)
    {
    case PlaneConvexMirror:
    case PlaneConcaveMirror:
    case ConcavePlaneMirror:
    case ConvexPlaneMirror:
    {
        painter->setPen(getMirrorPen());

        switch (_paintMode)
        {
        case PlaneConvexMirror:
            painter->drawArc(rightSurface, int(16*(360-startAngle)), int(16*sweepAngle));
            break;

        case PlaneConcaveMirror:
            painter->drawArc(rightSurface, int(16*(180-startAngle)), int(16*sweepAngle));
            break;

        case ConcavePlaneMirror:
            painter->drawArc(leftSurface, int(16*(360-startAngle)), int(16*sweepAngle));
            break;

        case ConvexPlaneMirror:
            painter->drawArc(leftSurface, int(16*(180-startAngle)), int(16*sweepAngle));
            break;

        default: break;
        }
        break;
    }
    default: break;
    }

    if (_markTS != MarkNone)
    {
        auto p = boundingRect().bottomRight();
        p.setX(p.x()-qreal(HW)/2.0);
        painter->setFont(getMarkTSFont());
        painter->drawText(p, strMarkTS());
    }
}

const QString& CurvedElementView::strMarkTS() const
{
    static QString markT("T");
    static QString markS("S");
    return _markTS == MarkT? markT: markS;
}
