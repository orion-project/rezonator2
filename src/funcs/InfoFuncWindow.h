#ifndef INFO_FUNC_WINDOW_H
#define INFO_FUNC_WINDOW_H

#include "../math/FunctionBase.h"
#include "../windows/SchemaWindows.h"

QT_BEGIN_NAMESPACE
class QTextBrowser;
class QToolBar;
class QAction;
class QActionGroup;
QT_END_NAMESPACE

class Schema;
class FrozenStateButton;
class FunctionBase;

class InfoFuncWindow : public ShortcutHandlerWindow, public SchemaToolWindow, public FunctionListener
{
    Q_OBJECT

public:
    ~InfoFuncWindow() override;

    static void open(InfoFunction *func, QWidget* parent = nullptr);

protected:
    void recalcRequired(Schema*) override { processCalc(); }
    void elementDeleting(Schema*, Element*) override;
    void functionCalculated(FunctionBase*) override;
    void functionDeleted(FunctionBase*) override;

private slots:
    void freeze(bool);
    void help();
    void processCalc();
    void linkClicked(const class QUrl&);
    void copyAll();

private:
    QTextBrowser *_editor;
    QAction *actnUpdate, *actnFreeze, *actnFrozenInfo;
    FrozenStateButton *buttonFrozenInfo;
    InfoFunction *_function;
    QMap<int, QActionGroup*> _actionGroups;

    explicit InfoFuncWindow(InfoFunction *func, QWidget *parent = nullptr);

    void createToolbar();
    void updateFrozenInfo();
};

#endif // INFO_FUNC_WINDOW_H
