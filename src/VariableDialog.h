#ifndef VARIABLE_DLG_H
#define VARIABLE_DLG_H

#include "RezonatorDialog.h"
#include "widgets/WidgetResult.h"

class Schema;
class ElemSelectorWidget;
class ElemAndParamSelector;
namespace Z {
struct Variable;
}
namespace VariableEditor {
class ElementEd;
class ElementRangeEd;
class MultiElementRangeEd;
}
namespace VariableRangeEditor {
    class GeneralRangeEd;
    class PointsRangeEd;
}

namespace VariableDialog {

/**
    The base class for function argument dialogs.
*/
class VariableDlg : public RezonatorDialog
{
public:
    bool run();

protected:
    VariableDlg(const QString& windowTitle, const QString& recentKey);

    QString _recentKey;
};

/**
    The function arguments dialog that can choose two different parameters
    and set variation of each parameter and the number of points for plotting.
*//*
class TwoElemensDlg : public VariableDlg
{
    Q_OBJECT

public:
    explicit TwoElemensDlg(Schema*, Z::Variable*, Z::Variable*, const QString& title, const QString& recentKey);

private:
    Z::Variable *_var1, *_var2;
    VariableEditor::ElementEd *_varEditor1, *_varEditor2;

protected slots:
    void collect();
};
*/
} // namespace VariableDialog

#endif // VARIABLE_DLG_H
