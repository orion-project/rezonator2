#ifndef WINDOW_STABILITY_MAP_2D_H
#define WINDOW_STABILITY_MAP_2D_H

#include "PlotFuncWindowStorable.h"
#include "../RezonatorDialog.h"
#include "../funcs/StabilityMap2DFunction.h"

class ElemAndParamSelector;
class GeneralRangeEditor;

QT_BEGIN_NAMESPACE
class QGroupBox;
QT_END_NAMESPACE

class StabilityMap2DWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit StabilityMap2DWindow(Schema*);

    StabilityMap2DFunction* function() const { return dynamic_cast<StabilityMap2DFunction*>(_function); }

protected:
    // Implementation of PlotFuncWindow
    bool configureInternal() override;
    ElemDeletionReaction reactElemDeletion(const Elements&) override;

    // Implementation of SchemaListener
    void elementDeleting(Schema*, Element*) override;
};


/**
    The function arguments dialog that can choose two different parameters
    and set variation of each parameter and the number of points for plotting.
*/
class StabilityMap2DParamsDlg : public RezonatorDialog
{
    Q_OBJECT

    struct VarEditor {
        Z::Variable *var;
        ElemAndParamSelector *elemSelector;
        GeneralRangeEditor *rangeEditor;
        QGroupBox *groupBox;
    };

public:
    explicit StabilityMap2DParamsDlg(Schema*, Z::Variable*, Z::Variable*);

protected slots:
    void collect() override;

private:
    VarEditor _editor1, _editor2;
    QString _recentKey = "func_stab_map_2d";

    void makeControls(const QString& title, Schema *schema, VarEditor* editor);
    void guessRange(VarEditor* editor);
    void populate(VarEditor* editor);
    void collect(VarEditor* editor, Z::Variable *var);
};

#endif // WINDOW_STABILITY_MAP_2D_H
