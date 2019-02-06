#ifndef CAUSTIC_WINDOW_H
#define CAUSTIC_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../funcs/CausticFunction.h"

class CausticWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit CausticWindow(Schema*);

    CausticFunction* function() const { return dynamic_cast<CausticFunction*>(_function); }

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    bool configureInternal() override;
    QString getDefaultTitle() const override;
    QString getDefaultTitleX() const override;
    QString getDefaultTitleY() const override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;
};

#endif // CAUSTIC_WINDOW_H
