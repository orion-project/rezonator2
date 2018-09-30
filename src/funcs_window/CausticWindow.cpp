#include "CausticWindow.h"

#include "CausticOptionsPanel.h"
#include "../io/z_io_utils.h"
#include "../io/z_io_json.h"
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
