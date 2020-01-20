#ifndef CUSTOM_ELEMS_WINDOW_H
#define CUSTOM_ELEMS_WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QFileSystemWatcher;
class QMenu;
class QTabWidget;
QT_END_NAMESPACE

class Element;
class Schema;
class SchemaElemsTable;
namespace Ori {
namespace Widgets {
class FlatToolBar;
}}

class CustomElemsWindow : public QWidget
{
    Q_OBJECT

public:
    ~CustomElemsWindow() override;

    static void showWindow();

private slots:
    void actionElemAdd();
    void actionElemMoveUp();
    void actionElemMoveDown();
    void actionElemProp();
    void actionElemDelete();
    void copy();
    void paste();

private:
    Schema *_library;
    QTabWidget *_tabs;
    SchemaElemsTable *_table;
    Ori::Widgets::FlatToolBar *_toolbar;
    QAction *_actnElemAdd, *_actnElemMoveUp, *_actnElemMoveDown, *_actnElemProp,
            *_actnElemDelete, *_actnEditCopy, *_actnEditPaste;
    QMenu *_menuContextElement, *_menuContextLastRow;
    QFileSystemWatcher *_watcher;
    bool _isLibrarySaving = false;

    explicit CustomElemsWindow(Schema* library);

    void createActions();
    void createToolbar();
    void createMenu();
    void restoreState();
    void storeState();
    void editElement(Element* elem);
    void rowDoubleClicked(Element*);
    void currentCellChanged(int curRow, int, int prevRow, int);
    void contextMenuAboutToShow(QMenu* menu);
    bool saveLibrary();
    void libraryFileChanged(const QString&);
};

#endif // CUSTOM_ELEMS_WINDOW_H
