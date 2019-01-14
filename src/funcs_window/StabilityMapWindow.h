#ifndef WINDOW_STABILITY_MAP_H
#define WINDOW_STABILITY_MAP_H

#include "PlotFuncWindowStorable.h"
#include "../funcs/StabilityMapFunction.h"

class StabilityMapWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit StabilityMapWindow(Schema*);

    StabilityMapFunction* function() const { return dynamic_cast<StabilityMapFunction*>(_function); }

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    bool configureInternal() override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;

private:
    QAction *actnStabilityAutolimits;

    void createControl();

private slots:
    void autolimitsStability();
};

#endif // WINDOW_STABILITY_MAP_H
