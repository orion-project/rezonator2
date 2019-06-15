#ifndef ADJUSTMENT_WINDOW_H
#define ADJUSTMENT_WINDOW_H

#include <QToolButton>

#include "SchemaWindows.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QTimer;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
    class ValueEdit;
}}


class AdjusterButton : public QToolButton
{
    Q_OBJECT

public:
    AdjusterButton(const char* iconPath);

signals:
    void focused(bool focus);

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
};


class AdjusterWidget : public QWidget, public Z::ParameterListener
{
    Q_OBJECT

public:
    explicit AdjusterWidget(Z::Parameter* param, QWidget *parent = nullptr);
    ~AdjusterWidget() override;

    void parameterChanged(Z::ParameterBase*) override;

public slots:
    void focus();

signals:
    void focused();
    void goingFocusNext();
    void goingFocusPrev();
    void valueEdited(double value);
    void deleteRequsted();

protected:
    void mousePressEvent(QMouseEvent*) override;

private:
    Ori::Widgets::ValueEdit* _valueEditor;
    Z::Parameter* _param;
    Z::Value _sourceValue;
    Z::Value _currentValue;
    double _increment = 1;
    double _multiplier = 1.1;
    bool _isValueChanging = false;
    QLabel *_labelLabel, *_labelUnit;
    AdjusterButton *_buttonPlus, *_buttonMinus, *_buttonMult, *_buttonDivide;
    bool _isFocused = false;
    QTimer* _changeValueTimer = nullptr;

    void editorFocused(bool focus);
    void editorKeyPressed(int key);
    void populate();
    void adjustPlus();
    void adjustMinus();
    void adjustMult();
    void adjustDivide();
    void setupAdjuster();
    void restoreValue();
    void help();
    void changeValue();

    void setCurrentValue(double value);
    double currentValue() const;
};


class AdjusterListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdjusterListWidget(QWidget *parent = nullptr);

    void add(AdjusterWidget* w);
    void remove(AdjusterWidget* w);

private:
    QList<AdjusterWidget*> _items;

    void focusNextParam();
    void focusPrevParam();
};


class AdjustmentWindow : public QWidget, public SchemaToolWindow
{
    Q_OBJECT

public:
    static void adjust(Schema* schema, Z::Parameter* param);
    ~AdjustmentWindow() override;

    // Implementation of SchemaListener
    void elementDeleting(Schema*, Element*) override;
    void customParamDeleting(Schema*, Z::Parameter*) override;

private:
    explicit AdjustmentWindow(Schema* schema, QWidget *parent = nullptr);

    struct AdjusterItem
    {
        Z::Parameter* param;
        AdjusterWidget* widget;
    };

    QList<AdjusterItem> _adjusters;
    AdjusterListWidget* _adjustersWidget;

    void addAdjuster(Z::Parameter* param);
    void deleteAdjuster(Z::Parameter* param);
    void deleteCurrentAdjuster();
};

#endif // ADJUSTMENT_WINDOW_H
