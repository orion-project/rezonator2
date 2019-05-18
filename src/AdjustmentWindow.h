#ifndef ADJUSTMENT_WINDOW_H
#define ADJUSTMENT_WINDOW_H

#include <QWidget>
#include <QLabel>

#include "SchemaWindows.h"

class AdjusterWidget : public QWidget, public Z::ParameterListener
{
    Q_OBJECT

public:
    explicit AdjusterWidget(Z::Parameter* param, QWidget *parent = nullptr);
    ~AdjusterWidget() override;

    void parameterChanged(Z::ParameterBase*) override;

private:
    Z::Parameter* _param;
    QLabel* _testLabel;
};


class AdjusterListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdjusterListWidget(QWidget *parent = nullptr);

    void add(AdjusterWidget* w);
};


class AdjustmentWindow : public QWidget, public SchemaToolWindow
{
    Q_OBJECT

public:
    static void adjust(Schema* schema, Z::Parameter* param);
    ~AdjustmentWindow() override;

    // Implementation of SchemaListener
    void elementDeleting(Schema*, Element*) override;
    void customParamDeleting(Schema*, Z::Parameter*) override;

private:
    explicit AdjustmentWindow(Schema* schema, QWidget *parent = nullptr);

    struct AdjusterItem
    {
        Z::Parameter* param;
        AdjusterWidget* widget;
    };

    QList<AdjusterItem> _adjusters;
    AdjusterListWidget* _adjustersWidget;

    void addAdjuster(Z::Parameter* param);
    void deleteAdjuster(Z::Parameter* param);
};

#endif // ADJUSTMENT_WINDOW_H
