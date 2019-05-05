#ifndef CALCULATOR_WINDOW_H
#define CALCULATOR_WINDOW_H

#include "RezonatorDialog.h"

QT_BEGIN_NAMESPACE
class QGroupBox;
class QLabel;
class QPlainTextEdit;
class QSplitter;
QT_END_NAMESPACE

namespace Z {
class Lua;
}


class CalculatorSettingsDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    CalculatorSettingsDlg();

protected slots:
    void collect() override;

private:
    QGroupBox* _groupFont;
    QPushButton* _chooseFontButton;
    QLabel* _fontSampleLabel;
    void chooseFontCliked();
};


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
    void showSettings();

private:
    Z::Lua* _lua;
    QPlainTextEdit* _logView;
    QPlainTextEdit* _editor;
    QLabel* _errorView;
    QSplitter* _splitter;
    bool _overrideFont = false;
    QString _overrideFontName;
    int _overrideFontSize = 0;

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
    void adjustFont();

    friend class CalculatorSettingsDlg;
};

#endif // CALCULATOR_WINDOW_H
