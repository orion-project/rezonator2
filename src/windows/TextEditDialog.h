#ifndef TEXT_EDIT_DIALOG_H
#define TEXT_EDIT_DIALOG_H

#include <QDialog>

class TextEditDialog : public QDialog
{
  Q_OBJECT

public:
  TextEditDialog(QWidget*);

private:
  class QTextEdit *editor;
  class QAction *actnFontB, *actnFontI, *actnFontU, *actnFontDlg, *actnEditUndo,
    *actnEditRedo, *actnEditCopy, *actnEditCut, *actnEditPaste, *actnAlignLeft,
    *actnAlignCenter, *actnAlignRight, *actnSymbolDlg, *actnSymbolSub,
    *actnSymbolSuper;
  class MenuToolButton *buttonAlign;

  void createActions();

private slots:
  void setFontBold(bool);
  void actionFontDlg();
  void actionSymbolDlg();
};

#endif // TEXT_EDIT_DIALOG_H
