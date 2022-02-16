#ifndef LENSMAKER_WINDOW_H
#define LENSMAKER_WINDOW_H

#include "core/Parameters.h"

#include <QSplitter>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsView;
QT_END_NAMESPACE

namespace LensmakerItems {
class LensShapeItem;
class OpticalAxisItem;
class PaperGridItem;
} // namespace LensmakerItems

class LensmakerWidget : public QSplitter, public Z::ParameterListener
{
    Q_OBJECT

public:
    explicit LensmakerWidget(QWidget *parent = nullptr);
    ~LensmakerWidget();

    void parameterChanged(Z::ParameterBase*) override;

private:
    QGraphicsScene* _scene;
    QGraphicsView* _view;
    Z::Parameter *_D, *_R1, *_R2, *_IOR, *_T, *_gridStep;
    Z::Parameter *_F, *_P;
    LensmakerItems::OpticalAxisItem* _axis;
    LensmakerItems::LensShapeItem* _shape;
    LensmakerItems::PaperGridItem* _grid;

    void refresh();
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
};

#endif // LENSMAKER_WINDOW_H
