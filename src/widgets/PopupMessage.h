#ifndef POPUP_MESSAGE_H
#define POPUP_MESSAGE_H

#include <QFrame>

class PopupMessage : public QFrame
{
    Q_OBJECT

public:
    static void affirm(const QString& text, int duration = 2000);
    static void error(const QString& text, int duration = 2000);
    static void info(const QString& text, int duration = 2000);

protected:
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    enum Mode {AFFIRM, ERROR, INFO};
    Mode _mode;
    explicit PopupMessage(Mode mode, const QString& text, int duration, QWidget *parent);
    ~PopupMessage();
    static PopupMessage* _instance;
};

#endif // POPUP_MESSAGE_H
