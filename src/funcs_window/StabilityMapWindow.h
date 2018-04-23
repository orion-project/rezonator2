#ifndef WINDOW_STABILITY_MAP_H
#define WINDOW_STABILITY_MAP_H

#include "PlotFuncWindowStorable.h"
#include "../funcs_meat/StabilityMapFunction.h"

class StabilityMapWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit StabilityMapWindow(Schema*);

    StabilityMapFunction* function() const { return (StabilityMapFunction*)_function; }

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    bool configureInternal(QWidget* parent) override;

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
