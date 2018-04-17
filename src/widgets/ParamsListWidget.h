#ifndef PARAMS_LIST_WIDGET_H
#define PARAMS_LIST_WIDGET_H

#include <QListWidget>

#include "../core/Parameters.h"

class ParamsListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit ParamsListWidget(const Z::Parameters *params, QWidget *parent = nullptr);

    void addParamItem(Z::Parameter *param, bool select = false);

    void setSelectedParam(const Z::Parameter* param);
    Z::Parameter* selectedParam() const;

    /// Show parameters list in a dialog allowing user to choose one of parameters.
    /// Returns nullptr when the dialog is rejected via Cancel button os Esc.
    static Z::Parameter* selectParamDlg(const Z::Parameters *params, const QString& title);
    static Z::Parameter* selectParamDlg(const Z::Parameters *params, const Z::Parameter *selected, const QString& title);

    /// Special kind of parameter that can be treated as 'absence of parameter'.
    /// Add this parameter to Z::Parameters list before passing it to ParamsListWidget
    /// and then check for selectedParam() == noneParam() to implement parameter reset behaviour.
    static Z::Parameter* noneParam();

private:
    const Z::Parameters *_params;

    Z::Parameter* paramOfItem(QListWidgetItem *item) const;
};

#endif // PARAMS_LIST_WIDGET_H
