#ifndef CALCULATOR_WINDOW_H
#define CALCULATOR_WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QPlainTextEdit;
QT_END_NAMESPACE

namespace Z {
class Lua;
}

class CalculatorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CalculatorWindow(QWidget *parent = nullptr);
    ~CalculatorWindow();

    static void showCalcWindow();

private slots:
    void calculate();

private:
    Z::Lua* _lua;
    QPlainTextEdit* _logView;
    QPlainTextEdit* _editor;
    QLabel* _errorView;

    QWidget* makeToolbar();
    void showError(const QString& error);
    void showResult(const QString& code, double value);

    static QString stateFileName();
    void restoreState();
    void storeState();
};

#endif // CALCULATOR_WINDOW_H
