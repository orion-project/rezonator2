#ifndef LENSMAKER_WINDOW_H
#define LENSMAKER_WINDOW_H

#include "../core/Parameters.h"

#include <QSplitter>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QTabWidget;
QT_END_NAMESPACE

namespace LensmakerItems {
class LensItem;
class AxisItem;
class GridItem;
class PlaneItem;
class BeamItem;
class PointItem;
}

namespace Z {
class GraphicsView;
}

namespace Ori {
namespace Widgets {
class MenuToolButton;
}}

class LensmakerWidget : public QSplitter, public Z::ParameterListener
{
    Q_OBJECT

public:
    explicit LensmakerWidget(QWidget *parent = nullptr);
    ~LensmakerWidget();

    void parameterChanged(Z::ParameterBase*) override;

    Z::GraphicsView* view() const { return _view; }

    void refresh(const char *reason);

    void zoomIn();
    void zoomOut();

    void storeValues(QJsonObject& root);
    void restoreValues(QJsonObject& root);

    int visibleParts() const { return _visibleParts; }
    void setVisibleParts(int parts, bool doRefresh);

    int index = 1;

protected:
    bool event(QEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private:
    QGraphicsScene* _scene;
    Z::GraphicsView* _view;
    Z::Parameter *_D, *_R1, *_R2, *_IOR, *_T, *_gridStep;
    Z::Parameter *_F, *_P, *_F1, *_F2;
    Z::Parameters _params;
    Z::Parameters _results;
    LensmakerItems::AxisItem* _axis;
    LensmakerItems::LensItem* _lens;
    LensmakerItems::GridItem* _grid;
    LensmakerItems::BeamItem *_beam, *_beamIm;
    LensmakerItems::PlaneItem *_focus1, *_focus2;
    LensmakerItems::PlaneItem *_princip1, *_princip2;
    LensmakerItems::PointItem *_vertex1, *_vertex2;
    double _targetH = 250;
    bool _restoring = false;
    int _visibleParts = 0;
    int _paramsW = 200, _viewW = 600;
    bool _splitted = false;

    void setTargetH(const double& v, bool doRefresh = true);
};


class LensmakerWindow : public QWidget
{
    Q_OBJECT

public:
    ~LensmakerWindow();

    static void showWindow();

private slots:
    void addLens();
    void removeLens();
    void zoomIn();
    void zoomOut();
    void copyImage();

private:
    explicit LensmakerWindow(QWidget *parent = nullptr);

    QTabWidget* _tabs;
    Ori::Widgets::MenuToolButton *_visibleParts;

    void restoreState();
    void storeState();
    void addTab(LensmakerWidget* lens);
    LensmakerWidget* activeLens();
};

#endif // LENSMAKER_WINDOW_H
