#ifndef CALCULATOR_WINDOW_H
#define CALCULATOR_WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QPlainTextEdit;
class QSplitter;
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

    static void showWindow();

private slots:
    void calculate();
    void clearLog();

private:
    Z::Lua* _lua;
    QPlainTextEdit* _logView;
    QPlainTextEdit* _editor;
    QLabel* _errorView;
    QSplitter* _splitter;

    struct LogItem {
        QString code;
        double result;
    };
    QList<LogItem> _log;

    QWidget* makeToolbar();
    void showError(const QString& error);
    void showResult(const QString& code, double result);

    void restoreState();
    void storeState();
};

#endif // CALCULATOR_WINDOW_H
