#ifndef VARIABLE_EDITOR_H
#define VARIABLE_EDITOR_H

#include <QVBoxLayout>

#include "WidgetResult.h"

class ElemAndParamSelector;
class ElemSelectorWidget;
class Schema;
class VariableRangeWidget;
class VariableRangeWidget_ElementRange;

namespace Z {
    struct Variable;
}

class VariableEditor : public QVBoxLayout
{
    Q_OBJECT

public:
    explicit VariableEditor(Schema *schema);

    WidgetResult verify();
    void populate(Z::Variable *var);
    void collect(Z::Variable *var);

private slots:
    void guessRange();

private:
    ElemAndParamSelector* _elemSelector;
    VariableRangeWidget* _rangeEditor;
};


class VariableEditor_ElementRange : public QVBoxLayout
{
    Q_OBJECT

public:
    explicit VariableEditor_ElementRange(Schema *schema);

    WidgetResult verify();
    void populate(Z::Variable *var);
    void collect(Z::Variable *var);

private slots:
    void guessRange();

private:
    ElemSelectorWidget* _elemSelector;
    VariableRangeWidget_ElementRange* _rangeEditor;
};


#endif // VARIABLE_EDITOR_H
