#include "MultiCausticWindow.h"

#include "CausticOptionsPanel.h"
#include "../funcs/CausticFunction.h"
#include "../funcs/FunctionGraph.h"
#include "../io/CommonUtils.h"
#include "../io/JsonUtils.h"
#include "../widgets/Plot.h"
#include "../widgets/ElemSelectorWidget.h"
#include "../widgets/VariableRangeEditor.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"

#include <QAction>
#include <QCheckBox>
#include <QDebug>
#include <QGroupBox>

//------------------------------------------------------------------------------
//                              MultiCausticParamsDlg
//------------------------------------------------------------------------------

MultiCausticParamsDlg::MultiCausticParamsDlg(Schema *schema, const QVector<Z::Variable>& vars)
    : RezonatorDialog(DontDeleteOnClose)
{
    setWindowTitle(tr("Ranges"));
    setObjectName("MultiCausticParamsDlg");

    std::shared_ptr<ElementFilter> elemFilter(
        ElementFilter::make<ElementFilterIsRange, ElementFilterEnabled>());

    _elemsSelector = new MultiElementSelectorWidget(schema, elemFilter.get());
    connect(_elemsSelector, &MultiElementSelectorWidget::currentElementChanged,
            this, &MultiCausticParamsDlg::currentElementChanged);

    _sameSettings = new QCheckBox(tr("Use these settings for all elements"));

    _rangeEditor = new PointsRangeEditor;
    _rangeEditor->addWidget(_sameSettings, _rangeEditor->rowCount()+1, 0, 1, _rangeEditor->columnCount());

    mainLayout()->addWidget(_elemsSelector);
    mainLayout()->addWidget(Ori::Gui::group(tr("Plot accuracy"), _rangeEditor));
    mainLayout()->addSpacing(8);

    populate(vars);

    _elemsSelector->setCurrentRow(0);
    _elemsSelector->setFocus();
}

void MultiCausticParamsDlg::populate(const QVector<Z::Variable>& vars)
{
    for (auto elem : _elemsSelector->allElements())
    {
        bool hasRange = false;

        // Use given range for element
        for (const Z::Variable& var : vars)
            if (var.element == elem)
            {
                _elemsSelector->selectElement(elem);
                _elemRanges[elem] = var.range;
                hasRange = true;
                break;
            }

        // Make default range for the element if there is not given one
        if (!hasRange)
        {
            Z::VariableRange range;
            range.stop = Z::Utils::getRangeStop(Z::Utils::asRange(elem));
            range.start = Z::Value(0, range.stop.unit());
            range.step = range.stop / 100.0;
            range.useStep = false;
            range.points = 100;
            _elemRanges[elem] = range;
        }
    }
}

void MultiCausticParamsDlg::collect()
{
    saveEditedRange(_elemsSelector->currentElement());

    auto selectedElems = _elemsSelector->selectedElements();
    if (selectedElems.isEmpty())
    {
        Ori::Dlg::warning(tr("No elements are chosen.\nYou should mark at least one element."));
        _elemsSelector->setFocus();
        return;
    }

    _result.clear();
    for (auto elem : selectedElems)
    {
        Z::Variable var;
        var.element = elem;
        var.parameter = Z::Utils::asRange(elem)->paramLength();
        var.range = _elemRanges[elem];
        _result.append(var);
    }

    accept();
}

void MultiCausticParamsDlg::currentElementChanged(Element *current, Element *previous)
{
    saveEditedRange(previous);

    if (_elemRanges.contains(current))
        _rangeEditor->setRange(_elemRanges[current]);
}

void MultiCausticParamsDlg::saveEditedRange(Element *elem)
{
    auto newRange = _rangeEditor->range();
    if (_sameSettings->isChecked())
        for (auto elem : _elemRanges.keys())
            _elemRanges[elem].assignPoints(newRange);
    else if (_elemRanges.contains(elem))
        _elemRanges[elem].assignPoints(newRange);
}

//------------------------------------------------------------------------------
//                              MultiCausticWindow
//------------------------------------------------------------------------------

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

QWidget* MultiCausticWindow::makeOptionsPanel()
{
    return new CausticOptionsPanel<MultiCausticWindow>(this);
}

bool MultiCausticWindow::configureInternal()
{
    MultiCausticParamsDlg dlg(schema(), function()->args());
    if (dlg.run())
    {
        function()->setArgs(dlg.result());
        return true;
    }
    return false;
}

void MultiCausticWindow::calculate()
{
    if (schema()->isSP())
        function()->setPump(schema()->activePump());

    PlotFuncWindow::calculate();
}

void MultiCausticWindow::updateGraphs()
{
    QList<PlotFunction*> funcs;
    for (auto func : function()->funcs())
        funcs << func;
    _graphs->update(funcs);
}

void MultiCausticWindow::afterUpdate()
{
    updateElementBoundMarkers();
}

void MultiCausticWindow::afterSetUnitsX(Z::Unit old, Z::Unit cur)
{
    Q_UNUSED(old)
    Q_UNUSED(cur)
    updateElementBoundMarkers();
}

void MultiCausticWindow::updateElementBoundMarkers()
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

QCPItemStraightLine* MultiCausticWindow::makeElemBoundMarker() const
{
    QCPItemStraightLine *line = new QCPItemStraightLine(plot());
    line->setPen(QPen(Qt::magenta, 1, Qt::DashLine)); // TODO make configurable
    line->setSelectable(false);
    return line;
}

void MultiCausticWindow::schemaRebuilt(Schema* schema)
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
    function()->setMode(Z::IO::Utils::enumFromStr(
        root["mode"].toString(), CausticFunction::BeamRadius));
    return QString();
}

QString MultiCausticWindow::writeFunction(QJsonObject& root)
{
    QJsonArray argsJson;
    for (const Z::Variable& arg : function()->args())
        argsJson.append(Z::IO::Json::writeVariable(&arg, schema()));
    root["args"] = argsJson;
    root["mode"] = Z::IO::Utils::enumToStr(function()->mode());
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

void MultiCausticWindow::fillViewMenuActions(QList<QAction*>& actions) const
{
    actions << _actnElemBoundMarkers;
}

void MultiCausticWindow::toggleElementBoundMarkers(bool on)
{
    for (auto marker : _elemBoundMarkers)
        marker->setVisible(on);
    plot()->replot();
    schema()->events().raise(SchemaEvents::Changed);
}

QString MultiCausticWindow::getDefaultTitle() const
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

QString MultiCausticWindow::getDefaultTitleX() const
{
    QStringList strs;
    for (auto arg : function()->args())
        strs << arg.element->displayLabel();
    return QStringLiteral("%1 (%2)").arg(strs.join(QStringLiteral(", ")), getUnitX()->name());
}

QString MultiCausticWindow::getDefaultTitleY() const
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
