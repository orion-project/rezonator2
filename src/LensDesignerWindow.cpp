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
        painter->drawLine(QLineF(-len/2, 0, len/2, 0));
        painter->drawLine(QLineF(0, -high/2, 0, high/2));
    }

    double len = 0;
    double high = 0;
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

    void calc()
    {
        _path.clear();
        switch (getForm()) {
        case Plane: //      ||
            qDebug() << "Plane";
            _path.addRect(-T/2, -D/2, T, D);
            break;
        case PlanoConvex: //      |)
            qDebug() << "PlanoConvex";
        {
            const qreal sagitta = R2 - qSqrt(Sqr(R2) - Sqr(D/2));
            const qreal startAngle = qRadiansToDegrees(qAsin(D/2 / R2));
            const qreal sweepAngle = 2*startAngle;
            QRectF rightSurface = QRectF(T/2 + sagitta - 2*R2, -R2, 2*R2, 2*R2);

            _path.moveTo(-T/2, -D/2);
            _path.lineTo(-T/2, D/2);
            _path.lineTo(T/2, D/2);
            _path.arcTo(rightSurface, 360-startAngle, sweepAngle);
            _path.closeSubpath();
        }
            break;
        case PlanoConcave: //      |(
            qDebug() << "PlanoConcave";
            break;
        case ConcavePlano: //      )|
            qDebug() << "ConcavePlano";
            break;
        case ConvexPlano: //      (|
            qDebug() << "ConvexPlano";
            break;
        case ConcaveConvex: //      ))
            qDebug() << "ConcaveConvex";
            break;
        case Concave: //      )(
            qDebug() << "Concave";
            break;
        case Convex: //      ()
            qDebug() << "Convex";
            break;
        case ConvexConcave: //      ((
            qDebug() << "ConvexConcave";
            break;
        }
    }

    QRectF boundingRect() const override
    {
        return _path.boundingRect();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        painter->drawPath(_path);
    }

    QPainterPath _path;
    double R1 = 0;
    double R2 = 0;
    double D = 0;
    double T = 0;
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
    //_R1->setValue(-100_mm);
    _R1->setValue(0_mm); _R1->addListener(this);
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

    _axis = new LensDesignerItems::OpticalAxisItem;
    _shape = new LensDesignerItems::LensShapeItem;

    _scene = new QGraphicsScene(this);
    _scene->addItem(_axis);
    _scene->addItem(_shape);

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
    _shape->T = _T->value().toSi() * 1000;
    _shape->D = _D->value().toSi() * 1000;
    _shape->R1 = _R1->value().toSi() * 1000;
    _shape->R2 = _R2->value().toSi() * 1000;
    _shape->calc();

    auto r = _shape->boundingRect();
    _axis->len = r.width() * 2;
    _axis->high = r.height() * 1.5;

    auto b = _scene->itemsBoundingRect();
    b.adjust(-10, -10, 10, 10);
    _scene->setSceneRect(b);
    _view->scale(5, 5);
    //_view->fitInView(r, Qt::KeepAspectRatio);
    //_view->centerOn(r.center());
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
