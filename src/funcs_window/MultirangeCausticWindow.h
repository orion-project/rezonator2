#ifndef MULTIRANGE_CAUSTIC_WINDOW_H
#define MULTIRANGE_CAUSTIC_WINDOW_H

#include "MulticausticWindow.h"
#include "../funcs/MultirangeCausticFunction.h"

class MultirangeCausticWindow final : public MulticausticWindow
{
    Q_OBJECT

public:
    explicit MultirangeCausticWindow(Schema*);

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    QString getDefaultTitle() const override;
    QString getDefaultTitleY() const override;
    QString getCursorInfo(const QPointF& pos) const override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;
};

#endif // MULTIRANGE_CAUSTIC_WINDOW_H
