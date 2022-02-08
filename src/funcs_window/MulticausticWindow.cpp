#include "MulticausticWindow.h"

#include "InfoFuncWindow.h"
#include "MultiCausticParamsDlg.h"
#include "../funcs/FunctionGraph.h"
#include "../funcs/PlotFuncRoundTripFunction.h"
#include "../io/JsonUtils.h"

#include "qcpl_cursor.h"
#include "qcpl_plot.h"

MulticausticWindow::MulticausticWindow(MultirangeCausticFunction* function) : PlotFuncWindowStorable(function)
{
    _plot->addLayer("elem_bounds", _plot->layer("graphs"), QCustomPlot::limBelow);

    _plot->addTextVarX(QStringLiteral("{elems}"), tr("Element labels and titles"), [this]{
        QStringList strs;
        foreach (const auto& arg, this->function()->args())
            if (!arg.element->disabled())
                strs << arg.element->displayLabelTitle();
        return strs.join(QStringLiteral(", "));
    });
    _plot->addTextVarX(QStringLiteral("{elem_labels}"), tr("Element labels"), [this]{
        QStringList strs;
        foreach (const auto& arg, this->function()->args())
            if (!arg.element->disabled())
                strs << arg.element->displayLabel();
        return strs.join(QStringLiteral(", "));
    });
    _plot->addTextVarX(QStringLiteral("{elem_titles}"), tr("Element titles"), [this]{
        QStringList strs;
        foreach (const auto& arg, this->function()->args())
            if (!arg.element->disabled())
                strs << arg.element->displayTitle();
        return strs.join(QStringLiteral(", "));
    });
    _plot->setDefaultTitleX(QStringLiteral("{elem_labels} {(unit)}"));
    _plot->setFormatterTextX(QStringLiteral("{elem_labels} {(unit)}"));

    _actnElemBoundMarkers = new QAction(tr("Element bound markers"), this);
    _actnElemBoundMarkers->setCheckable(true);
    _actnElemBoundMarkers->setChecked(true);
    connect(_actnElemBoundMarkers, &QAction::toggled, this, &MulticausticWindow::toggleElementBoundMarkers);
}

bool MulticausticWindow::configureInternal()
{
    MultiCausticParamsDlg dlg(schema(), function()->args());
    if (dlg.run())
    {
        function()->setArgs(dlg.result());
        return true;
    }
    return false;
}

void MulticausticWindow::toggleElementBoundMarkers(bool on)
{
    foreach (auto marker, _elemBoundMarkers)
        marker->setVisible(on);
    plot()->replot();
    schema()->events().raise(SchemaEvents::Changed, "MultirangeCausticWindow: toggleElementBoundMarkers");
}

QCPItemStraightLine* MulticausticWindow::makeElemBoundMarker() const
{
    QCPItemStraightLine *line = new QCPItemStraightLine(plot());
    line->setPen(QPen(Qt::magenta, 1, Qt::DashLine)); // TODO make configurable
    line->setSelectable(false);
    line->setLayer("elem_bounds");
    return line;
}

void MulticausticWindow::updateElementBoundMarkers()
{
    auto unitX = getUnitX();
    double offset = 0;
    QList<QCPItemStraightLine*> markers;
    auto funcs = function()->funcs();
    for (int i = 0; i < funcs.size()-1; i++)
    {
        auto arg = funcs.at(i)->arg();
        if (arg->element->disabled())
            continue;
        offset += arg->range.stop.toSi();
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
    foreach (auto oldMarker, _elemBoundMarkers)
        plot()->removeItem(oldMarker);
    _elemBoundMarkers = markers;
}

void MulticausticWindow::afterUpdate()
{
    updateElementBoundMarkers();
}

void MulticausticWindow::fillViewMenuActions(QList<QAction*>& actions) const
{
    actions << _actnElemBoundMarkers;
}

ElemDeletionReaction MulticausticWindow::reactElemDeletion(const Elements& elems)
{
    int deletingArgsCount = 0;
    foreach (const Z::Variable& arg, function()->args())
    {
        if (elems.contains(arg.element))
            deletingArgsCount++;
    }
    if (deletingArgsCount == function()->args().size())
        return ElemDeletionReaction::Close;
    return ElemDeletionReaction::None;
}

QString MulticausticWindow::readFunction(const QJsonObject& root)
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

QString MulticausticWindow::writeFunction(QJsonObject& root)
{
    QJsonArray argsJson;
    foreach (const Z::Variable& arg, function()->args())
        argsJson.append(Z::IO::Json::writeVariable(&arg, schema()));
    root["args"] = argsJson;
    return QString();
}

QString MulticausticWindow::readWindowSpecific(const QJsonObject& root)
{
    _actnElemBoundMarkers->setChecked(root["elem_bound_markers"].toBool(true));
    return QString();
}

QString MulticausticWindow::writeWindowSpecific(QJsonObject& root)
{
    root["elem_bound_markers"] = _actnElemBoundMarkers->isChecked();
    return QString();
}

void MulticausticWindow::updateGraphs()
{
    QList<PlotFunction*> funcs;
    for (auto func : function()->funcs())
        if (!func->arg()->element->disabled())
            funcs << func;
    _graphs->update(funcs);
}

void MulticausticWindow::schemaRebuilt(Schema* schema)
{
    // We only have to ensure all arguments are in the same order as schema elements.
    // Don't recalculate here, recalculation will be done later on the intended event.
    QHash<Element*, Z::Variable> oldArgs;
    foreach (auto& arg, function()->args())
        oldArgs.insert(arg.element, arg);
    QVector<Z::Variable> newArgs;
    for (auto elem : schema->elements())
        if (oldArgs.contains(elem))
            newArgs.append(oldArgs[elem]);
    function()->setArgs(newArgs);
}

void MulticausticWindow::elementChanged(Schema*, Element* elem)
{
    // Only modify the set of arguments, don't recalculate here,
    // recalculation will be done later on the intended event.
    auto args = function()->args();
    for (int i = 0; i < args.size(); i++)
    {
        auto& arg = args[i];
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
}

void MulticausticWindow::elementDeleting(Schema*, Element* elem)
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

void MulticausticWindow::showRoundTrip()
{
    auto unitX = getUnitX();
    auto currentX = unitX->toSi(_cursor->position().x());
    double prevOffset = 0;
    foreach (auto func, function()->funcs())
    {
        if (func->arg()->element->disabled())
            continue;

        double nextOffset = prevOffset + func->arg()->range.stop.toSi();
        if (currentX >= prevOffset && currentX < nextOffset)
        {
            QString funcTitle = QString("%1 (inside of %2)").arg(windowTitle(), func->arg()->element->displayLabel());
            InfoFuncWindow::open(new PlotFuncRoundTripFunction(funcTitle, func), this);
            return;
        }
        prevOffset = nextOffset;
    }
}

Z::Unit MulticausticWindow::getDefaultUnitX() const
{
    const auto& funcs = function()->funcs();
    return funcs.isEmpty() ? Z::Units::none() : funcs.first()->arg()->parameter->value().unit();
}

Z::Unit MulticausticWindow::getDefaultUnitY() const
{
    switch (function()->mode())
    {
    case CausticFunction::BeamRadius: return AppSettings::instance().defaultUnitBeamRadius;
    case CausticFunction::FrontRadius: return AppSettings::instance().defaultUnitFrontRadius;
    case CausticFunction::HalfAngle: return AppSettings::instance().defaultUnitAngle;
    }
    return Z::Units::none();
}

void MulticausticWindow::storeView(FuncMode mode)
{
    ViewSettings vs;
    storeViewParts(vs, VP_LIMITS_Y | VP_TITLE_Y | VP_UNIT_Y | VP_CUSRSOR_POS);
    _storedView[mode] = vs;
}

void MulticausticWindow::restoreView(FuncMode mode)
{
    ViewSettings vs;
    if (_storedView.contains(mode))
        vs = _storedView[mode];
    restoreViewParts(vs, VP_LIMITS_Y | VP_TITLE_Y | VP_UNIT_Y | VP_CUSRSOR_POS);
}
