#include "MulticausticWindow.h"

#include "../core/Format.h"
#include "../funcs/InfoFuncWindow.h"
#include "../funcs/MultiCausticParamsDlg.h"
#include "../io/JsonUtils.h"
#include "../math/FunctionGraph.h"
#include "../math/PlotFuncRoundTripFunction.h"
#include "../widgets/PlotHelpers.h"

#include "helpers/OriDialogs.h"
#include "widgets/OriValueEdit.h"

#include "qcpl_cursor.h"
#include "qcpl_cursor_panel.h"
#include "qcpl_io_json.h"
#include "qcpl_plot.h"

MulticausticWindow::MulticausticWindow(MultirangeCausticFunction* function) : PlotFuncWindowStorable(function)
{
    _plot->addLayer("elem_bounds", _plot->layer("main"), QCustomPlot::limBelow);

    _plot->addTextVarX("{elems}", tr("Element labels and titles"), [this]{
        QStringList strs;
        foreach (const auto& arg, this->function()->args())
            if (!arg.element->disabled())
                strs << arg.element->displayLabelTitle();
        return strs.join(QStringLiteral(", "));
    });
    _plot->addTextVarX("{elem_labels}", tr("Element labels"), [this]{
        QStringList strs;
        foreach (const auto& arg, this->function()->args())
            if (!arg.element->disabled())
                strs << arg.element->displayLabel();
        return strs.join(QStringLiteral(", "));
    });
    _plot->addTextVarX("{elem_titles}", tr("Element titles"), [this]{
        QStringList strs;
        foreach (const auto& arg, this->function()->args())
            if (!arg.element->disabled())
                strs << arg.element->displayTitle();
        return strs.join(QStringLiteral(", "));
    });
    _plot->setDefaultTextX("{elem_labels} {(unit)}");
    _plot->setFormatterTextX(_plot->defaultTextX());

    _actnElemBoundMarkers = new QAction(tr("Element Edge Markers"), this);
    _actnElemBoundMarkers->setCheckable(true);
    _actnElemBoundMarkers->setChecked(true);
    connect(_actnElemBoundMarkers, &QAction::toggled, this, &MulticausticWindow::toggleElementBoundMarkers);

    _actnElemBoundMarkersFormat = new QAction(tr("Element Edge Markers Format..."), this);
    connect(_actnElemBoundMarkersFormat, &QAction::triggered, this, &MulticausticWindow::formatElementBoundMarkers);

    connect(_cursorPanel, &QCPL::CursorPanel::customCommandInvoked,
            this, &MulticausticWindow::handleCursorPanelCommand);
}

bool MulticausticWindow::configureInternal()
{
    MultiCausticParamsDlg dlg(schema(), function()->args(), function()->helpTopic());
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
    schema()->markModified("MulticausticWindow::toggleElementBoundMarkers");
}

void MulticausticWindow::formatElementBoundMarkers()
{
    PlotHelpers::FormatPenDlgProps props;
    props.title = tr("Element Edge Markers Format");
    props.onApply = [this](const QPen& pen){
        _elemBoundMarkersPen = pen;
        foreach (auto line, _elemBoundMarkers)
            line->setPen(pen);
        _plot->replot();
    };
    props.onReset = [this](){
        _elemBoundMarkersPen.reset();
        foreach (auto line, _elemBoundMarkers)
            line->setPen(elemBoundMarkersPen());
        _plot->replot();
    };
    if (PlotHelpers::formatPenDlg(elemBoundMarkersPen(), props))
        schema()->markModified("MulticausticWindow::formatElementBoundMarkers");
}

QPen MulticausticWindow::elemBoundMarkersPen() const
{
    return _elemBoundMarkersPen ? *_elemBoundMarkersPen : AppSettings::instance().pen(AppSettings::PenElemBound);
}

QCPItemStraightLine* MulticausticWindow::makeElemBoundMarker() const
{
    QCPItemStraightLine *line = new QCPItemStraightLine(plot());
    line->setPen(elemBoundMarkersPen());
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

QList<BasicMdiChild::MenuItem> MulticausticWindow::viewMenuItems() const
{
    return { _actnElemBoundMarkers };
}

QList<BasicMdiChild::MenuItem> MulticausticWindow::formatMenuItems() const
{
    return { _actnElemBoundMarkersFormat };
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
    if (root.contains("elem_bound_markers_pen"))
        _elemBoundMarkersPen = QCPL::readPen(root["elem_bound_markers_pen"].toObject(),
                                             AppSettings::instance().pen(AppSettings::PenElemBound));
    return QString();
}

QString MulticausticWindow::writeWindowSpecific(QJsonObject& root)
{
    root["elem_bound_markers"] = _actnElemBoundMarkers->isChecked();
    if (_elemBoundMarkersPen)
        root["elem_bound_markers_pen"] = QCPL::writePen(*_elemBoundMarkersPen);
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

MulticausticWindow::FuncOffset MulticausticWindow::findFuncOffset(const Z::Value &x) const
{
    auto currentX = x.toSi();
    double prevOffset = 0;
    foreach (auto func, function()->funcs())
    {
        if (func->arg()->element->disabled())
            continue;
        double nextOffset = prevOffset + func->arg()->range.stop.toSi();
        if (currentX >= prevOffset && currentX < nextOffset)
        {
            return { func, currentX-prevOffset, prevOffset };
        }
        prevOffset = nextOffset;
    }
    return { nullptr, 0, 0 };
}

void MulticausticWindow::showRoundTrip()
{
    auto funcOffset = findFuncOffset(Z::Value(_cursor->position().x(), getUnitX()));
    if (funcOffset.func)
    {
        auto func = funcOffset.func;
        QString funcTitle = QString("%1 (inside of %2)").arg(windowTitle(), func->arg()->element->displayLabel());
        InfoFuncWindow::open(new PlotFuncRoundTripFunction(funcTitle, func), this);
    }
}

Z::Unit MulticausticWindow::getDefaultUnitX() const
{
    const auto& funcs = function()->funcs();
    return funcs.isEmpty() ? Z::Units::none() : funcs.first()->arg()->parameter->value().unit();
}

Z::Unit MulticausticWindow::getDefaultUnitY() const
{
    return getDefaultUnitY(function()->mode());
}

Z::Unit MulticausticWindow::getDefaultUnitY(FuncMode mode) const
{
    switch (mode)
    {
    case CausticFunction::BeamRadius: return AppSettings::instance().defaultUnitBeamRadius;
    case CausticFunction::FrontRadius: return AppSettings::instance().defaultUnitLinear;
    case CausticFunction::HalfAngle: return AppSettings::instance().defaultUnitAngle;
    }
    return Z::Units::none();
}

SpecPointParams MulticausticWindow::getSpecPointsParams() const
{
    return {
        { CausticFunction::spUnitX, SpecPointParam(getUnitX()) },
        { CausticFunction::spUnitW, SpecPointParam(function()->mode() == CausticFunction::BeamRadius ? getUnitY() : getUnitY(CausticFunction::BeamRadius)) },
        { CausticFunction::spUnitR, SpecPointParam(function()->mode() == CausticFunction::FrontRadius ? getUnitY() : getUnitY(CausticFunction::FrontRadius)) },
    };
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

void MulticausticWindow::getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values)
{
    if (!function()->ok()) return;
    auto funcOffset = findFuncOffset(pos.X);
    if (funcOffset.func)
    {
        auto unitX = getUnitX();
        for (int i = 0; i < values.size(); i++)
            if (values.at(i).kind == CursorInfoValue::VALUE_X)
            {
                values[i].note = QStringLiteral("(%1 @ %2)").arg(
                    funcOffset.func->arg()->element->displayLabel(),
                    _cursorPanel->formatLink("offset", Z::format(unitX->fromSi(funcOffset.offset))));
                break;
            }
    }
}

void MulticausticWindow::handleCursorPanelCommand(const QString& cmd)
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

void MulticausticWindow::optionChanged(AppSettingsOption option)
{
    PlotFuncWindow::optionChanged(option);

    if (option == AppSettingsOption::DefaultPenFormat)
    {
        for (auto line: std::as_const(_elemBoundMarkers))
            line->setPen(elemBoundMarkersPen());
        _plot->replot();
    }
}
