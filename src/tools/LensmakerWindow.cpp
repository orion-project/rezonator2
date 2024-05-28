#include "LensmakerWindow.h"

#include "../app/Appearance.h"
#include "../app/PersistentState.h"
#include "../math/LensCalculator.h"
#include "../io/JsonUtils.h"
#include "../widgets/GraphicsView.h"
#include "../widgets/ParamsEditor.h"
#include "../widgets/ParamEditor.h"

#include "core/OriFloatingPoint.h"
#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriWindows.h"
#include "widgets/OriMenuToolButton.h"

#include <QContextMenuEvent>
#include <QGraphicsItem>
#include <QIcon>
#include <QMenu>
#include <QTabWidget>
#include <QToolBar>
#include <QtMath>

#define Sqr(x) ((x)*(x))

namespace {

LensmakerWindow* __instance = nullptr;

enum {
    PART_GRID = 0x01,
    PART_BEAM = 0x02,
    PART_FOCUS = 0x04,
    PART_PRINCIP = 0x08,
    PART_VERTEX = 0x10,
};

} // namespace

namespace LensmakerItems {

class AxisItem : public QGraphicsItem
{
public:
    AxisItem(QGraphicsItem *parent = nullptr) : QGraphicsItem(parent)
    {
        pen.setDashPattern({20, 5, 2, 5});
    }

    QRectF boundingRect() const override
    {
        return rect;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen(pen);
        painter->drawLine(QLineF(rect.left(), 0, rect.right(), 0));
        painter->restore();
    }

    QRectF rect;
    QPen pen = QPen(QColor(50, 50, 50), 1, Qt::DashDotLine);
};

class GridItem : public QGraphicsItem
{
public:
    QRectF boundingRect() const override
    {
        return rect;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        qreal s = qAbs(step);
        if (Double(s).is(0)) return;

        qreal high = rect.height()/2.0;
        qreal left = rect.left();
        qreal right = rect.right();

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen(pen);

        // positive verticals
        double x = 0;
        while (x <= right) {
            painter->drawLine(QLineF(x, -high, x, high));
            x += s;
        }
        // negative verticals
        x = -s;
        while (x >= left) {
            painter->drawLine(QLineF(x, -high, x, high));
            x -= s;
        }
        // horizontals
        double y = s;
        while (y <= high) {
            painter->drawLine(QLineF(left, y, right, y));
            painter->drawLine(QLineF(left, -y, right, -y));
            y += s;
        }
        painter->restore();
    }

    QRectF rect;
    qreal step = 10;
    QPen pen = QPen(Qt::gray, 1, Qt::DotLine);
};

class LensItem : public QGraphicsItem
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
        if (!Double(R1).is(0)) {
            s1 = r1 - qSqrt(Sqr(r1) - Sqr(h)); // left arc sagitta
            a1 = qRadiansToDegrees(qAsin(h/r1)); // left arc half-angle
        }
        if (!Double(R2).is(0)) {
            s2 = r2 - qSqrt(Sqr(r2) - Sqr(h)); // right arc sagitta
            a2 = qRadiansToDegrees(qAsin(h/r2)); // right arc half-angle
        }
        _path = QPainterPath();
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
    qreal R1 = 0;
    qreal R2 = 0;
    qreal D = 0;
    qreal T = 0;
    QPen glassPen = QPen(Qt::black, 1.5);
    QBrush glassBrush = QBrush(QPixmap(":/misc/glass_pattern_big"));
};

class PlaneItem : public QGraphicsItem
{
public:
    PlaneItem(QGraphicsItem *parent = nullptr) : QGraphicsItem(parent)
    {
        pen.setDashPattern({6, 6});
    }

    QRectF boundingRect() const override
    {
        return QRectF(pos-r, -high/2.0, 2*r, high);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen(pen);
        painter->drawLine(QLineF(pos, -high/2.0, pos, high/2.0));
        if (!title.isEmpty())
        {
            auto f = Z::Gui::ElemLabelFont().get();
            QFontMetricsF fm(f);
            painter->setFont(f);
            painter->drawText(QPointF(titleAtLeft ? pos-fm.boundingRect(title).width()-6 : pos+3, -high/2.0+fm.height()-3), title);
        }
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setBrush(brush);
        painter->drawEllipse(QRectF(pos-r, -r, 2*r, 2*r));
        painter->restore();
    }

    qreal pos = 0;
    qreal high = 0;
    qreal r = 3;
    QString title;
    bool titleAtLeft = false;
    QPen pen = QPen(QColor(50, 50, 50), 1, Qt::DashLine);
    QBrush brush = QBrush(Qt::black);
};

class BeamItem : public QGraphicsItem
{
public:
    QRectF boundingRect() const override
    {
        if (points.empty())
            return QRectF();

        qreal minX = 0, maxX = 0, maxY = 0;
        foreach (const QPointF &p, points) {
            if (p.x() < minX) minX = p.x();
            if (p.x() > maxX) maxX = p.x();
            if (p.y() > maxY) maxY = p.y();
        }
        return QRectF(minX, -qAbs(maxY), maxX-minX, qAbs(maxY)*2.0);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        if (points.empty())
            return;

        QPainterPath path, path1;
        bool first = true;
        foreach (const QPointF &p, points) {
            QPointF p1(p.x(), -p.y());
            if (first) {
                path.moveTo(p);
                path1.moveTo(p1);
                first = false;
            } else {
                path.lineTo(p);
                path1.lineTo(p1);
            }
        }
        painter->setPen(pen);
        painter->drawPath(path);
        painter->drawPath(path1);
    }

    QVector<QPointF> points;
    QPen pen = QPen(Qt::red, 1.5);
};

class PointItem : public QGraphicsItem
{
public:
    QRectF boundingRect() const override
    {
        return QRectF(x-r, y-r, 2*r, 2*r);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawEllipse(QRectF(x-r, y-r, 2*r, 2*r));
        if (!title.isEmpty())
        {
            auto f = Z::Gui::ElemLabelFont().get();
            QFontMetrics fm(f);
            painter->setFont(f);
            painter->drawText(QPointF(titleAtLeft ? x-fm.boundingRect(title).width()-6 : x+3, y+fm.height()-3), title);
        }
    }

    qreal x = 0;
    qreal y = 0;
    qreal r = 3;
    QString title;
    bool titleAtLeft = false;
    QPen pen = QPen(Qt::black, 1);
    QBrush brush = QBrush(Qt::black);
};

} // namespace LensmakerItems

//--------------------------------------------------------------------------------
//                              LensmakerWidget
//--------------------------------------------------------------------------------

LensmakerWidget::LensmakerWidget(QWidget *parent) : QSplitter(parent)
{
    _D = new Z::Parameter(Z::Dims::linear(), QStringLiteral("D"), QStringLiteral("D"),
                         tr("Diameter"), tr("Lens diameter."));
    _R1 = new Z::Parameter(Z::Dims::linear(), QStringLiteral("R1"), QStringLiteral("R<sub>1</sub>"),
                          tr("Left ROC"),
                          tr("Negative value means right-bulged surface, "
                             "positive value means left-bulged surface. "
                             "Set to zero to get planar face."));
    _R2 = new Z::Parameter(Z::Dims::linear(), QStringLiteral("R2"), QStringLiteral("R<sub>2</sub>"),
                          tr("Right ROC"),
                          tr("Negative value means right-bulged surface, "
                             "positive value means left-bulged surface. "
                             "Set to zero to get planar face."));
    _IOR = new Z::Parameter(Z::Dims::none(), QStringLiteral("n"), QStringLiteral("n"),
                          tr("Index of refraction"), tr("Index of refraction of the lens material."));
    _T = new Z::Parameter(Z::Dims::linear(), QStringLiteral("T"), QStringLiteral("T"),
                          tr("Thickness"), tr("Distance between surfaces on axis."));
    _gridStep = new Z::Parameter(Z::Dims::linear(), QStringLiteral("grid_step"), tr("Grid step", "Lens designer"),
                          tr("Grid step"), tr("Distance between grid lines. Set to zero to disable grid."));
    _F = new Z::Parameter(Z::Dims::linear(), QStringLiteral("F"), QStringLiteral("F"),
                          tr("Effective focal length"),
                          tr("Distance between focal point and respective principal plane. <code>H-F</code> or <code>F'-H'</code>"));
    _P = new Z::Parameter(Z::Dims::none(), QStringLiteral("P"), QStringLiteral("P"),
                          tr("Optical power"), tr("Optical power in dioptres."));
    _F1 = new Z::Parameter(Z::Dims::linear(), QStringLiteral("F1"), QStringLiteral("F<sub>F</sub>"),
                          tr("Front focal length"), tr("Distance between the front focal point <code>F</code> and the left surface"));
    _F2 = new Z::Parameter(Z::Dims::linear(), QStringLiteral("F2"), QStringLiteral("F<sub>R</sub>"),
                          tr("Rear focal length"), tr("Distance between the right surface and the rear focal point <code>F'</code>"));

    _D->setValue(40_mm); _D->addListener(this); _params.append(_D);
    _R1->setValue(40_mm); _R1->addListener(this); _params.append(_R1);
    _R2->setValue(-75_mm); _R2->addListener(this); _params.append(_R2);
    _T->setValue(15_mm); _T->addListener(this); _params.append(_T);
    _IOR->setValue(1.7); _IOR->addListener(this); _params.append(_IOR);
    _gridStep->setValue(5_mm); _gridStep->addListener(this); _params.append(_gridStep);
    _F->setValue(0_mm); _results.append(_F);
    _P->setValue(0); _results.append(_P);
    _F1->setValue(0_mm); _results.append(_F1);
    _F2->setValue(0_mm); _results.append(_F2);

    ParamsEditor::Options opts(nullptr);
    opts.ownParams = true;
    opts.checkChanges = true;
    opts.applyMode = ParamsEditor::Options::ApplyEnter;
    auto paramsEditor = new ParamsEditor(opts);
    QVector<Z::Unit> reasonableUnits{Z::Units::mm(), Z::Units::cm(), Z::Units::m()};
    paramsEditor->addEditor(_D, {Z::Units::mm(), Z::Units::cm()});
    paramsEditor->addEditor(_R1, reasonableUnits);
    paramsEditor->addEditor(_R2, reasonableUnits);
    paramsEditor->addEditor(_IOR);
    paramsEditor->addEditor(_T, {Z::Units::mm(), Z::Units::cm()});
    paramsEditor->addSeparator();
    paramsEditor->addEditor(_gridStep, reasonableUnits);
    paramsEditor->addSeparator();
    paramsEditor->addSeparator(tr("Results"));

    auto editorF = paramsEditor->addEditor(_F, reasonableUnits);
    editorF->setReadonly(true, false);
    editorF->rescaleOnUnitChange = true;

    auto editorP = paramsEditor->addEditor(_P);
    editorP->setReadonly(true, true);

    auto editorFF = paramsEditor->addEditor(_F1, reasonableUnits);
    editorFF->setReadonly(true, false);
    editorFF->rescaleOnUnitChange = true;

    auto editorRF = paramsEditor->addEditor(_F2, reasonableUnits);
    editorRF->setReadonly(true, false);
    editorRF->rescaleOnUnitChange = true;

    _scene = new QGraphicsScene(this);
    _scene->addItem(_grid = new LensmakerItems::GridItem);
    _scene->addItem(_lens = new LensmakerItems::LensItem);
    _scene->addItem(_axis = new LensmakerItems::AxisItem);
    _scene->addItem(_beam = new LensmakerItems::BeamItem);
    _scene->addItem(_beamIm = new LensmakerItems::BeamItem);
    _scene->addItem(_focus1 = new LensmakerItems::PlaneItem);
    _scene->addItem(_focus2 = new LensmakerItems::PlaneItem);
    _scene->addItem(_princip1 = new LensmakerItems::PlaneItem);
    _scene->addItem(_princip2 = new LensmakerItems::PlaneItem);
    _scene->addItem(_vertex1 = new LensmakerItems::PointItem);
    _scene->addItem(_vertex2 = new LensmakerItems::PointItem);

    _focus1->title = "F"; _focus1->setData(0, _focus1->title);
    _focus2->title = "F'"; _focus2->setData(0, _focus2->title);
    _princip1->title = "H"; _princip1->setData(0, _princip1->title); _princip1->titleAtLeft = true;
    _princip2->title = "H'"; _princip2->setData(0, _princip2->title);
    _vertex1->title = "V"; _vertex1->titleAtLeft = true;
    _vertex2->title = "V'";
    _beamIm->pen = _focus2->pen;

    setVisibleParts(PART_GRID | PART_FOCUS | PART_PRINCIP, false);

    _view = new Z::GraphicsView;
    _view->setScene(_scene);
    connect(_view, &Z::GraphicsView::zoomIn, this, &LensmakerWidget::zoomIn);
    connect(_view, &Z::GraphicsView::zoomOut, this, &LensmakerWidget::zoomOut);

    addWidget(paramsEditor);
    addWidget(_view);
    setStretchFactor(0, 1);
    setStretchFactor(1, 20);
    connect(this, &QSplitter::splitterMoved, [this]{
        auto sz = sizes();
        _paramsW = sz.at(0);
        _viewW = sz.at(1);
    });
}

LensmakerWidget::~LensmakerWidget()
{
}

void LensmakerWidget::parameterChanged(Z::ParameterBase*)
{
    if (_restoring)
        return;

    refresh("param");
}

void LensmakerWidget::refresh(const char *reason)
{
    Q_UNUSED(reason)
    //qDebug() << "refresh lens view:" << index << reason;

    LensCalculator calc;
    calc.T = qAbs(_T->value().toSi());
    calc.n = qAbs(_IOR->value().toSi());
    calc.R1 = _R1->value().toSi();
    calc.R2 = _R2->value().toSi();
    calc.calc();

    Z::Param::setSi(_F, calc.F);
    Z::Param::setSi(_P, calc.P);
    Z::Param::setSi(_F1, calc.F1);
    Z::Param::setSi(_F2, calc.F2);

    qreal scale = _targetH / qAbs(_D->value().toSi());

    _lens->D = _targetH;
    _lens->T = calc.T * scale;
    _lens->R1 = calc.R1 * scale;
    _lens->R2 = calc.R2 * scale;
    _lens->calc();

    _vertex1->x = -_lens->T / 2.0;
    _vertex2->x = _lens->T / 2.0;

    qreal margin = _lens->D / 4.0;
    qreal gridH = _lens->D + 2*margin;
    qreal focus = calc.F * scale;
    qreal beamH = _lens->D * 0.45;

    _focus1->setVisible(!calc.planar && _visibleParts&PART_FOCUS);
    _focus2->setVisible(!calc.planar && _visibleParts&PART_FOCUS);
    _princip1->setVisible(!calc.planar && _visibleParts&PART_PRINCIP);
    _princip2->setVisible(!calc.planar && _visibleParts&PART_PRINCIP);
    _beam->points.clear();
    _beamIm->points.clear();

    if (!calc.planar)
    {
        _focus1->high = gridH;
        _focus2->high = gridH;
        _princip1->high = gridH;
        _princip2->high = gridH;
        _focus1->pos = -_lens->T/2.0 + calc.F1*scale;
        _focus2->pos = _lens->T/2.0 + calc.F2*scale;
        _princip1->pos = _focus1->pos + focus;
        _princip2->pos = _focus2->pos - focus;

        // The first point will be inserted when we know full bounds
        //_beam->points.append({-_axis->len / 2.0, beamH});
        _beam->points.append({_princip2->pos, beamH});
        if (focus > 0)
        {
            _beam->points.append({_focus2->pos, 0});
        }
        else
        {
            _beam->points.append({_princip2->pos - focus * (gridH/2.0 - beamH) / beamH, gridH/2.0});
            _beamIm->points.append({_focus2->pos, 0});
            _beamIm->points.append({_princip2->pos, beamH});
        }
    }

    qreal step = qAbs(_gridStep->value().toSi()) * scale;

    QRectF r = _lens->boundingRect();
    qreal marginV = margin;
    if (_focus1->isVisible()) {
        r = r.united(_focus1->boundingRect())
             .united(_focus2->boundingRect());
        // planes already take extra vertical space
        marginV = 0;
    }
    if (_princip1->isVisible()) {
        r = r.united(_princip1->boundingRect())
             .united(_princip2->boundingRect());
        // planes already take extra vertical space
        marginV = 0;
    }
    if (_beam->isVisible())
        r = r.united(_beam->boundingRect())
             .united(_beamIm->boundingRect());
    r = r.adjusted(-margin, -marginV, margin, marginV);

    if (!_beam->points.empty())
        _beam->points.insert(0, {r.left(), beamH});

    _grid->rect = r;
    _grid->step = step;

    _axis->rect = r;

    _scene->setSceneRect(r);
    _scene->update(r);
}

void LensmakerWidget::setTargetH(const double& v, bool doRefresh)
{
    _targetH = round(v);
    if (_targetH < 100) _targetH = 100;
    if (doRefresh) refresh("zoom");
}

void LensmakerWidget::zoomIn()
{
    setTargetH(_targetH * 1.1);
}

void LensmakerWidget::zoomOut()
{
    setTargetH(_targetH * 0.9);
}

void LensmakerWidget::storeValues(QJsonObject& root)
{
    root["lens_height"] = _targetH;

    root["width_params"] = _paramsW;
    root["width_view"] = _viewW;
    root["show_grid"] = bool(_visibleParts & PART_GRID);
    root["show_beam"] = bool(_visibleParts & PART_BEAM);
    root["show_focus"] = bool(_visibleParts & PART_FOCUS);
    root["show_princip"] = bool(_visibleParts & PART_PRINCIP);
    root["show_vertex"] = bool(_visibleParts & PART_VERTEX);

    foreach(Z::Parameter *p, _params) {
        root["param_"+p->alias()] = Z::IO::Json::writeValue(p->value());
    }
    foreach(Z::Parameter *p, _results) {
        if (p->dim() == Z::Dims::none())
            continue;

        QJsonObject json;
        Z::IO::Json::writeUnit(json, p->value().unit());
        root["result_"+p->alias()] = json;
    }
}

void LensmakerWidget::restoreValues(QJsonObject& root)
{
    _restoring = true;

    setTargetH(root["lens_height"].toDouble(250), false);

    _paramsW = root["width_params"].toInt(_paramsW);
    _viewW = root["width_view"].toInt(_paramsW);

    int parts = 0;
    if (root["show_grid"].toBool(true)) parts |= PART_GRID;
    if (root["show_beam"].toBool(true)) parts |= PART_BEAM;
    if (root["show_focus"].toBool(true)) parts |= PART_FOCUS;
    if (root["show_princip"].toBool(true)) parts |= PART_PRINCIP;
    if (root["show_vertex"].toBool(false)) parts |= PART_VERTEX;
    setVisibleParts(parts, false);

    foreach(Z::Parameter *p, _params)
    {
        auto res = Z::IO::Json::readValue(root["param_"+p->alias()].toObject(), p->dim());
        if (res.ok())
            p->setValue(res.value());
    }
    foreach(Z::Parameter *p, _results) {
        if (p->dim() == Z::Dims::none())
            continue;

        auto res = Z::IO::Json::readUnit(root["result_"+p->alias()].toObject(), p->dim());
        if (res.ok())
            p->setValue({0, res.value()}); // only unit matters, a magnituge will be updated
    }

    _restoring = false;
}

bool LensmakerWidget::event(QEvent *e)
{
    if (dynamic_cast<QShowEvent*>(e))
    {
        // Apply sizes on first show
        if (_paramsW > 0 && _viewW > 0 && !_splitted)
        {
            setSizes({_paramsW, _viewW});
            _splitted = true;
        }
    }
    return QSplitter::event(e);
}

void LensmakerWidget::resizeEvent(QResizeEvent *e)
{
    QSplitter::resizeEvent(e);
    // First resize happens before show,
    // splitter doesn't have valid sizes yet
    if (!isVisible()) return;
    auto sz = sizes();
    _paramsW = sz.at(0);
    _viewW = sz.at(1);
}

void LensmakerWidget::setVisibleParts(int parts, bool doRefresh)
{
    _visibleParts = parts;
    _grid->setVisible(parts & PART_GRID);
    _beam->setVisible(parts & PART_BEAM);
    _beamIm->setVisible(parts & PART_BEAM);
    _vertex1->setVisible(parts & PART_VERTEX);
    _vertex2->setVisible(parts & PART_VERTEX);
    if (doRefresh)
        refresh("visbility");
}

//--------------------------------------------------------------------------------
//                              LensmakerWindow
//--------------------------------------------------------------------------------

void LensmakerWindow::showWindow()
{
    if (!__instance)
        __instance = new LensmakerWindow;
    __instance->show();
    __instance->raise();
    __instance->activateWindow();
}

LensmakerWindow::LensmakerWindow(QWidget *parent) : QWidget(parent)
{
    Ori::Wnd::initWindow(this, "Lensmaker", ":/window_icons/lens");

    auto actnAddLens = Ori::Gui::V0::action(tr("Add Lens"), this, SLOT(addLens()), ":/toolbar/elem_add", QKeySequence::New);
    auto actnDelLens = Ori::Gui::V0::action(tr("Remove Lens"), this, SLOT(removeLens()), ":/toolbar/elem_delete");
    auto actnCopyImage = Ori::Gui::V0::action(tr("Copy Image"), this, SLOT(copyImage()), ":/toolbar/copy_img");
    auto actnZoomOut = Ori::Gui::V0::action(tr("Zoom Out"), this, SLOT(zoomOut()), ":/toolbar/zoom_out", QKeySequence::ZoomOut);
    auto actnZoomIn = Ori::Gui::V0::action(tr("Zoom In"), this, SLOT(zoomIn()), ":/toolbar/zoom_in", QKeySequence::ZoomIn);

    _visibleParts = new Ori::Widgets::MenuToolButton;
    _visibleParts->multiselect = true;
    _visibleParts->setIcon(QIcon(":/toolbar/options"));
    _visibleParts->setToolTip(tr("Visible parts"));
    _visibleParts->addAction(PART_GRID, tr("Grid"));
    _visibleParts->addAction(PART_BEAM, tr("Beam"));
    _visibleParts->addAction(PART_FOCUS, tr("Focal planes"));
    _visibleParts->addAction(PART_PRINCIP, tr("Principal planes"));
    _visibleParts->addAction(PART_VERTEX, tr("Surface vertices"));
    connect(_visibleParts, &QToolButton::triggered, this, [this]{
        auto lens = activeLens();
        if (lens) lens->setVisibleParts(_visibleParts->selectedFlags(0), true);
    });
    connect(_visibleParts, &Ori::Widgets::MenuToolButton::aboutToShow, this, [this]{
        auto lens = activeLens();
        if (lens) _visibleParts->setSelectedFlags(lens->visibleParts());
    });

    auto toolbar = Z::Gui::makeToolbar({ actnAddLens, actnDelLens,
                                         nullptr, actnCopyImage,
                                         nullptr, actnZoomOut, actnZoomIn,
                                         nullptr, _visibleParts,
                                       }, "calc_lens");

    _tabs = Z::Gui::makeBorderlessTabs();

    Ori::Layouts::LayoutV({toolbar, _tabs}).setSpacing(0).setMargin(0).useFor(this);

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
    QJsonObject root = PersistentState::load("lens");
    PersistentState::restoreWindowGeometry(root, this);

    auto jsonLenses = root["lenses"];
    if (jsonLenses.isArray())
    {
        int index = 1;
        auto lensArray = jsonLenses.toArray();
        for (auto it = lensArray.begin(); it != lensArray.end(); it++)
        {
            auto lens = new LensmakerWidget;
            lens->index = index;
            QJsonObject json = it->toObject();
            lens->restoreValues(json);
            lens->refresh("restore");
            addTab(lens);
            index++;
        }
    }

    if (_tabs->count() > 0)
        _tabs->setCurrentIndex(root["active_lens"].toInt(0));
    else
        addLens();
}

void LensmakerWindow::storeState()
{
    QJsonObject root;

    QJsonArray jsonLenses;
    for (int i = 0; i < _tabs->count(); i++)
    {
        auto lens = qobject_cast<LensmakerWidget*>(_tabs->widget(i));
        if (!lens)
            continue;
        QJsonObject json;
        lens->storeValues(json);
        jsonLenses.append(json);
    }
    root["lenses"] = jsonLenses;
    root["active_lens"] = _tabs->currentIndex();

    PersistentState::storeWindowGeometry(root, this);
    PersistentState::save("lens", root);
}

void LensmakerWindow::addLens()
{
    int maxIndex = 0;
    QJsonObject json;
    for (int i = 0; i < _tabs->count(); i++)
    {
        auto lens = qobject_cast<LensmakerWidget*>(_tabs->widget(i));
        if (!lens)
            continue;
        if (lens->index > maxIndex)
            maxIndex = lens->index;
        if (i == _tabs->currentIndex())
            lens->storeValues(json);
    }
    auto lens = new LensmakerWidget;
    lens->index = maxIndex+1;
    lens->restoreValues(json);
    lens->refresh("init");
    addTab(lens);
    _tabs->setCurrentWidget(lens);
}

void LensmakerWindow::addTab(LensmakerWidget* lens)
{
    _tabs->addTab(lens, QStringLiteral("Lens %1").arg(lens->index));
}

void LensmakerWindow::removeLens()
{
    if (_tabs->count() == 1)
        return;

    if (!Ori::Dlg::yes(tr("Remove the lens?")))
        return;

    auto lens = activeLens();
    if (lens) lens->deleteLater();
}

LensmakerWidget* LensmakerWindow::activeLens()
{
    return qobject_cast<LensmakerWidget*>(_tabs->currentWidget());
}

void LensmakerWindow::zoomIn()
{
    auto lens = activeLens();
    if (lens) lens->zoomIn();
}

void LensmakerWindow::zoomOut()
{
    auto lens = activeLens();
    if (lens) lens->zoomOut();
}

void LensmakerWindow::copyImage()
{
    auto lens = activeLens();
    if (lens) lens->view()->copyImage();
}
