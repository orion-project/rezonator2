#include "ParamsListWidget.h"

#include "../app/Appearance.h"
#include "../app/PersistentState.h"

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
    Ori::Dlg::Dialog dlg(&paramsList, false);
    dlg.withTitle(title)
       .withStretchedContent()
       .withInitialSize(RecentData::getSize("select_param_dlg_size"))
       .withOkSignal(SIGNAL(itemDoubleClicked(QListWidgetItem*)));
    if (dlg.exec())
    {
        RecentData::setSize("select_param_dlg_size", dlg.size());
        return paramsList.selectedParam();
    }
    return nullptr;
}

ParamsListWidget::ParamsListWidget(const Z::Parameters *params, QWidget *parent) : QListWidget(parent), _params(params)
{
    setFont(Z::Gui::ValueFont().get());

#ifdef Q_OS_WIN
    // Default icon size looks OK on Ubuntu and MacOS but it is too small on Windows
    setIconSize(QSize(24, 24));
#endif

    for (auto param : *_params)
        addParamItem(param);
}

void ParamsListWidget::addParamItem(Z::Parameter* param, bool select)
{
    QListWidgetItem *item;
    if (param == noneParam())
        item = new QListWidgetItem(QIcon(":/toolbar/param_delete"), param->alias());
    else if (param->failed())
        item = new QListWidgetItem(QIcon(":/toolbar/param_warn"), param->displayStr());
    else if (param->valueDriver() == Z::ParamValueDriver::Formula)
        item = new QListWidgetItem(QIcon(":/toolbar/param_formula"), param->displayStr());
    else
        item = new QListWidgetItem(QIcon(":/toolbar/parameter"), param->displayStr());
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

