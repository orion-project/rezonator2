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

class LinkButton;
class UnitComboBox;

class ParamEditor : public QWidget, public Z::ParameterListener
{
    Q_OBJECT

public:
    struct Options {
        /// Target editing parameter.
        Z::Parameter* param;

        /// Global params are used to link target parameter to.
        /// Must be set when @a allowLinking is enabled.
        Z::Parameters* globalParams = nullptr;

        /// Container for links to global parameters.
        /// Must be set when @a allowLinking is enabled.
        Z::ParamLinks* paramLinks = nullptr;

        /// Show full parameter name before its alias.
        bool showName = false;

        /// Enable linking target param to global params.
        bool allowLinking = false;

        Options(Z::Parameter* p) : param(p) {}
    };

    explicit ParamEditor(Options opts);
    ~ParamEditor() override;

    Z::Parameter* parameter() const { return _param; }

    /// Returns value of the editor.
    /// This is NOT a value of the edited parameter, at least till `apply()` will be called.
    Z::Value getValue() const;

    QString verify() const;

    QWidget* labelName() const;
    QWidget* labelLabel() const;
    QWidget* valueEditor() const;
    QWidget* unitsSelector() const;

    // Implements Z::ParameterListener
    void parameterChanged(Z::ParameterBase*) override;

signals:
    void focused();
    void goingFocusNext();
    void goingFocusPrev();
    void editorInfoChanged();
    void valueEdited(double value);
    void unitChanged(Z::Unit unit);

public slots:
    void populate();
    void apply();
    void focus();

protected:
    void mousePressEvent(QMouseEvent*) override;

private:
    Z::Parameter* _param;
    Z::Parameter* _linkSource = nullptr;
    Z::Parameters* _globalParams;
    Z::ParamLinks* _paramLinks;
    Ori::Widgets::ValueEdit* _valueEditor;
    UnitComboBox* _unitsSelector;
    QLabel* _labelName = nullptr;
    QLabel* _labelLabel = nullptr;
    LinkButton* _linkButton = nullptr;
    QString _editorInfo;
    bool _paramChangedHandlerEnabled = true;

    void linkToGlobalParameter();
    void showValue(Z::Parameter *param);
    void setIsLinked(bool on);
    void editorFocused(bool focus);
    void editorKeyPressed(int key);
};

#endif // PARAM_EDITOR_H
