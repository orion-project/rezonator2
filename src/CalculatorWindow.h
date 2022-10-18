#ifndef CALCULATOR_WINDOW_H
#define CALCULATOR_WINDOW_H

#include "RezonatorDialog.h"

QT_BEGIN_NAMESPACE
class QGroupBox;
class QLabel;
class QTableWidget;
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
    void reuseItem();

private:
    Z::Lua* _lua;
    QPlainTextEdit* _logView;
    QPlainTextEdit* _editor;
    QTableWidget* _varsView;
    QLabel* _errorView;
    QSplitter* _mainSplitter;
    QSplitter* _sessionSplitter;
    bool _overrideFont = false;
    QString _overrideFontName;
    int _overrideFontSize = 0;
    QAction *_actnCalc, *_actnClear;

    struct LogItem {
        QString code;
        double result;
    };
    QList<LogItem> _log;

    QWidget* makeToolbar();
    bool reopenLua();
    void showError(const QString& error);
    void showResult(const QString& code, double result);
    void restoreState();
    void storeState();
    void adjustFont();
    void populateVars();

    friend class CalculatorSettingsDlg;
};

#endif // CALCULATOR_WINDOW_H
