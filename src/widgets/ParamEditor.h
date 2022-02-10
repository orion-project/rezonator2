#ifndef PARAM_EDITOR_H
#define PARAM_EDITOR_H

#include <QToolButton>
#include <QPushButton>

#include "../core/Parameters.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
    class ValueEdit;
}}
class UnitComboBox;


// QPushButton looks ugly on "macintosh" style, it looses its standard view
// and can't calcutale its size propely (even fixed size).
// QToolButton can't be focused by tab but looks the same on all styles so use it.
// TODO: add Ctrl+= shortcut to invoke the button, it will be even more usable than tabbing it.
class LinkButton : public QToolButton
{
    Q_OBJECT

public:
    LinkButton();
    QSize sizeHint() const override;
    void showLinkSource(Z::Parameter *param);

signals:
    void focused(bool focus);

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
};


class MenuButton : public QPushButton
{
    Q_OBJECT

public:
    MenuButton(QList<QAction*> actions);

    QMenu* menu() { return _menu; }

signals:
    void focused(bool focus);

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

private:
    QMenu* _menu;
    bool _isMenuOpened = false;
};


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

        /// If this list is not empty, the "Menu" button is added after the editor
        QList<QAction*> menuButtonActions;

        /// Auxilary control to be put after unit selector.
        QWidget* auxControl = nullptr;

        /// Should editor take ownership on the parameter.
        /// If yes, the parameter is deleted when the editor gets deleted.
        bool ownParam = false;

        bool rescaleOnUnitChange = false;

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

    void editorFocused(bool focus);

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
    bool _ownParam;
    bool _rescaleOnUnitChange = false;

    void linkToGlobalParameter();
    void showValue(Z::Parameter *param);
    void setIsLinked(bool on);
    void editorKeyPressed(int key);
};

#endif // PARAM_EDITOR_H
