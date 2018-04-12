#ifndef PARAM_EDITOR_H
#define PARAM_EDITOR_H

#include <QWidget>

#include "../core/Parameters.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
    class ValueEdit;
}}

class UnitComboBox;

class ParamEditor : public QWidget, public Z::ParameterListener
{
    Q_OBJECT

public:
    explicit ParamEditor(Z::Parameter* param, bool showName = true);
    ~ParamEditor();

    Z::Parameter* parameter() const { return _param; }

    QString verify() const;

    QWidget* labelName() const;
    QWidget* labelLabel() const;
    QWidget* valueEditor() const;
    QWidget* unitsSelector() const;

    void parameterChanged(Z::ParameterBase*) override { populate(); }

signals:
    void focused();
    void goingFocusNext();
    void goingFocusPrev();

public slots:
    void populate();
    void apply();
    void focus();

protected:
    void mousePressEvent(QMouseEvent*) override;

private:
    Z::Parameter* _param;
    Ori::Widgets::ValueEdit* _valueEditor;
    UnitComboBox* _unitsSelector;
    QLabel* _labelName = nullptr;
    QLabel* _labelLabel = nullptr;

private slots:
    void editorFocused(bool focus);
    void editorKeyPressed(int key);
};

//------------------------------------------------------------------------------


#endif // PARAM_EDITOR_H
