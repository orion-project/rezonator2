#ifndef CUSTOM_ELEMS_WINDOW_H
#define CUSTOM_ELEMS_WINDOW_H

#include <QWidget>

class CustomElemsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CustomElemsWindow(QWidget *parent = nullptr);
    ~CustomElemsWindow() override;

    static void showWindow();

private:
    void restoreState();
    void storeState();
};

#endif // CUSTOM_ELEMS_WINDOW_H
