#include "TextEditDialog.h"
#include "CharMapWindow.h"
#include "../core/Settings.h"
#include "../widgets/DialogButtonsBox.h"
#include "../widgets/FlatToolBar.h"
#include "../widgets/MenuToolButton.h"
#include "../widgets/PopupButton.h"
#include "../widgets/PopupGrid.h"

#include <QAction>
#include <QBoxLayout>
#include <QComboBox>
#include <QFontComboBox>
#include <QLabel>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolBar>

bool dialogTextEdit(QWidget *parent)
{
  TextEditDialog dialog(parent);
  dialog.exec();
  return dialog.result() == QDialog::Accepted;
}

TextEditDialog::TextEditDialog(QWidget *parent) : QDialog(parent)
{
  ///////// font controls
  QFontComboBox *comboFont = new QFontComboBox;
  QComboBox *comboSize = new QComboBox;

  ///////// text editor
  editor = new QTextEdit;
  editor->setAcceptRichText(false);
  editor->setText("Hallo, World!");

  createActions();

  ///////// view controls
  buttonAlign = new MenuToolButton;
  buttonAlign->addAction(actnAlignLeft);
  buttonAlign->addAction(actnAlignCenter);
  buttonAlign->addAction(actnAlignRight);

  ///////// edit controls
  QString resPath = Settings::instance().toolbarImages();
  QFont font("Times", 12);

  PopupGrid *popupGreekSm  = new PopupGrid(this);
  for(ushort i = 0x03B1; i <= 0x03C9; i++)
    popupGreekSm->addItem(QChar(i));
  popupGreekSm->setColCount(5);
  popupGreekSm->setCellSize(32);
  popupGreekSm->setFont(font);
  connect(popupGreekSm, SIGNAL(selected(const QString&)),
    editor, SLOT(insertPlainText(const QString&)));

  PopupButton *buttonGreekSm = new PopupButton;
  buttonGreekSm->setPopup(popupGreekSm);
  buttonGreekSm->setIcon(QIcon(resPath + "symbol_greek_sm"));
  buttonGreekSm->setToolTip(tr("Insert small greek letter"));

  PopupGrid *popupGreekBig  = new PopupGrid(this);
  for(ushort i = 0x0391; i <= 0x03A1; i++)
    popupGreekBig->addItem(QChar(i));
  for(ushort i = 0x03A3; i <= 0x03A9; i++)
    popupGreekBig->addItem(QChar(i));
  popupGreekBig->setColCount(5);
  popupGreekBig->setCellSize(32);
  popupGreekBig->setFont(font);
  connect(popupGreekBig, SIGNAL(selected(const QString&)),
    editor, SLOT(insertPlainText(const QString&)));

  PopupButton *buttonGreekBig = new PopupButton;
  buttonGreekBig->setPopup(popupGreekBig);
  buttonGreekBig->setIcon(QIcon(resPath + "symbol_greek_big"));
  buttonGreekBig->setToolTip(tr("Insert capital greek letter"));

  ///////// toolbar "View"
  FlatToolBar *toolbarView = new FlatToolBar;
  toolbarView->setIconSize(Settings::instance().toolbarIconSize());
  toolbarView->addAction(actnFontDlg);
  toolbarView->addWidget(new QLabel(" "));
  toolbarView->addWidget(comboFont);
  toolbarView->addWidget(new QLabel(" "));
  toolbarView->addWidget(comboSize);
  toolbarView->addWidget(new QLabel(" "));
  toolbarView->addAction(actnFontB);
  toolbarView->addAction(actnFontI);
  toolbarView->addAction(actnFontU);
  toolbarView->addSeparator();
  toolbarView->addWidget(buttonAlign);

  ///////// toolbar "Edit"
  FlatToolBar *toolbarEdit = new FlatToolBar;
  toolbarEdit->setIconSize(Settings::instance().toolbarIconSize());
  toolbarEdit->addAction(actnEditUndo);
  toolbarEdit->addAction(actnEditRedo);
  toolbarEdit->addSeparator();
  toolbarEdit->addAction(actnEditCut);
  toolbarEdit->addAction(actnEditCopy);
  toolbarEdit->addAction(actnEditPaste);
  toolbarEdit->addSeparator();
  toolbarEdit->addAction(actnSymbolSub);
  toolbarEdit->addAction(actnSymbolSuper);
  toolbarEdit->addSeparator();
  toolbarEdit->addWidget(buttonGreekSm);
  toolbarEdit->addWidget(buttonGreekBig);
  toolbarEdit->addAction(actnSymbolDlg);

  ///////// tool palettes
  /*QStackedWidget *tools = new QStackedWidget;
  tools->addWidget(toolbarView);
  tools->addWidget(toolbarEdit);

  QTabBar *tabs = new QTabBar;
  tabs->addTab(tr("View"));
  tabs->addTab(tr("Edit"));
  connect(tabs, SIGNAL(currentChanged(int)), tools, SLOT(setCurrentIndex(int)));*/

  QTabWidget *tabs = new QTabWidget;
  tabs->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  tabs->addTab(toolbarView, tr("View"));
  tabs->addTab(toolbarEdit, tr("Edit"));

  ///////// dialog buttons
  DialogButtonsBox *layoutButtons = new DialogButtonsBox(0,
    DialogButtonsBox::Help | DialogButtonsBox::Apply);
  connect(layoutButtons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(layoutButtons, SIGNAL(rejected()), this, SLOT(reject()));

  ///////// main container
  QVBoxLayout *layoutMain = new QVBoxLayout;
  layoutMain->setMargin(6);
  layoutMain->addWidget(tabs);
  layoutMain->addWidget(editor);
  layoutMain->addLayout(layoutButtons);
  setLayout(layoutMain);
}

void TextEditDialog::createActions()
{
  QString resPath = Settings::instance().toolbarImages();

  ///////// view actions
  actnFontB = new QAction(this);
  actnFontB->setToolTip(tr("Bold"));
  actnFontB->setIcon(QIcon(resPath + "edit_font_b"));
  actnFontB->setCheckable(true);
  connect(actnFontB, SIGNAL(toggled(bool)), this, SLOT(setFontBold(bool)));

  actnFontI = new QAction(this);
  actnFontI->setToolTip(tr("Italic"));
  actnFontI->setIcon(QIcon(resPath + "edit_font_i"));
  actnFontI->setCheckable(true);
  connect(actnFontI, SIGNAL(toggled(bool)), editor, SLOT(setFontItalic(bool)));

  actnFontU = new QAction(this);
  actnFontU->setToolTip(tr("Underline"));
  actnFontU->setIcon(QIcon(resPath + "edit_font_u"));
  actnFontU->setCheckable(true);
  connect(actnFontU, SIGNAL(toggled(bool)), editor, SLOT(setFontUnderline(bool)));

  actnFontDlg = new QAction(this);
  actnFontDlg->setToolTip(tr("Font dialog"));
  actnFontDlg->setIcon(QIcon(resPath + "edit_font_dlg"));

  ///////// edit actions
  actnEditUndo = new QAction(this);
  actnEditUndo->setToolTip(tr("Undo"));
  actnEditUndo->setIcon(QIcon(resPath + "edit_undo"));
  actnEditUndo->setEnabled(false);
  connect(actnEditUndo, SIGNAL(triggered()), editor, SLOT(undo()));
  connect(editor, SIGNAL(undoAvailable(bool)), actnEditUndo, SLOT(setEnabled(bool)));

  actnEditRedo = new QAction(this);
  actnEditRedo->setToolTip(tr("Redo"));
  actnEditRedo->setIcon(QIcon(resPath + "edit_redo"));
  actnEditRedo->setEnabled(false);
  connect(actnEditRedo, SIGNAL(triggered()), editor, SLOT(redo()));
  connect(editor, SIGNAL(redoAvailable(bool)), actnEditRedo, SLOT(setEnabled(bool)));

  actnEditCut = new QAction(this);
  actnEditCut->setToolTip(tr("Cut"));
  actnEditCut->setIcon(QIcon(resPath + "edit_cut"));
  actnEditCut->setEnabled(false);
  connect(actnEditCut, SIGNAL(triggered()), editor, SLOT(cut()));
  connect(editor, SIGNAL(copyAvailable(bool)), actnEditCut, SLOT(setEnabled(bool)));

  actnEditCopy = new QAction(this);
  actnEditCopy->setToolTip(tr("Copy"));
  actnEditCopy->setIcon(QIcon(resPath + "edit_copy"));
  actnEditCopy->setEnabled(false);
  connect(actnEditCopy, SIGNAL(triggered()), editor, SLOT(copy()));
  connect(editor, SIGNAL(copyAvailable(bool)), actnEditCopy, SLOT(setEnabled(bool)));

  actnEditPaste = new QAction(this);
  actnEditPaste->setToolTip(tr("Paste"));
  actnEditPaste->setIcon(QIcon(resPath + "edit_paste"));
  connect(actnEditPaste, SIGNAL(triggered()), editor, SLOT(paste()));

  actnAlignLeft = new QAction(this);
  actnAlignLeft->setText(tr("Left"));
  actnAlignLeft->setToolTip(tr("Align: left"));
  actnAlignLeft->setIcon(QIcon(resPath + "edit_align_left"));

  actnAlignCenter = new QAction(this);
  actnAlignCenter->setText(tr("Center"));
  actnAlignCenter->setToolTip(tr("Align: center"));
  actnAlignCenter->setIcon(QIcon(resPath + "edit_align_center"));

  actnAlignRight = new QAction(this);
  actnAlignRight->setText(tr("Right"));
  actnAlignRight->setToolTip(tr("Align: right"));
  actnAlignRight->setIcon(QIcon(resPath + "edit_align_right"));

  actnSymbolSub = new QAction(this);
  actnSymbolSub->setToolTip(tr("Subscript"));
  actnSymbolSub->setIcon(QIcon(resPath + "symbol_subscript"));

  actnSymbolSuper = new QAction(this);
  actnSymbolSuper->setToolTip(tr("Superscript"));
  actnSymbolSuper->setIcon(QIcon(resPath + "symbol_superscript"));

  actnSymbolDlg = new QAction(this);
  actnSymbolDlg->setToolTip(tr("Sharacter map"));
  actnSymbolDlg->setIcon(QIcon(resPath + "symbol_dlg"));
  connect(actnSymbolDlg, SIGNAL(triggered()), this, SLOT(actionSymbolDlg()));
}

void TextEditDialog::setFontBold(bool yes)
{
  editor->setFontWeight(yes? QFont::Bold: QFont::Normal);
}

void TextEditDialog::actionFontDlg()
{
}

void TextEditDialog::actionSymbolDlg()
{
  CharMapWindow *w = new CharMapWindow(this);
  w->show();
}
