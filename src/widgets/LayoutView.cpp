#include "LayoutView.h"

#include <QtMath>

#define RadToDeg(angle) (angle * 180.0 / M_PI)
#define DegToRad(angle) (angle / 180.0 * M_PI)
#define Sqr(x) (x*x)

////////////////////////////////////////////////////////////////////////////////

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

const QFont& getLabelFont()
{
    static QFont f = QFont("Times", 12);
    return f;
}

const QFont& getMarkTSFont()
{
    static QFont f = QFont("Arial", 8, QFont::Bold);
    return f;
}

////////////////////////////////////////////////////////////////////////////////
//                               LayoutView
////////////////////////////////////////////////////////////////////////////////

LayoutView::LayoutView(QWidget *parent) : QGraphicsView(parent)
{
    _axis = new OpticalAxisView;
    _axis->setZValue(1000);
    _scene.addItem(_axis);

    setRenderHint(QPainter::Antialiasing, true);
    setScene(&_scene);
}

void LayoutView::add(ElementView *elem)
{
    if (!_elems.isEmpty())
    {
        ElementView *last = _elems.last();
        elem->setPos(last->x() + last->halfw() + elem->halfw(), 0);
    }
    else
        elem->setPos(0, 0);

    _elems.append(elem);

    qreal fullWidth = 0;
    foreach (ElementView *elem, _elems)
        fullWidth += elem->width();

    _axis->setLength(fullWidth + 20);
    _axis->setX(_axis->halfw() - _elems.first()->halfw() - 10);

    _scene.addItem(elem);

    addLabel(elem);
}

void LayoutView::addLabel(ElementView* elem)
{
    QGraphicsTextItem *label = _scene.addText(elem->label());
    label->setZValue(1000 + _elems.count());
    label->setFont(getLabelFont());
    QRectF r = label->boundingRect();
    label->setX(elem->x() - r.width() / 2.0);
    label->setY(elem->y() - elem->halfh() - r.height());
    _elemLabels.insert(elem, label);
}

void LayoutView::clear()
{
    _scene.removeItem(_axis);
    _scene.clear();
    _elems.clear();
    _elemLabels.clear();
    _scene.addItem(_axis);
}

////////////////////////////////////////////////////////////////////////////////
//                             ElementView
////////////////////////////////////////////////////////////////////////////////

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

QRectF ElementView::boundingRect() const
{
    return QRectF(-HW, -HH, 2*HW, 2*HH);
}


////////////////////////////////////////////////////////////////////////////////
//                             ElementRangeView
////////////////////////////////////////////////////////////////////////////////

void ElementRangeView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    Q_UNUSED(painter)

    //painter->setBrush(QColor(127, 127, 127, 50));
    //painter->drawRect(boundingRect());
}


////////////////////////////////////////////////////////////////////////////////
//                             OpticalAxisView
////////////////////////////////////////////////////////////////////////////////

void OpticalAxisView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->drawLine(-HW, 0, HW, 0);
}


////////////////////////////////////////////////////////////////////////////////
//                               ElemPlateView
////////////////////////////////////////////////////////////////////////////////

void ElemPlateView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(getGlassPen());
    painter->setBrush(getGlassBrush());

    slopePainter(painter);

    painter->drawRect(boundingRect());
}


////////////////////////////////////////////////////////////////////////////////
//                           ElemTiltedCrystalView
////////////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////////////
//                             ElemPointView
////////////////////////////////////////////////////////////////////////////////

void ElemPointView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(getGlassPen());
    painter->setBrush(Qt::black);
    painter->drawEllipse(boundingRect());
}

////////////////////////////////////////////////////////////////////////////////
//                           ElemMatrixView
////////////////////////////////////////////////////////////////////////////////

void ElemMatrixView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(getGlassPen());
    painter->fillRect(boundingRect(), getMirrorBrush());
    painter->drawRect(-HW, -HH, 2*HW, 2*HH);
    painter->drawLine(0, -HH, 0, HH);
    painter->drawLine(-HW, 0, HW, 0);
}

////////////////////////////////////////////////////////////////////////////////
//                           ElemMediumRangeView
////////////////////////////////////////////////////////////////////////////////

void ElemMediumRangeView::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->fillRect(boundingRect(), getGlassBrush());
    painter->setPen(getGlassPen());
    painter->drawLine(-HW, -HH, HW, -HH);
    painter->drawLine(-HW, HH, HW, HH);
}


////////////////////////////////////////////////////////////////////////////////
//                             ElemFlatMirrorView
////////////////////////////////////////////////////////////////////////////////

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
    painter->drawLine(x, -HH, x, HH);
}


////////////////////////////////////////////////////////////////////////////////
//                             ElemCurveMirrorView
////////////////////////////////////////////////////////////////////////////////

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
        // no break

    case PlaneConvexLens:
        rightSurface = QRectF(HW - 2*ROC, -ROC, 2*ROC, 2*ROC);
        path.moveTo(HW - sagitta, HH);
        path.arcTo(rightSurface, 360-startAngle, sweepAngle);
        path.lineTo(-HW, -HH);
        path.lineTo(-HW, HH);
        break;

    case PlaneConcaveMirror:
        painter->setBrush(getMirrorBrush());
        // no break

    case PlaneConcaveLens:
        rightSurface = QRectF(HW - sagitta, -ROC, 2*ROC, 2*ROC);
        path.moveTo(HW, -HH);
        path.arcTo(rightSurface, 180-startAngle, sweepAngle);
        path.lineTo(-HW, HH);
        path.lineTo(-HW, -HH);
        break;

    case ConcavePlaneMirror:
        painter->setBrush(getMirrorBrush());
        // no break

    case ConcavePlaneLens:
        leftSurface = QRectF(-HW + sagitta - 2*ROC, -ROC, 2*ROC, 2*ROC);
        path.moveTo(-HW, HH);
        path.arcTo(leftSurface, 360-startAngle, sweepAngle);
        path.lineTo(HW, -HH);
        path.lineTo(HW, HH);
        break;

    case ConvexPlaneMirror:
        painter->setBrush(getMirrorBrush());
        // no break

    case ConvexPlaneLens:
        leftSurface = QRectF(-HW, -ROC, 2*ROC, 2*ROC);
        path.moveTo(-HW + sagitta, -HH);
        path.arcTo(leftSurface, 180-startAngle, sweepAngle);
        path.lineTo(HW, HH);
        path.lineTo(HW, -HH);
        break;

    default: break;
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
            painter->drawArc(rightSurface, 16*(360-startAngle), 16*sweepAngle);
            break;

        case PlaneConcaveMirror:
            painter->drawArc(rightSurface, 16*(180-startAngle), 16*sweepAngle);
            break;

        case ConcavePlaneMirror:
            painter->drawArc(leftSurface, 16*(360-startAngle), 16*sweepAngle);
            break;

        case ConvexPlaneMirror:
            painter->drawArc(leftSurface, 16*(180-startAngle), 16*sweepAngle);
            break;

        default: break;
        }
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
