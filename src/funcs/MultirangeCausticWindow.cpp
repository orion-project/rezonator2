#include "MultirangeCausticWindow.h"

#include "../funcs/BeamShapeExtension.h"
#include "../funcs/CausticOptionsPanel.h"
#include "../io/CommonUtils.h"
#include "../math/CausticFunction.h"

#include "qcpl_plot.h"

#include <QDebug>
#include <QJsonObject>

MultirangeCausticWindow::MultirangeCausticWindow(Schema *schema): MulticausticWindow(new MultirangeCausticFunction(schema))
{
    _beamShape = new BeamShapeExtension(this);

    _plot->addTextVarY("{func_mode}", tr("Function mode"), [this]{
        return CausticFunction::modeDisplayName(function()->mode()); });

    _plot->setDefaultTextY("{func_mode} {(unit)}");
    _plot->setFormatterTextY(_plot->defaultTextY());
}

QList<BasicMdiChild::MenuItem> MultirangeCausticWindow::viewMenuItems() const
{
    return MulticausticWindow::viewMenuItems() << _beamShape->actionToggle();
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

void MultirangeCausticWindow::getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values)
{
    if (!function()->ok()) return;
    auto res = function()->calculateAt(pos.X);
    _beamShape->setShape(res);
    QString valueName = function()->valueSymbol();
    values << CursorInfoValue(valueName+'t', res.T);
    values << CursorInfoValue(valueName+'s', res.S);
    MulticausticWindow::getCursorInfo(pos, values);
}
