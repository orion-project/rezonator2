#include "MultirangeCausticWindow.h"

#include "BeamShapeExtension.h"
#include "CausticOptionsPanel.h"
#include "../core/Format.h"
#include "../funcs/CausticFunction.h"
#include "../io/CommonUtils.h"

#include <QDebug>
#include <QJsonObject>

MultirangeCausticWindow::MultirangeCausticWindow(Schema *schema): MulticausticWindow(new MultirangeCausticFunction(schema))
{
    _beamShape = new BeamShapeExtension(this);
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

QString MultirangeCausticWindow::getDefaultTitle() const
{
    switch (function()->mode())
    {
    case CausticFunction::Mode::BeamRadius:
        return tr("Beam Radius");
    case CausticFunction::Mode::FrontRadius:
        return tr("Wavefront Curvature Radius");
    case CausticFunction::Mode::HalfAngle:
        return tr("Half of Divergence Angle");
    }
    return QString();
}

QString MultirangeCausticWindow::getDefaultTitleY() const
{
    QString title;
    switch (function()->mode())
    {
    case CausticFunction::Mode::BeamRadius:
        title = tr("Beam radius");
        break;
    case CausticFunction::Mode::FrontRadius:
        title = tr("Wavefront curvature radius");
        break;
    case CausticFunction::Mode::HalfAngle:
        title = tr("Half of divergence angle");
        break;
    }
    return QStringLiteral("%1 (%2)").arg(title, getUnitY()->name());
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
