#include "MultirangeCausticWindow.h"

#include "BeamShapeExtension.h"
#include "CausticOptionsPanel.h"
#include "../core/Format.h"
#include "../funcs/CausticFunction.h"
#include "../io/CommonUtils.h"

#include <qcpl_plot.h>

#include <QDebug>
#include <QJsonObject>

MultirangeCausticWindow::MultirangeCausticWindow(Schema *schema): MulticausticWindow(new MultirangeCausticFunction(schema))
{
    _beamShape = new BeamShapeExtension(this);

    _plot->addTextVarY(QStringLiteral("{func_mode}"), tr("Function mode"), [this]{
        return CausticFunction::modeDisplayName(function()->mode()); });

    _plot->setDefaultTitleY(QStringLiteral("{func_mode} {(unit)}"));
    _plot->setFormatterTextY(QStringLiteral("{func_mode} {(unit)}"));
}

QWidget* MultirangeCausticWindow::makeOptionsPanel()
{
    return new CausticOptionsPanel<MultirangeCausticWindow>(this);
}

QString MultirangeCausticWindow::readFunction(const QJsonObject& root)
{
    QString res = MulticausticWindow::readFunction(root);
    if (!res.isEmpty()) return res;

    function()->setMode(Z::IO::Utils::enumFromStr(
        root["mode"].toString(), CausticFunction::BeamRadius));
    return QString();
}

QString MultirangeCausticWindow::writeFunction(QJsonObject& root)
{
    MulticausticWindow::writeFunction(root);
    root["mode"] = Z::IO::Utils::enumToStr(function()->mode());
    return QString();
}

QString MultirangeCausticWindow::getCursorInfo(const QPointF& pos) const
{
    if (!function()->ok()) return QString();
    double x = getUnitX()->toSi(pos.x());
    auto res = function()->calculateAt(x);
    _beamShape->setShape(res);
    auto unitY = getUnitY();
    return QString("%1t = %2; %1s = %3").arg(
                CausticFunction::modeAlias(function()->mode()),
                Z::format(unitY->fromSi(res.T)),
                Z::format(unitY->fromSi(res.S)));
}
