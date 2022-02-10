#include "LensDesignerWindow.h"

#include "widgets/ParamsEditor.h"

#include "core/OriFloatingPoint.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriFlatToolBar.h"

#include <QIcon>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QtMath>

#define Sqr(x) ((x)*(x))

namespace {

LensDesignerWindow* __instance = nullptr;

} // namespace

namespace LensDesignerItems {

class OpticalAxisItem : public QGraphicsItem
{
public:
    QRectF boundingRect() const override
    {
        return QRectF(-len/2, -high/2, len, high);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen(pen);
        painter->drawLine(QLineF(-len/2, 0, len/2, 0));
        painter->drawLine(QLineF(0, -high/2, 0, high/2));
        painter->restore();
    }

    double len = 0;
    double high = 0;
    QPen pen = QPen(Qt::black, 1, Qt::DashDotLine);
};

class PaperGridItem : public QGraphicsItem
{
public:
    QRectF boundingRect() const override
    {
        return QRectF(-len/2, -high/2, len, high);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen(pen);
        double x = step * scale;
        while (x <= len/2) {
            painter->drawLine(QLineF(x, -high/2, x, high/2));
            painter->drawLine(QLineF(-x, -high/2, -x, high/2));
            x += step * scale;
        }
        double y = step * scale;
        while (y <= high/2) {
            painter->drawLine(QLineF(-len/2, y, len/2, y));
            painter->drawLine(QLineF(-len/2, -y, len/2, -y));
            y += step * scale;
        }
        painter->restore();
    }

    double len = 0;
    double high = 0;
    double scale = 1;
    double step = 10;
    QPen pen = QPen(Qt::gray, 1, Qt::DotLine);
};

class LensShapeItem : public QGraphicsItem
{
    enum CurvedForm {
        Plane,          //      ||
        PlanoConvex,    //      |)
        PlanoConcave,   //      |(
        ConcavePlano,   //      )|
        ConvexPlano,    //      (|
        ConcaveConvex,  //      ))
        Concave,        //      )(
        Convex,         //      ()
        ConvexConcave,  //      ((
    };

public:
    CurvedForm getForm() const
    {
        bool plane1 = Double(R1).is(0);
        bool plane2 = Double(R2).is(0);
        if (plane1 && plane2) return Plane;
        if (plane1) return (R2 > 0) ? PlanoConvex : PlanoConcave;
        if (plane2) return (R1 > 0) ? ConcavePlano : ConvexPlano;
        if (R1 > 0) return (R2 > 0) ? ConcaveConvex : Concave;
        return (R2 > 0) ? Convex : ConvexConcave;
    }

    void rightArcTopBottom()
    {

    }

    void calc()
    {
        const qreal t = qAbs(T); // thickness of lens
        const qreal tt = t/2.0; // half-thickness of lens
        const qreal d = qAbs(D); // diameter of lens
        const qreal h = d/2.0; // half-diameter of lens
        const qreal r1 = qAbs(R1); // left diameter of curvature
        const qreal r2 = qAbs(R2); // right diameter of curvature
        const qreal d1 = r1*2.0; // left diameter of curvature
        const qreal d2 = r2*2.0; // right diameter of curvature
        qreal s1 = 0, a1 = 0, s2 = 0, a2 = 0;
        if (R1 != 0) {
            s1 = r1 - qSqrt(Sqr(r1) - Sqr(h)); // left arc sagitta
            a1 = qRadiansToDegrees(qAsin(h/r1)); // left arc half-angle
        }
        if (R2 != 0) {
            s2 = r2 - qSqrt(Sqr(r2) - Sqr(h)); // right arc sagitta
            a2 = qRadiansToDegrees(qAsin(h/r2)); // right arc half-angle
        }
        qreal tl = 0; // full lens half-thickness

        _path.clear();
        switch (getForm()) {
        case Plane: //      ||
            _path.moveTo(-tt, -h);
            _path.lineTo(-tt, h);
            _path.lineTo(tt, h);
            _path.lineTo(tt, -h);
            break;
        case PlanoConvex: //      |)
            tl = (t + s2)/2.0;
            _path.moveTo(-tl, -h);
            _path.lineTo(-tl, h);
            _path.lineTo(tl-s2, h);
            _path.arcTo({tl-d2, -r2, d2, d2}, 360-a2, 2*a2);
            break;
        case PlanoConcave: //      |(
            _path.moveTo(s2-tt, -h);
            _path.arcTo({tt, -r2, d2, d2}, 180-a2, 2*a2);
            _path.lineTo(-tt, h);
            _path.lineTo(-tt, -h);
            break;
        case ConcavePlano: //      )|
            _path.moveTo(-s1-tt, h);
            _path.arcTo({-d1-tt, -r1, d1, d1}, 360-a1, 2*a1);
            _path.lineTo(tt, -h);
            _path.lineTo(tt, h);
            break;
        case ConvexPlano: //      (|
            tl = (t + s1)/2.0;
            _path.moveTo(-tl+s1, -h);
            _path.arcTo({-tl, -r1, d1, d1}, 180-a1, 2*a1);
            _path.lineTo(tl, h);
            _path.lineTo(tl, -h);
            break;
        case ConcaveConvex: //      ))
            if (r1 < r2) {
                _path.moveTo(-s1-tt, h);
                _path.arcTo({-d1-tt, -r1, d1, d1}, 360-a1, 2*a1);
                _path.lineTo(tt-s1, -h);
                _path.arcTo({tt-d2, -r2, d2, d2}, a2, -2*a2);
            } else {
                tl = (t - s1 + s2)/2.0;
                _path.moveTo(-tl-s1, -h);
                _path.arcTo({-tl-d1, -r1, d1, d1}, a1, -2*a1);
                _path.lineTo(tl-s2, h);
                _path.arcTo({tl-d2, -r2, d2, d2}, 360-a2, 2*a2);
            }
            break;
        case Concave: //      )(
            _path.moveTo(-tt-s1, h);
            _path.arcTo({-tt-d1, -r1, d1, d1}, 360-a1, 2*a1);
            _path.lineTo(tt+s2, -h);
            _path.arcTo({tt, -r2, d2, d2}, 180-a2, 2*a2);
            break;
        case Convex: //      ()
            tl = (t + s1 + s2)/2.0;
            _path.moveTo(-tl+s1, -h);
            _path.arcTo({-tl, -r1, d1, d1}, 180-a1, 2*a1);
            _path.lineTo(tl-s2, h);
            _path.arcTo({tl-d2, -r2, d2, d2}, 360-a2, 2*a2);
            break;
        case ConvexConcave: //      ((
            if (r1 < r2) {
                tl = (t + s1 - s2)/2.0;
                _path.moveTo(-tl+s1, -h);
                _path.arcTo({-tl, -r1, d1, d1}, 180-a1, 2*a1);
                _path.lineTo(tl+s2, h);
                _path.arcTo({tl, -r2, d2, d2}, 180+a2, -2*a2);
            } else {
                _path.moveTo(-tt+s1, -h);
                _path.arcTo({-tt, -r1, d1, d1}, 180-a1, 2*a1);
                _path.lineTo(tt+s2, h);
                _path.arcTo({tt, -r2, d2, d2}, 180+a2, -2*a2);
            }
            break;
        }
        _path.closeSubpath();
    }

    QRectF boundingRect() const override
    {
        return _path.boundingRect();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        painter->setPen(glassPen);
        painter->setBrush(glassBrush);
        painter->drawPath(_path);
    }

    QPainterPath _path;
    qreal R1 = 0;
    qreal R2 = 0;
    qreal D = 0;
    qreal T = 0;
    QPen glassPen = QPen(Qt::black, 1.5);
    QBrush glassBrush = QBrush(QPixmap(":/misc/glass_pattern_big"));
};

} // namespace LensDesignerItems

//--------------------------------------------------------------------------------
//                              LensDesignerWidget
//--------------------------------------------------------------------------------

LensDesignerWidget::LensDesignerWidget(QWidget *parent) : QSplitter(parent)
{
    _D = new Z::Parameter(Z::Dims::linear(), QStringLiteral("D"), QStringLiteral("D"),
                         tr("Diameter", "Lens designer"),
                         tr("Lens diameter.", "Lens designer"));
    _R1 = new Z::Parameter(Z::Dims::linear(), QStringLiteral("R1"), QStringLiteral("R1"),
                          tr("Left ROC", "Lens designer"),
                          tr("Negative value means left-bulged surface, "
                             "positive value means right-bulged surface. "
                             "Set to zero to get plane face.", "Lens designer"));
    _R2 = new Z::Parameter(Z::Dims::linear(), QStringLiteral("R2"), QStringLiteral("R2"),
                          tr("Right ROC", "Lens designer"),
                          tr("Negative value means left-bulged surface, "
                             "positive value means right-bulged surface. "
                             "Set to zero to get plane face.", "Lens designer"));
    _IOR = new Z::Parameter(Z::Dims::none(), QStringLiteral("n"), QStringLiteral("n"),
                          tr("Index of refraction", "Lens designer"),
                          tr("Index of refraction of the lens material.", "Lens designer"));
    _T = new Z::Parameter(Z::Dims::linear(), QStringLiteral("T"), QStringLiteral("T"),
                          tr("Thickness", "Lens designer"),
                          tr("Minimal thickness.", "Lens designer"));

    _D->setValue(40_mm); _D->addListener(this);
    _R1->setValue(-100_mm); _R1->addListener(this);
    _R2->setValue(100_mm); _R2->addListener(this);
    _T->setValue(5_mm); _T->addListener(this);
    _IOR->setValue(1.7); _IOR->addListener(this);

    ParamsEditor::Options opts(nullptr);
    opts.ownParams = true;
    opts.checkChanges = true;
    opts.applyMode = ParamsEditor::Options::ApplyEnter;
    auto paramsEditor = new ParamsEditor(opts);
    QVector<Z::Unit> reasonableUnits{Z::Units::mm(), Z::Units::cm(), Z::Units::m()};
    paramsEditor->addEditor(_D, reasonableUnits);
    paramsEditor->addEditor(_R1, reasonableUnits);
    paramsEditor->addEditor(_R2, reasonableUnits);
    paramsEditor->addEditor(_IOR);
    paramsEditor->addEditor(_T, reasonableUnits);

    _grid = new LensDesignerItems::PaperGridItem;
    _axis = new LensDesignerItems::OpticalAxisItem;
    _shape = new LensDesignerItems::LensShapeItem;

    _scene = new QGraphicsScene(this);
    _scene->addItem(_grid);
    _scene->addItem(_shape);
    _scene->addItem(_axis);

    _view = new QGraphicsView;
    _view->setRenderHint(QPainter::Antialiasing, true);
    _view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    _view->setScene(_scene);

    addWidget(paramsEditor);
    addWidget(_view);
    setSizes({200, 600});
    setStretchFactor(0, 1);
    setStretchFactor(1, 20);

    redraw();
}

LensDesignerWidget::~LensDesignerWidget()
{
}

void LensDesignerWidget::parameterChanged(Z::ParameterBase*)
{
    qDebug() << "param changed";
    redraw();
}

void LensDesignerWidget::redraw()
{
    qreal tagretH = 300;
    qreal D = _D->value().toSi() * 1000;
    qreal scale = tagretH / D;

    _shape->D = D * scale;
    _shape->T = _T->value().toSi() * 1000 * scale;
    _shape->R1 = _R1->value().toSi() * 1000 * scale;
    _shape->R2 = _R2->value().toSi() * 1000 * scale;
    _shape->calc();

    auto r = _shape->boundingRect();

    qreal paperW = r.width() * 4;
    qreal paperH = r.height() * 1.5;

    _grid->scale = scale;
    _grid->len = paperW;
    _grid->high = paperH;

    _axis->len = paperW;
    _axis->high = paperH;

    qreal margin = 10 * scale;
    r.adjust(-margin, -margin, margin, margin);
    _scene->setSceneRect(r);
    _scene->update(r);
}

//--------------------------------------------------------------------------------
//                              LensDesignerWindow
//--------------------------------------------------------------------------------

void LensDesignerWindow::showWindow()
{
    if (!__instance)
        __instance = new LensDesignerWindow;
    __instance->show();
    __instance->activateWindow();
}

LensDesignerWindow::LensDesignerWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Lens Designer"));
    setWindowIcon(QIcon(":/window_icons/lens"));

    auto toolbar = new Ori::Widgets::FlatToolBar;

    _designer = new LensDesignerWidget;

    Ori::Layouts::LayoutV({toolbar, _designer}).setMargin(3).useFor(this);
}

LensDesignerWindow::~LensDesignerWindow()
{
    __instance = nullptr;
}
