#ifndef GRIN_LENS_WINDOW_H
#define GRIN_LENS_WINDOW_H

#include <QWidget>

#include "../core/Parameters.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
class SvgView;
}}

class GrinLensWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GrinLensWindow(QWidget *parent = nullptr);
    ~GrinLensWindow() override;

    static void showWindow();

private slots:
    void calculate(Z::Parameter* p);

private:
    Z::Parameters _params;
    Z::Parameter *_length, *_ior, *_ior2, *_focus;
    QAction *_actionCalcN2, *_actionCalcF;
    QLabel *_statusLabel;
    Ori::Widgets::SvgView *_outline;
    int _outlineKind = 0;

    void restoreState();
    void storeState();
    void showError(const QString& err);
    void calculateN2();
    void calculateF();
    void updateOutline(bool neg);
};

#endif // GRIN_LENS_WINDOW_H
