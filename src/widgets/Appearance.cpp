#include "Appearance.h"

#include <QApplication>
#include <QLabel>
#include <QStyle>

namespace Z {
namespace Gui {

void setValueFont(QWidget *widget)
{
    QFont f = widget->font();
    if (f.pointSize() < 10)
    {
        f.setPointSize(10);
        widget->setFont(f);
    }
}

void setSymbolFont(QWidget *widget)
{
    QFont f = widget->font();
    f.setBold(true);
    f.setPointSize(14);
    f.setFamily("Times New Roman");
    widget->setFont(f);
}

QLabel* symbolLabel(const QString& text)
{
    auto label = new QLabel(text);
    setSymbolFont(label);
    return label;
}

} // namespace Gui
} // namespace Z
