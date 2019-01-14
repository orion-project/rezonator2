#ifndef FROZENSTATEBUTTON_H
#define FROZENSTATEBUTTON_H

#include <QToolButton>
#include <QFrame>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class FrozenStateButton : public QToolButton
{
    Q_OBJECT

public:
    explicit FrozenStateButton(const QString& title, const char* icon, QWidget *parent = nullptr);
    explicit FrozenStateButton(QWidget *parent = nullptr);

    void setInfo(const QString&);

protected:
    void mousePressEvent(class QMouseEvent*) override;

private:
    QString infoText;

private slots:
    void popupClosed();
};


class FrozenStatePopup : public QFrame
{
    Q_OBJECT

public:
    explicit FrozenStatePopup(FrozenStateButton* owner);

    void show(const QPoint&);
    void setInfo(const QString&);

private:
    QLabel *textInfo;
    FrozenStateButton *_owner;

private slots:
    void buttonCopyClick();
};

#endif // FROZENSTATEBUTTON_H
