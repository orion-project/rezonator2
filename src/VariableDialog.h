#ifndef VARIABLE_DLG_H
#define VARIABLE_DLG_H

#include "RezonatorDialog.h"

class Schema;
class VariableEditor;
class VariableEditor_ElementRange;
class VariableEditor_MultiElementRange;
namespace Z {
struct Variable;
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

//------------------------------------------------------------------------------
/**
    The function argument dialog that can choose one of parameters
    and set variation of this parameter and number of points for plotting.
*/
class ElementDlg : public VariableDlg
{
    Q_OBJECT

public:
    explicit ElementDlg(Schema*, Z::Variable*, const QString& title, const QString& recentKey);

private:
    Z::Variable* _var;
    VariableEditor* _varEditor;

protected slots:
    void collect();
};

//------------------------------------------------------------------------------
/**
    The function arguments dialog that can choose two different parameters
    and set variation of each parameter and number of points for plotting.
*/
class TwoElemensDlg : public VariableDlg
{
    Q_OBJECT

public:
    explicit TwoElemensDlg(Schema*, Z::Variable*, Z::Variable*, const QString& title, const QString& recentKey);

private:
    Z::Variable *_var1, *_var2;
    VariableEditor *_varEditor1, *_varEditor2;

protected slots:
    void collect();
};

//------------------------------------------------------------------------------
/**
    The function arguments dialog that can choose one of range elements
    and set number of points for plotting inside selected element.
*/
class ElementRangeDlg : public VariableDlg
{
    Q_OBJECT

public:
    explicit ElementRangeDlg(Schema*, Z::Variable*, const QString& title, const QString& recentKey);

private:
    Z::Variable* _var;
    VariableEditor_ElementRange* _varEditor;

protected slots:
    void collect();
};

//------------------------------------------------------------------------------
/**
    The function arguments dialog that can choose several of range elements
    and set number of points for plotting inside each of selected elements.
*/
class MultiElementRangeDlg : public VariableDlg
{
    Q_OBJECT

public:
    explicit MultiElementRangeDlg(Schema*/*, Z::Variable**/, const QString& title, const QString& recentKey);

private:
    //Z::Variable* _var;
    VariableEditor_MultiElementRange *_varEditor;

protected slots:
    void collect();
};

} // namespace VariableDialog

#endif // VARIABLE_DLG_H
