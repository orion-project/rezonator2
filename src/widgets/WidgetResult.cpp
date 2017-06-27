#include "WidgetResult.h"

#include <QApplication>
#include <QLineEdit>
#include <QMessageBox>

void WidgetResult::show(QWidget *parent)
{
    if (_result || !_widget) return;

    auto focusable = findFocusable(_widget);

    auto lineEdit = qobject_cast<QLineEdit*>(focusable);
    if (lineEdit)
        lineEdit->selectAll();

    focusable->setFocus();

    QMessageBox::warning(parent, qApp->applicationName(), _message);
}

QWidget* WidgetResult::findFocusable(QWidget* w)
{
    auto children = w->children();
    if (children.isEmpty())
        return w;

    for (auto child: children)
    {
        auto widget = qobject_cast<QWidget*>(child);
        if (widget)
            return findFocusable(widget);
    }

    return w;
}
