#ifndef VARIABLE_EDITOR_H
#define VARIABLE_EDITOR_H

#include <QVBoxLayout>
#include <QVector>

#include "WidgetResult.h"
#include "../core/Variable.h"

#include <memory>

class ElemAndParamSelector;
class ElemSelectorWidget;
class ElementFilter;
class Schema;
class VariableRangeWidget;
class VariableRangeWidget_ElementRange;

QT_BEGIN_NAMESPACE
class QListWidget;
QT_END_NAMESPACE


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
    std::shared_ptr<ElementFilter> _elemFilter;
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
    std::shared_ptr<ElementFilter> _elemFilter;
};

namespace VariableEditors {

class MultiElementRangeEd : public QVBoxLayout
{
    Q_OBJECT

public:
    explicit MultiElementRangeEd(Schema *schema);
    ~MultiElementRangeEd();

    WidgetResult verify();
    void populateVars(const QVector<Z::Variable> &vars);
    QVector<Z::Variable> collectVars() const;

private:
    QListWidget *_elemsSelector;
    VariableRangeWidget_ElementRange* _rangeEditor;
    QVector<struct ElemData*> _itemsData;
};

} // namespace VariableEditor

#endif // VARIABLE_EDITOR_H
