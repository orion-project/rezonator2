#ifndef CUSTOM_FUNCS_WINDOW_H
#define CUSTOM_FUNCS_WINDOW_H

#include <QWidget>

class Schema;

class QAction;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QSplitter;
class QToolBar;

namespace Ori::Widgets {
class CodeEditor;
}

class CustomFuncsWindow : public QWidget
{
    Q_OBJECT

public:
    static void showWindow(Schema *target = nullptr);
    
protected:
    bool event(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    
private slots:
    void actionDelete();
    void actionApply();
    void actionEdit();
    void actionSave();
    void actionCancel();
    void actionExport();
    void actionImport();
    void actionHelp();
    void onFileSelected(QListWidgetItem *current, QListWidgetItem *previous);
    void applyFilter();

private:
    Schema *_target;
    QLineEdit *_filterEdit;
    QListWidget *_fileList;
    Ori::Widgets::CodeEditor *_editor;
    QToolBar *_toolbar;
    QSplitter *_splitter;
    QWidget *_infoBanner;
    QList<QWidget*> _fadingWidgets;
    QAction *_actnDelete, *_actnApply, *_actnEdit, *_actnExport, *_actnImport, *_actnHelp;
    QAction *_actnSave, *_actnCancel;
    QString _requiredSelectedPath;
    
    CustomFuncsWindow(Schema *target);
    ~CustomFuncsWindow();
    
    void createActions();
    void createToolbar();
    void fillFileList();
    void showCurrentCode();
    void restoreState();
    void storeState();
    void setEditMode(bool on);
    QString fileFilters() const;
    QString selectedPath() const;
    bool saveModifiedCode(const QString &filePath) const;
    void showInfoPopup(const QString &text);
};

#endif // CUSTOM_FUNCS_WINDOW_H
