#include "StabilityMap2DWindow.h"

StabilityMap2DWindow::StabilityMap2DWindow(Schema *schema) :
    PlotFuncWindowStorable(new StabilityMap2DFunction(schema))
{
}

bool StabilityMap2DWindow::configureInternal()
{
//    return VariableDialog::TwoElemensDlg(schema(), function()->paramX(), function()->paramY(),
//        tr("Contour Stability Map Parameters"), "func_stab_map_2d").run();
    return true;
}

ElemDeletionReaction StabilityMap2DWindow::reactElemDeletion(const Elements& elems)
{
    if (elems.contains(function()->paramX()->element) or
        elems.contains(function()->paramY()->element))
        return ElemDeletionReaction::Close;
    return ElemDeletionReaction::None;
}

void StabilityMap2DWindow::elementDeleting(Schema*, Element* elem)
{
    if (function()->paramX()->element == elem or
        function()->paramY()->element == elem)
        disableAndClose();
}
