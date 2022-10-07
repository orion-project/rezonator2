#ifndef LENSMAKER_WINDOW_H
#define LENSMAKER_WINDOW_H

#include "core/Parameters.h"

#include <QSplitter>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
QT_END_NAMESPACE

namespace LensmakerItems {
class LensItem;
class AxisItem;
class GridItem;
class PlaneItem;
class BeamItem;
class PointItem;
} // namespace LensmakerItems

namespace Z {
class GraphicsView;
}

class LensmakerWidget : public QSplitter, public Z::ParameterListener
{
    Q_OBJECT

public:
    explicit LensmakerWidget(QWidget *parent = nullptr);
    ~LensmakerWidget();

    void parameterChanged(Z::ParameterBase*) override;

    Z::GraphicsView* view() const { return _view; }

    void refresh(const char *reason);

    void storeValues(QJsonObject& root);
    void restoreValues(QJsonObject& root);

public slots:
    void zoomIn();
    void zoomOut();

private:
    QGraphicsScene* _scene;
    Z::GraphicsView* _view;
    Z::Parameter *_D, *_R1, *_R2, *_IOR, *_T, *_gridStep;
    Z::Parameter *_F, *_P;
    Z::Parameters _params;
    Z::Parameters _results;
    LensmakerItems::AxisItem* _axis;
    LensmakerItems::LensItem* _lens;
    LensmakerItems::GridItem* _grid;
    LensmakerItems::BeamItem *_beam, *_beamIm;
    LensmakerItems::PlaneItem *_rearFocus, *_frontFocus;
    LensmakerItems::PointItem *_rearFocusPt, *_frontFocusPt;
    double _targetH = 250;
    bool _restoring = false;

    void setTargetH(const double& v, bool doRefresh = true);
};


class LensmakerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LensmakerWindow(QWidget *parent = nullptr);
    ~LensmakerWindow();

    static void showWindow();

private:
    LensmakerWidget* _designer;

    void restoreState();
    void storeState();
};

#endif // LENSMAKER_WINDOW_H
