#include "PopupMessage.h"
#include "widgets/OriLabels.h"

#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QTimer>

void PopupMessage::affirm(const QString& text, int duration)
{
    (new PopupMessage(AFFIRM, text, duration, qApp->activeWindow()))->show();
}

void PopupMessage::error(const QString& text, int duration)
{
    (new PopupMessage(ERROR, text, duration, qApp->activeWindow()))->show();
}

void PopupMessage::info(const QString& text, int duration)
{
    (new PopupMessage(INFO, text, duration, qApp->activeWindow()))->show();
}

PopupMessage* PopupMessage::_instance = nullptr;

PopupMessage::PopupMessage(Mode mode, const QString& text, int duration, QWidget *parent) : QFrame(parent), _mode(mode)
{
    if (_instance)
        delete _instance;

    setAttribute(Qt::WA_DeleteOnClose);
    setFrameShape(QFrame::NoFrame);
    switch (mode) {
    case AFFIRM: setProperty("mode", "affirm"); break;
    case ERROR: setProperty("mode", "error"); break;
    case INFO: setProperty("mode", "info"); break;
    }

    auto label = new Ori::Widgets::Label(text);
    label->setAlignment(Qt::AlignHCenter);
    connect(label, &Ori::Widgets::Label::clicked, this, &PopupMessage::close);

    auto layout = new QHBoxLayout(this);
    layout->addWidget(label);

    auto shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(20);
    shadow->setOffset(2);
    setGraphicsEffect(shadow);

    adjustSize();
    auto sz = size();
    auto psz = parent->size();
    int x = psz.width() - sz.width() - 20;
    int y = psz.height() - sz.height()- 20;
    move(x, y);

    if (duration > 0)
        QTimer::singleShot(duration, this, [this](){
            auto opacity = new QGraphicsOpacityEffect();
            setGraphicsEffect(opacity);
            auto fadeout = new QPropertyAnimation(opacity, "opacity");
            fadeout->setDuration(1000);
            fadeout->setStartValue(1);
            fadeout->setEndValue(0);
            fadeout->setEasingCurve(QEasingCurve::OutBack);
            fadeout->start(QPropertyAnimation::DeleteWhenStopped);
            connect(fadeout, &QPropertyAnimation::finished, this, &PopupMessage::close);
        });

    _instance = this;
}

PopupMessage::~PopupMessage()
{
    _instance = nullptr;
}

void PopupMessage::mouseReleaseEvent(QMouseEvent*)
{
    close();
}
