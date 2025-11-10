#ifndef CALC_MANAGER_H
#define CALC_MANAGER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class Schema;
class InfoFunction;

class CalcManager : public QObject
{
    Q_OBJECT

public:
    explicit CalcManager(Schema *schema, QWidget *parent = nullptr);

    Schema* schema() const { return _schema; }

public slots:
    void funcSummary();
    void funcRoundTrip();
    void funcMatrixMult();
    void funcStabMap();
    void funcStabMap2d();
    void funcRepRate();
    void funcCaustic();
    void funcMultirangeCaustic();
    void funcMultibeamCaustic();
    void funcShowMatrices();
    void funcShowAllMatrices();
    void funcBeamVariation();
    void funcBeamParamsAtElems();
    void funcCustomTable();
    void funcCustomPlot();

private:
    QWidget* _parent;
    Schema* _schema;

    void showInfoFunc(InfoFunction* func);
    template <class TWindow> void showPlotFunc();
    template <class TWindow> void showPlotFuncV2();
    template <class TFunction> void showTableFunc();
};

#endif // CALC_MANAGER_H
