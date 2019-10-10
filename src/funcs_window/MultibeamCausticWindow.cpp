#include "MultibeamCausticWindow.h"

#include "MultiCausticParamsDlg.h"
#include "../funcs/FunctionGraph.h"
#include "../io/JsonUtils.h"
#include "../widgets/Plot.h"

#include <QAction>
#include <QDebug>

MultibeamCausticWindow::MultibeamCausticWindow(Schema *schema) : PlotFuncWindowStorable(new MultibeamCausticFunction(schema))
{
    createActions();
}

void MultibeamCausticWindow::createActions()
{
    _actnElemBoundMarkers = new QAction(tr("Element bound markers"), this);
    _actnElemBoundMarkers->setCheckable(true);
    _actnElemBoundMarkers->setChecked(true);
    connect(_actnElemBoundMarkers, &QAction::toggled, this, &MultibeamCausticWindow::toggleElementBoundMarkers);
}

bool MultibeamCausticWindow::configureInternal()
{
    MultiCausticParamsDlg dlg(schema(), function()->args());
    if (dlg.run())
    {
        function()->setArgs(dlg.result());
        return true;
    }
    return false;
}

void MultibeamCausticWindow::updateGraphs()
{
    QList<PlotFunction*> funcs;
    for (auto func : function()->funcs())
        funcs << func;
    _graphs->update(funcs);
}

void MultibeamCausticWindow::afterUpdate()
{
    updateElementBoundMarkers();
}

void MultibeamCausticWindow::updateElementBoundMarkers()
{
    auto unitX = getUnitX();
    double offset = 0;
    QList<QCPItemStraightLine*> markers;
    auto funcs = function()->funcs();
    for (int i = 0; i < funcs.size()-1; i++)
    {
        offset += funcs.at(i)->arg()->range.stop.toSi();
        QCPItemStraightLine* marker;
        if (!_elemBoundMarkers.isEmpty())
        {
            marker = _elemBoundMarkers.first();
            _elemBoundMarkers.removeFirst();
        }
        else marker = makeElemBoundMarker();
        double x = unitX->fromSi(offset);
        marker->point1->setCoords(x, 0);
        marker->point2->setCoords(x, 1);
        marker->setVisible(_actnElemBoundMarkers->isChecked());
        markers.append(marker);
    }
    for (auto oldMarker : _elemBoundMarkers)
        plot()->removeItem(oldMarker);
    _elemBoundMarkers = markers;
}

QCPItemStraightLine* MultibeamCausticWindow::makeElemBoundMarker() const
{
    QCPItemStraightLine *line = new QCPItemStraightLine(plot());
    line->setPen(QPen(Qt::magenta, 1, Qt::DashLine)); // TODO make configurable
    line->setSelectable(false);
    return line;
}

void MultibeamCausticWindow::schemaRebuilt(Schema* schema)
{
    // We only have to ensure all arguments are in the same order as schema elements.
    // Don't recalculate here, recalculation will be done later on the intended event.
    QMap<Element*, Z::Variable> oldArgs;
    for (auto arg : function()->args())
        oldArgs.insert(arg.element, arg);
    QVector<Z::Variable> newArgs;
    for (auto elem : schema->elements())
        if (oldArgs.contains(elem))
            newArgs.append(oldArgs[elem]);
    function()->setArgs(newArgs);
}

void MultibeamCausticWindow::elementChanged(Schema*, Element* elem)
{
    // Only modify the set of arguments, don't recalculate here,
    // recalculation will be done later on the intended event.
    auto args = function()->args();
    for (Z::Variable& arg : args)
        if (arg.element == elem)
        {
            auto newStop = Z::Utils::getRangeStop(Z::Utils::asRange(elem));
            if (newStop != arg.range.stop)
            {
                arg.range.stop = newStop;
                function()->setArgs(args);
                return;
            }
        }
}

void MultibeamCausticWindow::elementDeleting(Schema*, Element* elem)
{
    bool needUpdate = false;
    auto args = function()->args();
    for (int i = 0; i < args.size(); i++)
    {
        if (args.at(i).element == elem)
        {
            needUpdate = true;
            args.remove(i);
            break;
        }
    }
    if (needUpdate)
    {
        if (args.empty())
            disableAndClose();
        else
            function()->setArgs(args);
    }
}

QString MultibeamCausticWindow::readFunction(const QJsonObject& root)
{
    QVector<Z::Variable> args;
    QJsonArray argsJson = root["args"].toArray();
    for (auto it = argsJson.begin(); it != argsJson.end(); it++)
    {
        Z::Variable arg;
        auto res = Z::IO::Json::readVariable((*it).toObject(), &arg, schema());
        if (!res.isEmpty())
            return res;
        args.append(arg);
    }
    function()->setArgs(args);
    return QString();
}

QString MultibeamCausticWindow::writeFunction(QJsonObject& root)
{
    QJsonArray argsJson;
    for (const Z::Variable& arg : function()->args())
        argsJson.append(Z::IO::Json::writeVariable(&arg, schema()));
    root["args"] = argsJson;
    return QString();
}

QString MultibeamCausticWindow::readWindowSpecific(const QJsonObject& root)
{
    _actnElemBoundMarkers->setChecked(root["elem_bound_markers"].toBool(true));
    return QString();
}

QString MultibeamCausticWindow::writeWindowSpecific(QJsonObject& root)
{
    root["elem_bound_markers"] = _actnElemBoundMarkers->isChecked();
    return QString();
}

ElemDeletionReaction MultibeamCausticWindow::reactElemDeletion(const Elements& elems)
{
    int deletingArgsCount = 0;
    for (const Z::Variable& arg : function()->args())
    {
        if (elems.contains(arg.element))
            deletingArgsCount++;
    }
    if (deletingArgsCount == function()->args().size())
        return ElemDeletionReaction::Close;
    return ElemDeletionReaction::None;
}

void MultibeamCausticWindow::fillViewMenuActions(QList<QAction*>& actions) const
{
    actions << _actnElemBoundMarkers;
}

void MultibeamCausticWindow::toggleElementBoundMarkers(bool on)
{
    for (auto marker : _elemBoundMarkers)
        marker->setVisible(on);
    plot()->replot();
    schema()->events().raise(SchemaEvents::Changed);
}

QString MultibeamCausticWindow::getDefaultTitle() const
{
    return tr("Beam Radius");
}

QString MultibeamCausticWindow::getDefaultTitleX() const
{
    QStringList strs;
    for (auto arg : function()->args())
        strs << arg.element->displayLabel();
    return QStringLiteral("%1 (%2)").arg(strs.join(QStringLiteral(", ")), getUnitX()->name());
}

QString MultibeamCausticWindow::getDefaultTitleY() const
{
    QString title = tr("Beam radius");
    return QStringLiteral("%1 (%2)").arg(title, getUnitY()->name());
}
