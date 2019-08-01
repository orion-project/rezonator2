#include "FrozenStateButton.h"

#include "../Appearance.h"

#include "helpers/OriWidgets.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QLabel>
#include <QMouseEvent>
#include <QTextDocument>
#include <QToolButton>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
//                           FrozenStateButton
//------------------------------------------------------------------------------

FrozenStateButton::FrozenStateButton(const QString& title, const char* icon, QWidget *parent) : QToolButton(parent)
{
    setToolTip(title);
    setIcon(QPixmap(QStringLiteral(":/toolbar/") % icon));
    setVisible(false);
}

FrozenStateButton::FrozenStateButton(QWidget *parent) : QToolButton(parent)
{
    setVisible(false);
}

void FrozenStateButton::setInfo(const QString& info)
{
    infoText = info;
}

void FrozenStateButton::mousePressEvent(QMouseEvent *event)
{
    QToolButton::mousePressEvent(event);

    event->accept();

    setDown(true);

    auto popup = new FrozenStatePopup(this);
    popup->setInfo(infoText);
    popup->show(mapToGlobal(rect().bottomLeft()));
    connect(popup, SIGNAL(destroyed()), this, SLOT(popupClosed()));
}

void FrozenStateButton::popupClosed()
{
    // TODO кнопка ожимается, но остается в состоянии hovered, т.к. не получила mouseLeave из-за открывшегося меню
    setDown(false);
}

//------------------------------------------------------------------------------
//                            FrozenStatePopup
//------------------------------------------------------------------------------

FrozenStatePopup::FrozenStatePopup(FrozenStateButton *owner) : QFrame(owner, Qt::Popup), _owner(owner)
{
    setAttribute(Qt::WA_WindowPropagation);
    setAttribute(Qt::WA_DeleteOnClose);
    setFrameShape(QFrame::Box);

    textInfo = new QLabel;
    textInfo->setFont(Z::Gui::ValueFont().get());

    QToolButton *buttonCopy = new QToolButton;
    buttonCopy->setIcon(QPixmap(":/toolbar/copy"));
    buttonCopy->setIconSize(owner->iconSize());
    buttonCopy->setText(tr("Copy Info"));
    buttonCopy->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    buttonCopy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(buttonCopy, SIGNAL(clicked()), this, SLOT(buttonCopyClick()));

    QVBoxLayout *layoutMain = new QVBoxLayout;
    layoutMain->addWidget(textInfo);
    layoutMain->addWidget(buttonCopy);

    setLayout(layoutMain);
}

void FrozenStatePopup::show(const QPoint &at)
{
    move(at);
    QFrame::show();
}

void FrozenStatePopup::setInfo(const QString& info)
{
    textInfo->setText(info);
}

void FrozenStatePopup::buttonCopyClick()
{
    QTextDocument doc;
    doc.setHtml(textInfo->text());
    QApplication::clipboard()->setText(doc.toPlainText());
    close();
}
