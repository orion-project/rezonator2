#ifndef MULTIRANGE_CAUSTIC_WINDOW_H
#define MULTIRANGE_CAUSTIC_WINDOW_H

#include "../funcs/MulticausticWindow.h"

class MultirangeCausticWindow final : public MulticausticWindow
{
    Q_OBJECT

public:
    explicit MultirangeCausticWindow(Schema*);

protected:
    // Implementation of PlotFuncWindow
    QWidget* makeOptionsPanel() override;
    void getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values) const override;
    QList<BasicMdiChild::MenuItem> viewMenuItems() const override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;

private:
    class BeamShapeExtension *_beamShape;

    void handleCursorPanelCommand(const QString& cmd);
};

#endif // MULTIRANGE_CAUSTIC_WINDOW_H
