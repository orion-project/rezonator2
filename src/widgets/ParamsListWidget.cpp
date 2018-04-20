#include "ParamsListWidget.h"

#include "helpers/OriDialogs.h"

Z::Parameter *ParamsListWidget::noneParam()
{
    static Z::Parameter p("(none)");
    return &p;
}

Z::Parameter* ParamsListWidget::selectParamDlg(const Z::Parameters *params, const QString& title)
{
    return selectParamDlg(params, nullptr, title);
}

Z::Parameter* ParamsListWidget::selectParamDlg(const Z::Parameters *params, const Z::Parameter *selected, const QString& title)
{
    ParamsListWidget paramsList(params);
    if (selected)
        paramsList.setSelectedParam(selected);
    if (Ori::Dlg::Dialog(&paramsList)
            .withTitle(title)
            .withOkSignal(SIGNAL(itemDoubleClicked(QListWidgetItem*)))
            .exec())
        return paramsList.selectedParam();
    return nullptr;
}

ParamsListWidget::ParamsListWidget(const Z::Parameters *params, QWidget *parent)
    : QListWidget(parent), _params(params)
{
    for (auto param : *_params)
        addParamItem(param);
}

void ParamsListWidget::addParamItem(Z::Parameter* param, bool select)
{
    QListWidgetItem *item;
    if (param == noneParam())
        item = new QListWidgetItem(QIcon(":/toolbar/param_delete"), param->alias());
    else if (param->valueDriver() == Z::ParamValueDriver::Formula)
        item = new QListWidgetItem(QIcon(":/toolbar/param_formula"), param->str());
    else
        item = new QListWidgetItem(QIcon(":/toolbar/parameter"), param->str());
    item->setData(Qt::UserRole, QVariant::fromValue<void*>(reinterpret_cast<void*>(param)));
    if (param->valueDriver() != Z::ParamValueDriver::None)
    {
        auto f = item->font();
        f.setItalic(true);
        item->setFont(f);
    }
    addItem(item);

    if (select)
        setCurrentItem(item);
}

void ParamsListWidget::setSelectedParam(const Z::Parameter* param)
{
    for (int row = 0; row < count(); row++)
        if (paramOfItem(item(row)) == param)
        {
            setCurrentRow(row);
            break;
        }
}

Z::Parameter* ParamsListWidget::selectedParam() const
{
    auto item = currentItem();
    return item ? paramOfItem(item) : nullptr;
}

Z::Parameter* ParamsListWidget::paramOfItem(QListWidgetItem *item) const
{
    return reinterpret_cast<Z::Parameter*>(item->data(Qt::UserRole).value<void*>());
}

