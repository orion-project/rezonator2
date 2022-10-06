#include "LensmakerWindow.h"

#include "CustomPrefs.h"
#include "funcs/LensCalculator.h"
#include "io/Clipboard.h"
#include "widgets/ParamsEditor.h"
#include "widgets/ParamEditor.h"

#include "core/OriFloatingPoint.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriWindows.h"
#include "widgets/OriFlatToolBar.h"

#include <QContextMenuEvent>
#include <QIcon>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QtMath>
#include <QMenu>

#define Sqr(x) ((x)*(x))

namespace {

LensmakerWindow* __instance = nullptr;

} // namespace

namespace LensmakerItems {

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
        painter->drawLine(QLineF(-len/2.0, 0, len/2.0, 0));
        painter->drawLine(QLineF(0, -high/2.0, 0, high/2.0));
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
        return QRectF(-len/2.0, -high/2.0, len, high);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        double s = qAbs(step);
        if (s == 0) return;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen(pen);
        double x = s;
        while (x <= len/2.0) {
            painter->drawLine(QLineF(x, -high/2.0, x, high/2.0));
            painter->drawLine(QLineF(-x, -high/2.0, -x, high/2.0));
            x += s;
        }
        double y = s;
        while (y <= high/2.0) {
            painter->drawLine(QLineF(-len/2.0, y, len/2.0, y));
            painter->drawLine(QLineF(-len/2.0, -y, len/2.0, -y));
            y += s;
        }
        painter->restore();
    }

    double len = 0;
    double high = 0;
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
        if (plane1) return (R2 < 0) ? PlanoConvex : PlanoConcave;
        if (plane2) return (R1 < 0) ? ConcavePlano : ConvexPlano;
        if (R1 < 0) return (R2 < 0) ? ConcaveConvex : Concave;
        return (R2 < 0) ? Convex : ConvexConcave;
    }

    void calc()
    {
        const double t = qAbs(T)/2.0; // half-thickness of lens
        const double d = qAbs(D); // diameter of lens
        const double h = d/2.0; // half-diameter of lens
        const double r1 = qAbs(R1); // left diameter of curvature
        const double r2 = qAbs(R2); // right diameter of curvature
        const double d1 = r1*2.0; // left diameter of curvature
        const double d2 = r2*2.0; // right diameter of curvature
        double s1 = 0, a1 = 0, s2 = 0, a2 = 0;
        if (R1 != 0) {
            s1 = r1 - qSqrt(Sqr(r1) - Sqr(h)); // left arc sagitta
            a1 = qRadiansToDegrees(qAsin(h/r1)); // left arc half-angle
        }
        if (R2 != 0) {
            s2 = r2 - qSqrt(Sqr(r2) - Sqr(h)); // right arc sagitta
            a2 = qRadiansToDegrees(qAsin(h/r2)); // right arc half-angle
        }
        _path.clear();
        switch (getForm()) {
        case Plane: //      ||
            _path.moveTo(-t, -h);
            _path.lineTo(-t, h);
            _path.lineTo(t, h);
            _path.lineTo(t, -h);
            break;
        case PlanoConvex: //      |)
            _path.moveTo(-t, -h);
            _path.lineTo(-t, h);
            _path.lineTo(t-s2, h);
            _path.arcTo({t-d2, -r2, d2, d2}, 360-a2, 2*a2);
            break;
        case PlanoConcave: //      |(
            _path.moveTo(-t, -h);
            _path.lineTo(-t, h);
            _path.lineTo(t+s2, h);
            _path.arcTo({t, -r2, d2, d2}, 180+a2, -2*a2);
            break;
        case ConcavePlano: //      )|
            _path.moveTo(t, -h);
            _path.lineTo(t, h);
            _path.lineTo(-t-s1, h);
            _path.arcTo({-t-d1, -r1, d1, d1}, 360-a1, 2*a1);
            break;
        case ConvexPlano: //      (|
            _path.moveTo(t, -h);
            _path.lineTo(t, h);
            _path.lineTo(-t+s1, h);
            _path.arcTo({-t, -r1, d1, d1}, 180+a1, -2*a1);
            break;
        case Convex: //      ()
            _path.moveTo(-t+s1, -h);
            _path.arcTo({-t, -r1, d1, d1}, 180-a1, 2*a1);
            _path.lineTo(t-s2, h);
            _path.arcTo({t-d2, -r2, d2, d2}, 360-a2, 2*a2);
            break;
        case Concave: //      )(
            _path.moveTo(-t-s1, -h);
            _path.arcTo({-t-d1, -r1, d1, d1}, a1, -2*a1);
            _path.lineTo(t+s2, h);
            _path.arcTo({t, -r2, d2, d2}, 180+a2, -2*a2);
            break;
        case ConcaveConvex: //      ))
            _path.moveTo(-t-s1, -h);
            _path.arcTo({-t-d1, -r1, d1, d1}, a1, -2*a1);
            _path.lineTo(t-s2, h);
            _path.arcTo({t-d2, -r2, d2, d2}, 360-a2, 2*a2);
            break;
        case ConvexConcave: //      ((
            _path.moveTo(-t+s1, -h);
            _path.arcTo({-t, -r1, d1, d1}, 180-a1, 2*a1);
            _path.lineTo(t+s2, h);
            _path.arcTo({t, -r2, d2, d2}, 180+a2, -2*a2);
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
    double R1 = 0;
    double R2 = 0;
    double D = 0;
    double T = 0;
    QPen glassPen = QPen(Qt::black, 1.5);
    QBrush glassBrush = QBrush(QPixmap(":/misc/glass_pattern_big"));
};

} // namespace LensmakerItems

//--------------------------------------------------------------------------------
//                              LensmakerWidget
//--------------------------------------------------------------------------------

LensmakerWidget::LensmakerWidget(QWidget *parent) : QSplitter(parent)
{
    _D = new Z::Parameter(Z::Dims::linear(), QStringLiteral("D"), QStringLiteral("D"),
                         tr("Diameter", "Lens designer"),
                         tr("Lens diameter.", "Lens designer"));
    _R1 = new Z::Parameter(Z::Dims::linear(), QStringLiteral("R1"), QStringLiteral("R1"),
                          tr("Left ROC", "Lens designer"),
                          tr("Negative value means right-bulged surface, "
                             "positive value means left-bulged surface. "
                             "Set to zero to get plane face.", "Lens designer"));
    _R2 = new Z::Parameter(Z::Dims::linear(), QStringLiteral("R2"), QStringLiteral("R2"),
                          tr("Right ROC", "Lens designer"),
                          tr("Negative value means right-bulged surface, "
                             "positive value means left-bulged surface. "
                             "Set to zero to get plane face.", "Lens designer"));
    _IOR = new Z::Parameter(Z::Dims::none(), QStringLiteral("n"), QStringLiteral("n"),
                          tr("Index of refraction", "Lens designer"),
                          tr("Index of refraction of the lens material.", "Lens designer"));
    _T = new Z::Parameter(Z::Dims::linear(), QStringLiteral("T"), QStringLiteral("T"),
                          tr("Thickness", "Lens designer"),
                          tr("Distance between surfaces on axis.", "Lens designer"));
    _gridStep = new Z::Parameter(Z::Dims::linear(), QStringLiteral("grid_step"), tr("Grid step", "Lens designer"),
                                 tr("Grid step", "Lens designer"),
                                 tr("Distance between grid lines. Set to zero to disable grid.", "Lens designer"));
    _F = new Z::Parameter(Z::Dims::linear(), QStringLiteral("F"), QStringLiteral("F"),
                          tr("Focal length"), tr("Focal length."));
    _P = new Z::Parameter(Z::Dims::none(), QStringLiteral("P"), QStringLiteral("P"),
                          tr("Optical power"), tr("Optical power in dioptres."));

    _D->setValue(40_mm); _D->addListener(this);
    _R1->setValue(100_mm); _R1->addListener(this);
    _R2->setValue(-100_mm); _R2->addListener(this);
    _T->setValue(7_mm); _T->addListener(this);
    _IOR->setValue(1.7); _IOR->addListener(this);
    _gridStep->setValue(3_mm); _gridStep->addListener(this);
    _F->setValue(0_mm);
    _P->setValue(0);

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
    paramsEditor->addSeparator();
    paramsEditor->addEditor(_gridStep, reasonableUnits);
    paramsEditor->addSeparator();
    paramsEditor->addSeparator(tr("Results"));

    auto editorF = paramsEditor->addEditor(_F, reasonableUnits);
    editorF->setReadonly(true, false);
    editorF->rescaleOnUnitChange = true;

    auto editorP = paramsEditor->addEditor(_P);
    editorP->setReadonly(true, true);

    _grid = new LensmakerItems::PaperGridItem;
    _axis = new LensmakerItems::OpticalAxisItem;
    _shape = new LensmakerItems::LensShapeItem;

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

    refresh();
}

LensmakerWidget::~LensmakerWidget()
{
}

void LensmakerWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (!_menu) _menu = createContextMenu();
    _menu->popup(this->mapToGlobal(event->pos()));
}

QMenu* LensmakerWidget::createContextMenu()
{
    auto menu = new QMenu(this);
    menu->addAction(QIcon(":/toolbar/copy_img"), tr("Copy Image"), this, &LensmakerWidget::copyImage);
    return menu;
}

void LensmakerWidget::parameterChanged(Z::ParameterBase*)
{
    refresh();
}

void LensmakerWidget::refresh()
{
    LensCalculator calc;
    calc.T = _T->value().toSi();
    calc.n = _IOR->value().toSi();
    calc.R1 = _R1->value().toSi();
    calc.R2 = _R2->value().toSi();
    calc.calc();

    Z::Param::setSi(_F, calc.F);
    Z::Param::setSi(_P, calc.P);

    double D = _D->value().toSi();
    double scale = _targetH / D;

    _shape->D = D * scale;
    _shape->T = calc.T * scale;
    _shape->R1 = calc.R1 * scale;
    _shape->R2 = calc.R2 * scale;
    _shape->calc();

    auto r = _shape->boundingRect();
    double margin = D/4.0 * scale;
    r.adjust(-margin, -margin, margin, margin);

    _grid->step = _gridStep->value().toSi() * scale;
    _grid->len = r.width();
    _grid->high = r.height();

    _axis->len = r.width();
    _axis->high = r.height();

    _scene->setSceneRect(r);
    _scene->update(r);
}

void LensmakerWidget::setTargetH(const double& v, bool doRefresh)
{
    _targetH = round(v);
    if (_targetH < 100) _targetH = 100;
    if (doRefresh) refresh();
}

void LensmakerWidget::copyImage()
{
    Z::IO::Clipboard::setImage(_view);
}

void LensmakerWidget::zoomIn()
{
    setTargetH(targetH() * 1.1);
}

void LensmakerWidget::zoomOut()
{
    setTargetH(targetH() * 0.9);
}

//--------------------------------------------------------------------------------
//                              LensmakerWindow
//--------------------------------------------------------------------------------

void LensmakerWindow::showWindow()
{
    if (!__instance)
        __instance = new LensmakerWindow;
    __instance->show();
    __instance->activateWindow();
}

LensmakerWindow::LensmakerWindow(QWidget *parent) : QWidget(parent)
{
    Ori::Wnd::initWindow(this, tr("Lensmaker"), ":/window_icons/lens");

    _designer = new LensmakerWidget;

#define A_ Ori::Gui::action
    auto actnCopyImage = A_(tr("Copy Image"), _designer, SLOT(copyImage()), ":/toolbar/copy_img");
    auto actnZoomOut = A_(tr("Zoom Out"), _designer, SLOT(zoomOut()), ":/toolbar/zoom_out", QKeySequence::ZoomOut);
    auto actnZoomIn = A_(tr("Zoom In"), _designer, SLOT(zoomIn()), ":/toolbar/zoom_in", QKeySequence::ZoomIn);
#undef A_

    auto toolbar = new Ori::Widgets::FlatToolBar;
    Ori::Gui::populate(toolbar, {actnCopyImage, nullptr, actnZoomOut, actnZoomIn});

    Ori::Layouts::LayoutV({
                              toolbar,
                              Ori::Layouts::LayoutH({
                                  _designer
                              }).setMargin(3)
                          })
            .setSpacing(0)
            .setMargin(0)
            .useFor(this);

    restoreState();
    Ori::Wnd::moveToScreenCenter(this);
}

LensmakerWindow::~LensmakerWindow()
{
    storeState();
    __instance = nullptr;
}

void LensmakerWindow::restoreState()
{
    QJsonObject root = CustomDataHelpers::loadCustomData("lens");

    _designer->setTargetH(root["target_H"].toDouble(250));

    CustomDataHelpers::restoreWindowSize(root, this, 700, 500);
}

void LensmakerWindow::storeState()
{
    QJsonObject root;
    root["window_width"] = width();
    root["window_height"] = height();
    root["target_H"] = _designer->targetH();

    CustomDataHelpers::saveCustomData(root, "lens");
}
