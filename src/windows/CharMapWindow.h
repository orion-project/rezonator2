#ifndef CHAR_MAP_WINDOW_H
#define CHAR_MAP_WINDOW_H

#include <QWidget>

class CharMapWindow : public QWidget
{
private:
  class QComboBox *comboStyle, *comboSize;

public slots:
  void findStyles(const QFont &font);
  void findSizes(const QFont &font);

public:
  CharMapWindow(QWidget* parent = 0, Qt::WFlags f = 0);
};

#endif // CHAR_MAP_WINDOW_H
