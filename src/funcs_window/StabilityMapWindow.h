#ifndef WINDOW_STABILITY_MAP_H
#define WINDOW_STABILITY_MAP_H

#include "PlotFuncWindowStorable.h"
#include "../funcs_meat/StabilityMapFunction.h"

class StabilityMapWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit StabilityMapWindow(Schema*);

    bool configure(QWidget* parent) override;

    StabilityMapFunction* function() const { return (StabilityMapFunction*)_function; }

protected:
    QWidget* makeOptionsPanel() override;

private:
    QAction *actnStabilityAutolimits;

    void createControl();

private slots:
    void autolimitsStability();
};

#endif // WINDOW_STABILITY_MAP_H
