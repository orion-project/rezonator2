#include "StartWindow.h"

#include <QtGui>

StartWindow::StartWindow(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layoutMain = new QHBoxLayout;

    setLayout(layoutMain);
}
