#ifndef INFO_FUNC_WINDOW_H
#define INFO_FUNC_WINDOW_H

#include <QWidget>

#include "../funcs/InfoFunctions.h"
#include "../SchemaWindows.h"

QT_BEGIN_NAMESPACE
class QTextBrowser;
class QToolBar;
class QAction;
QT_END_NAMESPACE

class Schema;

class FrozenStateButton;

class InfoFuncWindow : public QWidget, public SchemaToolWindow
{
    Q_OBJECT

public:
    explicit InfoFuncWindow(InfoFunction *func, QWidget *parent = 0);
    ~InfoFuncWindow();

    static void open(InfoFunction *func, QWidget* parent = 0);

protected:
    void schemaChanged(Schema*) override { processCalc(); }
    void schemaLoaded(Schema*) override { processCalc(); }
    void elementDeleting(Schema*, Element*) override;

private slots:
    void freeze(bool);
    void help();
    void processCalc();
    void linkClicked(const class QUrl&);

private:
    QTextBrowser *_editor;
    QAction *actnUpdate, *actnFreeze, *actnFrozenInfo;
    FrozenStateButton *buttonFrozenInfo;

    InfoFunction *_function;
    QString _result;
    bool _needRecalc = false;
    bool _frozen = false;

    void createToolbar();
    void updateFrozenInfo();
    void updateResultText();

    QString paperColor();
};

#define INFO_FUNC(func_class) \
    (new InfoFuncWindow(new InfoFunc ## func_class(schema()), this))->show();

#define INFO_FUNC_1(func_class, arg1) \
    (new InfoFuncWindow(new InfoFunc ## func_class(schema(), arg1), this))->show();

#endif // INFO_FUNC_WINDOW_H
