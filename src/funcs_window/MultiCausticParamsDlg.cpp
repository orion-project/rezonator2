#include "MultiCausticParamsDlg.h"

#include "../widgets/ElemSelectorWidget.h"
#include "../widgets/VariableRangeEditor.h"

#include "helpers/OriWidgets.h"
#include "helpers/OriDialogs.h"

#include <QCheckBox>
#include <QGroupBox>

#include <memory>

MultiCausticParamsDlg::MultiCausticParamsDlg(Schema *schema, const QVector<Z::Variable>& vars)
    : RezonatorDialog(DontDeleteOnClose)
{
    setWindowTitle(tr("Ranges"));
    setObjectName("MultiCausticParamsDlg");

    std::shared_ptr<ElementFilter> elemFilter(
        ElementFilter::make<ElementFilterIsRange>());

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
