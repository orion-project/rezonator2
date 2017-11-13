#ifndef PARAM_EDITOR_H
#define PARAM_EDITOR_H

#include <QWidget>
#include <QGroupBox>

#include "../core/Parameters.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
    class InfoPanel;
    class ValueEdit;
}}

class UnitComboBox;

class ParamEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ParamEditor(Z::Parameter* param, bool showName = true);

    Z::Parameter* parameter() const { return _param; }

    QString verify() const;

    QWidget* labelName() const;
    QWidget* labelLabel() const;
    QWidget* valueEditor() const;
    QWidget* unitsSelector() const;

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

class ParamsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ParamsEditor(Z::Parameters *params, QWidget *parent = 0);

    void populate();

    void focus();
    void focus(Z::Parameter *param);

    QString verify() const;

public slots:
    void apply();

protected:
    void showEvent(QShowEvent*) override;

private:
    Z::Parameters* _params;
    QList<ParamEditor*> _editors;
    Ori::Widgets::InfoPanel* _infoPanel;

    void adjustEditors();

private slots:
    void paramFocused();
    void focusNextParam();
    void focusPrevParam();
};

//------------------------------------------------------------------------------

class ParamsEditorAbcd : public QGroupBox
{
    Q_OBJECT

public:
    explicit ParamsEditorAbcd(const QString& title, const Z::Parameters& params);
    void apply();
    void populate();
    void focus();

private:
    Z::Parameters _params;
    QVector<Ori::Widgets::ValueEdit*> _editors;
};

#endif // PARAM_EDITOR_H
