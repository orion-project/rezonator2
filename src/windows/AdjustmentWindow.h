#ifndef ADJUSTMENT_WINDOW_H
#define ADJUSTMENT_WINDOW_H

#include <QToolButton>

#include "SchemaWindows.h"

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
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


struct AdjusterSettings
{
    double increment = 1;
    double multiplier = 1.1;
};


class AdjusterSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    AdjusterSettingsWidget(const AdjusterSettings& settings, QWidget *parent = nullptr);

    AdjusterSettings settings() const;

    bool shouldSetDefault() const;
    bool shouldUseForAll() const;

private:
    AdjusterSettings _settings;
    Ori::Widgets::ValueEdit *_increment, *_multiplier;
    QCheckBox *_flagSetDefault, *_flagUseForAll;
};


class AdjusterWidget : public QWidget, public Z::ParameterListener
{
    Q_OBJECT

public:
    explicit AdjusterWidget(Schema* schema, Z::Parameter* param, QWidget *parent = nullptr);
    ~AdjusterWidget() override;

    void parameterChanged(Z::ParameterBase*) override;

    bool isFocused() const { return _isFocused; }
    bool isReadOnly() const { return _isReadOnly; }

    void adjustPlus();
    void adjustMinus();
    void adjustMult();
    void adjustDivide();

    AdjusterSettings settings() const { return _settings; }
    void setSettings(const AdjusterSettings& s) { _settings = s; }

    void restoreValue();
    void applyEditing();

public slots:
    void focus();

signals:
    void focused();
    void goingFocusNext();
    void goingFocusPrev();

protected:
    void mousePressEvent(QMouseEvent*) override;

private:
    Schema* _schema;
    Element* _elem = nullptr;
    Z::Parameter* _param;
    Z::Value _sourceValue;
    Z::Value _currentValue;
    AdjusterSettings _settings;
    bool _isValueChanging = false;
    QLabel *_labelName, *_labelUnit;
    Ori::Widgets::ValueEdit* _valueEditor;
    AdjusterButton *_buttonPlus, *_buttonMinus, *_buttonMult, *_buttonDivide;
    bool _isFocused = false;
    bool _isReadOnly = false;
    QTimer* _changeValueTimer = nullptr;
    QLabel* _lockLabel;

    void editorFocused(bool focus);
    void editorKeyPressed(int key);
    void populate();
    void changeValue();

    void setCurrentValue(double value);
    double currentValue() const;
    
    friend class AdjusterTester;
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


class AdjustmentWindow : public QWidget, public SchemaToolWindow, public IShortcutListener
{
    Q_OBJECT

public:
    static void adjust(Schema* schema, Z::Parameter* param);
    ~AdjustmentWindow() override;

    // Implementation of SchemaListener
    void elementDeleting(Schema*, Element*) override;
    void customParamDeleting(Schema*, Z::Parameter*) override;

    // Implementation of IShortcutListener
    void shortcutEnterPressed() override;

private slots:
    void addAdjuster();
    void restoreValue();
    void setupAdjuster();
    void deleteAdjuster();
    void help();

private:
    explicit AdjustmentWindow(Schema* schema, QWidget *parent = nullptr);

    struct AdjusterItem
    {
        Z::Parameter* param;
        AdjusterWidget* widget;
    };

    Schema* _schema;
    QList<AdjusterItem> _adjusters;
    AdjusterListWidget* _adjustersWidget;
    QAction *_actnRestore, *_actnSettings, *_actnDelete;

    void addAdjuster(Z::Parameter* param);
    void deleteAdjuster(Z::Parameter* param);
    AdjusterWidget* focusedAdjuster();
    void updateActions();
};

#endif // ADJUSTMENT_WINDOW_H
