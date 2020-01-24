#ifndef PlotFuncWindowStorable_H
#define PlotFuncWindowStorable_H

#include "PlotFuncWindow.h"
#include "../io/ISchemaWindowStorable.h"

class PlotFuncWindowStorable : public PlotFuncWindow, public ISchemaWindowStorable
{
public:
    PlotFuncWindowStorable(PlotFunction* func) : PlotFuncWindow(func) {}

    // implementation of ISchemaWindowStorable
    QString storableType() const override { return _function->alias(); }
    bool storableRead(const QJsonObject& root, Z::Report* report) override;
    bool storableWrite(QJsonObject& root, Z::Report* report) override;

protected:
    virtual QString readFunction(const QJsonObject& root) { Q_UNUSED(root)  return QString(); }
    virtual QString writeFunction(QJsonObject& root) { Q_UNUSED(root)  return QString(); }
    virtual QString readWindowSpecific(const QJsonObject& root) { Q_UNUSED(root)  return QString(); }
    virtual QString writeWindowSpecific(QJsonObject& root) { Q_UNUSED(root)  return QString(); }

private:
    QString readWindowGeneral(const QJsonObject& root);
    QString writeWindowGeneral(QJsonObject& root) const;
};

#endif // PlotFuncWindowStorable_H
