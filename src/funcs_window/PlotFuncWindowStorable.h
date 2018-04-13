#ifndef PlotFuncWindowStorable_H
#define PlotFuncWindowStorable_H

#include "PlotFuncWindow.h"
#include "../io/ISchemaStorable.h"

class PlotFuncWindowStorable : public PlotFuncWindow, public ISchemaStorable
{
public:
    PlotFuncWindowStorable(PlotFunction* func) : PlotFuncWindow(func) {}

    // implementation of ISchemaStorable
    QString type() const override { return _function->alias(); }
    QString read(const QJsonObject& root) override;
    QString write(QJsonObject& root) override;

protected:
    virtual QString readFunction(const QJsonObject& root) { Q_UNUSED(root);  return QString(); }
    virtual QString writeFunction(QJsonObject& root) { Q_UNUSED(root);  return QString(); }
    virtual QString readWindowSpecific(const QJsonObject& root) { Q_UNUSED(root);  return QString(); }
    virtual QString writeWindowSpecific(QJsonObject& root) { Q_UNUSED(root);  return QString(); }

private:
    QString readWindowGeneral(const QJsonObject& root);
    QString writeWindowGeneral(QJsonObject& root) const;
};

#endif // PlotFuncWindowStorable_H
