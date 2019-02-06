#include "CausticWindow.h"

#include "CausticOptionsPanel.h"
#include "../io/CommonUtils.h"
#include "../io/JsonUtils.h"
#include "../VariableDialog.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriLayouts.h"

#include <QBoxLayout>
#include <QLabel>

CausticWindow::CausticWindow(Schema *schema) : PlotFuncWindowStorable(new CausticFunction(schema))
{
}

bool CausticWindow::configureInternal()
{
    return VariableDialog::ElementRangeDlg(schema(), function()->arg(), tr("Range"), "func_caustic").run();
}

QWidget* CausticWindow::makeOptionsPanel()
{
    return new CausticOptionsPanel<CausticWindow>(this);
}

QString CausticWindow::readFunction(const QJsonObject& root)
{
    function()->setMode(Z::IO::Utils::enumFromStr(
        root["mode"].toString(), CausticFunction::BeamRadius));
    auto res = Z::IO::Json::readVariable(root["arg"].toObject(), function()->arg(), schema());
    if (!res.isEmpty())
        return res;
    return QString();
}

QString CausticWindow::writeFunction(QJsonObject& root)
{
    root["mode"] = Z::IO::Utils::enumToStr(function()->mode());
    root["arg"] = Z::IO::Json::writeVariable(function()->arg(), schema());
    return QString();
}

QString CausticWindow::getDefaultTitle() const
{
    switch (function()->mode())
    {
    case CausticFunction::Mode::BeamRadius:
        return tr("Beam Radius");
    case CausticFunction::Mode::FontRadius:
        return tr("Wavefront Curvature Radius");
    case CausticFunction::Mode::HalfAngle:
        return tr("Half of Divergence Angle");
    }
    return QString();
}

QString CausticWindow::getDefaultTitleX() const
{
    auto elem = function()->arg()->element;
    return QStringLiteral("%1 (%2)").arg(elem->displayLabelTitle(), getUnitX()->name());
}

QString CausticWindow::getDefaultTitleY() const
{
    QString title;
    switch (function()->mode())
    {
    case CausticFunction::Mode::BeamRadius:
        title = tr("Beam radius");
        break;
    case CausticFunction::Mode::FontRadius:
        title = tr("Wavefront curvature radius");
        break;
    case CausticFunction::Mode::HalfAngle:
        title = tr("Half of divergence angle");
        break;
    }
    return QStringLiteral("%1 (%2)").arg(title, getUnitY()->name());
}
