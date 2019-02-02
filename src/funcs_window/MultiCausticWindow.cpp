#include "MultiCausticWindow.h"

#include "CausticOptionsPanel.h"
#include "../VariableDialog.h"
#include "../funcs/CausticFunction.h"
#include "../io/CommonUtils.h"
#include "../io/JsonUtils.h"
#include "../widgets/Plot.h"

#include <QDebug>

MultiCausticWindow::MultiCausticWindow(Schema *schema): PlotFuncWindowStorable(new MultiCausticFunction(schema))
{
    createActions();
}

void MultiCausticWindow::createActions()
{
    _actnElemBoundMarkers = new QAction(tr("Element bound markers"), this);
    _actnElemBoundMarkers->setCheckable(true);
    _actnElemBoundMarkers->setChecked(true);
    connect(_actnElemBoundMarkers, &QAction::toggled, this, &MultiCausticWindow::toggleElementBoundMarkers);
}

bool MultiCausticWindow::configureInternal()
{
    auto args = function()->args();
    bool ok = VariableDialog::MultiElementRangeDlg(schema(), args, tr("Ranges"), "func_multi_caustic").run();
    if (ok)
        function()->setArgs(args);
    return ok;
}

QWidget* MultiCausticWindow::makeOptionsPanel()
{
    return new CausticOptionsPanel<MultiCausticWindow>(this);
}

void MultiCausticWindow::fillGraphWithFunctionResults(Z::WorkPlane plane, Graph *graph, int resultIndex)
{
    int resultIndex1 = 0;
    double offset = 0;
    for (CausticFunction* func : function()->funcs())
    {
        // Offset result for current element to place it after all previous elements
        int resultIndex2 = resultIndex1 + func->resultCount(plane);
        if (resultIndex >= resultIndex1 && resultIndex < resultIndex2)
        {
            auto result = func->result(plane, resultIndex - resultIndex1);
            graph->clearData();
            for (int i = 0; i < result.x.size(); i++)
                graph->addData(result.x.at(i) + offset, result.y.at(i));
            return;
        }
        resultIndex1 = resultIndex2;
        offset += func->argumentUnit()->fromSi(func->arg()->range.stop.toSi());
    }
}

void MultiCausticWindow::afterUpdate()
{
    updateElementBoundMarkers();
}

void MultiCausticWindow::updateElementBoundMarkers()
{
    double offset = 0;
    QList<QCPItemStraightLine*> markers;
    auto funcs = function()->funcs();
    for (int i = 0; i < funcs.size()-1; i++)
    {
        offset += funcs.at(i)->argumentUnit()->fromSi(funcs.at(i)->arg()->range.stop.toSi());
        QCPItemStraightLine* marker;
        if (!_elemBoundMarkers.isEmpty())
        {
            marker = _elemBoundMarkers.first();
            _elemBoundMarkers.removeFirst();
        }
        else marker = makeElemBoundMarker();
        marker->point1->setCoords(offset, 0);
        marker->point2->setCoords(offset, 1);
        marker->setVisible(_actnElemBoundMarkers->isChecked());
        markers.append(marker);
    }
    for (auto oldMarker : _elemBoundMarkers)
        plot()->removeItem(oldMarker);
    _elemBoundMarkers = markers;
}

QCPItemStraightLine* MultiCausticWindow::makeElemBoundMarker() const
{
    QCPItemStraightLine *line = new QCPItemStraightLine(plot());
    line->setPen(QPen(Qt::magenta, 1, Qt::DashLine)); // TODO make configurable
    line->setSelectable(false);
    plot()->addItem(line);
    return line;
}

void MultiCausticWindow::elementChanged(Schema*, Element* elem)
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

void MultiCausticWindow::elementDeleting(Schema*, Element* elem)
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

QString MultiCausticWindow::readFunction(const QJsonObject& root)
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

QString MultiCausticWindow::writeFunction(QJsonObject& root)
{
    QJsonArray argsJson;
    for (const Z::Variable& arg : function()->args())
        argsJson.append(Z::IO::Json::writeVariable(&arg, schema()));
    root["args"] = argsJson;
    return QString();
}

QString MultiCausticWindow::readWindowSpecific(const QJsonObject& root)
{
    _actnElemBoundMarkers->setChecked(root["elem_bound_markers"].toBool(true));
    return QString();
}

QString MultiCausticWindow::writeWindowSpecific(QJsonObject& root)
{
    root["elem_bound_markers"] = _actnElemBoundMarkers->isChecked();
    return QString();
}

ElemDeletionReaction MultiCausticWindow::reactElemDeletion(const Elements& elems)
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

QList<QAction*> MultiCausticWindow::viewActions()
{
    return {_actnElemBoundMarkers};
}

void MultiCausticWindow::toggleElementBoundMarkers(bool on)
{
    for (auto marker : _elemBoundMarkers)
        marker->setVisible(on);
    plot()->replot();
    schema()->events().raise(SchemaEvents::Changed);
}
