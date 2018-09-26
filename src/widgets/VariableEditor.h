#ifndef VARIABLE_EDITOR_H
#define VARIABLE_EDITOR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QVector>

#include "WidgetResult.h"
#include "../core/Variable.h"

#include <memory>

class ElemAndParamSelector;
class ElemSelectorWidget;
class ElementFilter;
class Schema;

namespace VariableRangeEditor {
class GeneralRangeEd;
class PointsRangeEd;
}

QT_BEGIN_NAMESPACE
class QCheckBox;
class QListWidget;
class QListWidgetItem;
QT_END_NAMESPACE

namespace VariableEditor {

/**
    The variable editor that can choose one of schema elements' parameters
    and set variation of this parameter and the number of points for plotting.
*/
class ElementEd : public QVBoxLayout
{
    Q_OBJECT

public:
    explicit ElementEd(Schema *schema);

    WidgetResult verify();
    void populate(Z::Variable *var);
    void collect(Z::Variable *var);

private slots:
    void guessRange();

private:
    ElemAndParamSelector* _elemSelector;
    VariableRangeEditor::GeneralRangeEd* _rangeEditor;
    std::shared_ptr<ElementFilter> _elemFilter;
};

//------------------------------------------------------------------------------
/**
    The variable editor that can choose one of range elements
    and set the number of points for plotting inside the selected element.
*/
class ElementRangeEd : public QVBoxLayout
{
    Q_OBJECT

public:
    explicit ElementRangeEd(Schema *schema);

    WidgetResult verify();
    void populate(Z::Variable *var);
    void collect(Z::Variable *var);

private slots:
    void guessRange();

private:
    ElemSelectorWidget* _elemSelector;
    VariableRangeEditor::PointsRangeEd* _rangeEditor;
    std::shared_ptr<ElementFilter> _elemFilter;
};

//------------------------------------------------------------------------------
/**
    The variable editor that can choose several of range elements
    and set the number of points for plotting inside each of selected elements.
*/
class MultiElementRangeEd : public QWidget
{
    Q_OBJECT

public:
    explicit MultiElementRangeEd(Schema *schema);
    ~MultiElementRangeEd();

    WidgetResult verify();
    void populateVars(const QVector<Z::Variable> &vars);
    QVector<Z::Variable> collectVars();

private slots:
    void selectAllElements();
    void deselectAllElements();
    void invertElementsSelection();

private:
    QListWidget *_elemsSelector;
    VariableRangeEditor::PointsRangeEd* _rangeEditor;
    QVector<struct ElemItemData*> _itemsData;
    QCheckBox *_sameSettings;

    void currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void invertCheckState(QListWidgetItem *item);
    void showRangeInEditor(QListWidgetItem *item);
    void saveEditedRange(QListWidgetItem *item);
};

} // namespace VariableEditor

#endif // VARIABLE_EDITOR_H
