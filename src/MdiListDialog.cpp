#include "MdiListDialog.h"

#include <QDialogButtonBox>
#include <QListWidget>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QVBoxLayout>

void dialogMdiWindowList(QMdiArea *mdiArea, QWidget *parent)
{
  MdiListDialog* dialog = new MdiListDialog(mdiArea, parent);
  dialog->exec();
}

int MdiListDialog::lastW = -1;
int MdiListDialog::lastH = -1;

MdiListDialog::MdiListDialog(QMdiArea *mdiArea, QWidget *parent, Qt::WFlags fl)
  : QDialog(parent, fl)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(tr("Select window"));
  if(lastW != -1 && lastH != -1) resize(lastW, lastH);

  this->mdiArea = mdiArea;

  ////////////// windows list
	listWindows = new QListWidget();
	listWindows->setAlternatingRowColors(true);
	listWindows->setSpacing(3);
	listWindows->setIconSize(QSize(16, 16));

	QListWidgetItem *item;
	QMdiSubWindow *subWindow;
	QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow();
	QList<QMdiSubWindow*> windows = mdiArea->subWindowList();
  for(int i = 0; i < windows.size(); ++i)
	{
    subWindow = windows.at(i);
    if(!subWindow->isVisible()) continue;
    item = new QListWidgetItem(subWindow->windowTitle(), listWindows);
    item->setIcon(subWindow->windowIcon());
    item->setData(Qt::UserRole, int(subWindow));
    if(subWindow == activeSubWindow)
    {
      QFont font = item->font();
      font.setBold(true);
      item->setFont(font);
      listWindows->setCurrentItem(item);
    }
  }
  if(!listWindows->currentItem() && listWindows->count() > 0)
    listWindows->setCurrentRow(0);
  connect(listWindows, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));

  ///////// dialog buttons
  QDialogButtonBox* buttonBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  ////////////// main container
  QVBoxLayout *layoutMain = new QVBoxLayout;
  layoutMain->setMargin(6);
  layoutMain->addWidget(listWindows);
  layoutMain->addWidget(buttonBox);
  setLayout(layoutMain);
}

void MdiListDialog::accept()
{
  if(listWindows->currentItem())
    mdiArea->setActiveSubWindow((QMdiSubWindow*)(
      listWindows->currentItem()->data(Qt::UserRole).toInt()));
  close();
}

void MdiListDialog::done(int r)
{
  lastW = width();
  lastH = height();
  QDialog::done(r);
}
