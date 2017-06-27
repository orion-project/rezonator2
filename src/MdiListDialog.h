#ifndef WINDOW_MDI_LIST_H
#define WINDOW_MDI_LIST_H

#include <QDialog>

class MdiListDialog : public QDialog
{
  Q_OBJECT

private:
  class QMdiArea* mdiArea;
  class QListWidget* listWindows;

  static int lastW, lastH;

public slots:
  void accept();
  void done(int r);

public:
  MdiListDialog(QMdiArea* mdiArea, QWidget* parent, Qt::WFlags fl = 0);
};

#endif // WINDOW_MDI_LIST_H
