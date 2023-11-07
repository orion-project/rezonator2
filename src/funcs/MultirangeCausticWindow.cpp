#include "MultirangeCausticWindow.h"

#include "../core/Format.h"
#include "../funcs/BeamShapeExtension.h"
#include "../funcs/CausticOptionsPanel.h"
#include "../io/CommonUtils.h"
#include "../math/CausticFunction.h"

#include "widgets/OriValueEdit.h"
#include "helpers/OriDialogs.h"

#include "qcpl_plot.h"
#include "qcpl_cursor.h"
#include "qcpl_cursor_panel.h"

#include <QDebug>
#include <QJsonObject>

MultirangeCausticWindow::MultirangeCausticWindow(Schema *schema): MulticausticWindow(new MultirangeCausticFunction(schema))
{
    _beamShape = new BeamShapeExtension(this);

    _plot->addTextVarY("{func_mode}", tr("Function mode"), [this]{
        return CausticFunction::modeDisplayName(function()->mode()); });

    _plot->setDefaultTextY("{func_mode} {(unit)}");
    _plot->setFormatterTextY(_plot->defaultTextY());

    connect(_cursorPanel, &QCPL::CursorPanel::customCommandInvoked,
            this, &MultirangeCausticWindow::handleCursorPanelCommand);
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

void MultirangeCausticWindow::getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values) const
{
    if (!function()->ok()) return;
    auto unitX = getUnitX();
    auto res = function()->calculateAt(pos.X);
    _beamShape->setShape(res);
    QString valueName = CausticFunction::modeAlias(function()->mode());
    values << CursorInfoValue(valueName+'t', res.T);
    values << CursorInfoValue(valueName+'s', res.S);
    auto funcOffset = findFuncOffset(pos.X);
    if (funcOffset.func)
        for (int i = 0; i < values.size(); i++)
            if (values.at(i).isX())
            {
                values[i].note = QStringLiteral("(%1 @ %2)").arg(
                    funcOffset.func->arg()->element->displayLabel(),
                    _cursorPanel->formatLink("offset", Z::format(unitX->fromSi(funcOffset.offset))));
                break;
            }
}

void MultirangeCausticWindow::handleCursorPanelCommand(const QString& cmd)
{
    if (cmd != "offset") return;
    auto unitX = getUnitX();
    auto p = _cursor->position();
    auto funcOffset = findFuncOffset({p.x(), unitX});
    if (!funcOffset.func) return;
    Ori::Widgets::ValueEdit editor(unitX->fromSi(funcOffset.offset));
    if (Ori::Dlg::Dialog(&editor, false)
            .withHorizontalPrompt(tr("<b>Offset inside %1:</b>").arg(funcOffset.func->arg()->element->displayLabel()))
            .withTitle(tr("Cursor Position"))
            .exec())
        _cursor->setPositionX(unitX->fromSi(funcOffset.position) + editor.value());
}
