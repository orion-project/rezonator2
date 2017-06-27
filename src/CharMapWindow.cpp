#include "CharMapWindow.h"

#include "../widgets/CharMap.h"

#include <QBoxLayout>
#include <QFontComboBox>
#include <QLabel>
#include <QPushButton>

CharMapWindow::CharMapWindow(QWidget* parent, Qt::WFlags f) : QWidget(parent, f)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(tr("Character map"));

  ///////// font controls
  QFontComboBox *comboFont = new QFontComboBox;
  comboSize = new QComboBox;
  comboStyle = new QComboBox;

  ///////// font controls container
  QHBoxLayout *layoutFont = new QHBoxLayout;
  layoutFont->addWidget(new QLabel(tr("Font:")));
  layoutFont->addWidget(comboFont);
  layoutFont->addWidget(new QLabel(tr("Size:")));
  layoutFont->addWidget(comboSize);
  layoutFont->addWidget(new QLabel(tr("Style:")));
  layoutFont->addWidget(comboStyle, 1);
  layoutFont->addStretch();

  findStyles(comboFont->currentFont());
  findSizes(comboFont->currentFont());

  ///////// char map
  CharMap *charMap = new CharMap;
  charMap->setFont(comboFont->currentFont());
  charMap->setSize(comboSize->currentText());
  charMap->setStyle(comboStyle->itemText(comboStyle->currentIndex()));

  connect(comboFont, SIGNAL(currentFontChanged(const QFont &)),
    this, SLOT(findStyles(const QFont &)));
  connect(comboFont, SIGNAL(currentFontChanged(const QFont &)),
    this, SLOT(findSizes(const QFont &)));
  connect(comboFont, SIGNAL(currentFontChanged(const QFont &)),
    charMap, SLOT(setFont(const QFont &)));
  connect(comboSize, SIGNAL(currentIndexChanged(const QString &)),
    charMap, SLOT(setSize(const QString &)));
  connect(comboStyle, SIGNAL(currentIndexChanged(const QString &)),
    charMap, SLOT(setStyle(const QString &)));

  ///////// buttons
  QPushButton *buttonCopy = new QPushButton(tr("To clipboard"));
  QPushButton *buttonPaste = new QPushButton(tr("Paste"));

  ///////// buttons container
  QHBoxLayout *layoutButtons = new QHBoxLayout;
  layoutButtons->addStretch();
  layoutButtons->addWidget(buttonCopy);
  layoutButtons->addWidget(buttonPaste);

  ///////// main container
  QVBoxLayout *layoutMain = new QVBoxLayout;
  layoutMain->addLayout(layoutFont);
  layoutMain->addWidget(charMap);
  layoutMain->addLayout(layoutButtons);

  setLayout(layoutMain);
}

void CharMapWindow::findStyles(const QFont &font)
{
  QFontDatabase fontDatabase;
  QString currentItem = comboStyle->currentText();
  comboStyle->clear();

  QString style;
  foreach(style, fontDatabase.styles(font.family()))
    comboStyle->addItem(style);

  int styleIndex = comboStyle->findText(currentItem);
  comboStyle->setCurrentIndex((styleIndex == -1)? 0: styleIndex);
}

void CharMapWindow::findSizes(const QFont &font)
{
  QFontDatabase fontDatabase;
  QString currentSize = comboSize->currentText();
  comboSize->blockSignals(true);
  comboSize->clear();
  int size;
  if(fontDatabase.isSmoothlyScalable(font.family(), fontDatabase.styleString(font)))
  {
    foreach(size, QFontDatabase::standardSizes())
      comboSize->addItem(QVariant(size).toString());
    comboSize->setEditable(true);
  }
  else
  {
    foreach(size, fontDatabase.smoothSizes(font.family(), fontDatabase.styleString(font)))
      comboSize->addItem(QVariant(size).toString());
    comboSize->setEditable(false);
  }
  comboSize->blockSignals(false);

  int sizeIndex = comboSize->findText(currentSize);
  comboSize->setCurrentIndex((sizeIndex == -1)? qMax(0, comboSize->count()/3): sizeIndex);
}
