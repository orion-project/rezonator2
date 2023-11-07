#ifndef IRIS_WINDOW_H
#define IRIS_WINDOW_H

#include <QWidget>

class IrisWindow : public QWidget
{
    Q_OBJECT

public:
    ~IrisWindow();

    static void showWindow();

private:
    explicit IrisWindow(QWidget *parent = nullptr);

    void restoreState();
    void storeState();
};

#endif // IRIS_WINDOW_H
