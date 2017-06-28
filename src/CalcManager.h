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
    explicit CalcManager(Schema *schema, QWidget *parent = 0);

    Schema* schema() const { return _schema; }

public slots:
    void funcSummary();
    void funcRoundTrip();
    void funcMultFwd();
    void funcMultBkwd();
    void funcStabMap();
    void funcStabMap2d();
    void funcRepRate();
    void funcCaustic();
    void funcShowMatrices();
    void funcShowAllMatrices();

private:
    QWidget* _parent;
    Schema* _schema;

    void showInfoFunc(InfoFunction* func);
    template <class TWindow> void showPlotFunc();
};

#endif // CALC_MANAGER_H
