#ifndef REZONATOR_DIALOG_H
#define REZONATOR_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

class RezonatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RezonatorDialog(QWidget *parent = 0);
    ~RezonatorDialog();

    QVBoxLayout* mainLayout() const { return _mainLayout; }

protected slots:
    virtual void collect() { accept(); }

protected:
    void showEvent(QShowEvent *event);

private:
    QVBoxLayout* _mainLayout;
};

#endif // REZONATOR_DIALOG_H
