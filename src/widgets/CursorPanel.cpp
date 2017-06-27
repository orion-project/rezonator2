#include "CursorPanel.h"
#include "../core/Format.h"
#include "../../libs/qcustomplot/qcpcursor.h"
#include "widgets/OriValueEdit.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriDialogs.h"

#include <QAction>
#include <QMenu>
#include <QScrollBar>
#include <QToolBar>
#include <QToolButton>

////////////////////////////////////////////////////////////////////////////////
//                             CursorPanel
////////////////////////////////////////////////////////////////////////////////

#define TEXT_SIZE 13
#define TEXT_MARGIN_TOP 8
#define TEXT_MARGIN_BOTTOM 5

CursorPanel::CursorPanel(PlotFunction *func, QCPCursor *cursor)
    : _cursor(cursor), _function(func)
{
    QFont font = this->font();
    font.setPixelSize(TEXT_SIZE);
    setFont(font);

    setStyleSheet("margin-left: 3px; margin-right: 1px");

    verticalScrollBar()->setVisible(false);
    horizontalScrollBar()->setVisible(false);
    setOpenLinks(false);
    connect(this, SIGNAL(anchorClicked(QUrl)), this, SLOT(linkClicked(QUrl)));

    connect(_cursor, SIGNAL(positionChanged()), this, SLOT(cursorPositionCanged()));

    createActions();
}

void CursorPanel::createActions()
{
    actnShowCursor = new QAction(this);
    actnShowCursor->setCheckable(true);
    actnShowCursor->setText(tr("Cursor", "Plot action"));
    actnShowCursor->setChecked(true);
    connect(actnShowCursor, SIGNAL(toggled(bool)), _cursor, SLOT(setVisible(bool)));

    actnCursorVert = new QAction(this);
    actnCursorHorz = new QAction(this);
    actnCursorBoth = new QAction(this);
    actnCursorVert->setCheckable(true);
    actnCursorHorz->setCheckable(true);
    actnCursorBoth->setCheckable(true);
    actnCursorVert->setText(tr("Vertical Line", "Plot action"));
    actnCursorHorz->setText(tr("Horizontal Line", "Plot action"));
    actnCursorBoth->setText(tr("Both Lines", "Plot action"));
    connect(actnCursorVert, SIGNAL(triggered()), this, SLOT(setCursorShape()));
    connect(actnCursorHorz, SIGNAL(triggered()), this, SLOT(setCursorShape()));
    connect(actnCursorBoth, SIGNAL(triggered()), this, SLOT(setCursorShape()));
    actnCursorBoth->setChecked(true);

    actnCursorFollow = new QAction(this);
    actnCursorFollow->setText(tr("Follow Mouse", "Plot action"));
    actnCursorFollow->setCheckable(true);
    actnCursorFollow->setShortcut(Qt::Key_F7);
    actnCursorFollow->setIcon(QIcon(":/misc16/plot_tracing"));
    connect(actnCursorFollow, SIGNAL(toggled(bool)), _cursor, SLOT(setFollowMouse(bool)));

    actnCursorSetX = new QAction(this);
    actnCursorSetX->setText(tr("Set Cursor X-position", "Plot action"));
    actnCursorSetX->setShortcut(Qt::Key_X);
    connect(actnCursorSetX, SIGNAL(triggered()), this, SLOT(setCursorX()));
    addAction(actnCursorSetX); // activates shortcut

    actnCursorSetY = new QAction(this);
    actnCursorSetY->setText(tr("Set Cursor Y-position", "Plot action"));
    actnCursorSetY->setShortcut(Qt::Key_Y);
    connect(actnCursorSetY, SIGNAL(triggered()), this, SLOT(setCursorY()));
    addAction(actnCursorSetY); // activates shortcut
}

void CursorPanel::placeIn(class QToolBar* toolbar)
{
    QActionGroup *group = new QActionGroup(this);
    group->addAction(actnCursorVert);
    group->addAction(actnCursorHorz);
    group->addAction(actnCursorBoth);

    QMenu *menu = new QMenu(this);
    menu->addActions(group->actions());

    QToolButton *button = new QToolButton;
    button->setStyleSheet("padding-top: 3px; padding-bottom: 3px; "
                          "padding-left: 12px; padding-right: 12px");
    button->setDefaultAction(actnShowCursor);
    button->setPopupMode(QToolButton::MenuButtonPopup);
    button->setMenu(menu);
    button->setToolTip("");

    toolbar->addWidget(button);
    toolbar->addWidget(this);
    toolbar->addAction(actnCursorFollow);
}

QSize CursorPanel::sizeHint() const
{
    return QSize(9999, TEXT_SIZE + TEXT_MARGIN_TOP + TEXT_MARGIN_BOTTOM);
}

QSize CursorPanel::minimumSizeHint() const
{
    return QSize(100, TEXT_SIZE + TEXT_MARGIN_TOP + TEXT_MARGIN_BOTTOM);
}

void CursorPanel::linkClicked(const QUrl& url)
{
    if (url.scheme() == "do")
    {
        if (url.host() == "set_x")
            setCursorX();
        else if (url.host() == "set_y")
            setCursorY();
    }
}

void CursorPanel::cursorPositionCanged()
{
    if (_autoUpdateInfo)
        update();
    if (!_cursor->followMouse()) // cursor was fixed by double click
        actnCursorFollow->setChecked(false);
}

QString CursorPanel::formatCursorInfo()
{
    auto point = _cursor->position();
    return QStringLiteral("X: <a href='do://set_x'>%1</a>; Y: <a href='do://set_y'>%2</a>")
                .arg(Z::format(point.x()), Z::format(point.y()));
}

void CursorPanel::update()
{
    setText(formatCursorInfo());
}

void CursorPanel::update(const QString& info)
{
    auto cursorInfo = formatCursorInfo();
    if (!info.isEmpty()) cursorInfo += "; " + info;
    setText(cursorInfo);
}

void CursorPanel::setCursorX()
{
    Ori::Widgets::ValueEdit editor(_cursor->position().x());
    if (Ori::Dlg::showDialogWithPromptH("<b>X:", &editor, tr("Cursor Position")))
        _cursor->setPositionX(editor.value());
}

void CursorPanel::setCursorY()
{
    Ori::Widgets::ValueEdit editor(_cursor->position().y());
    if (Ori::Dlg::showDialogWithPromptH("<b>Y:", &editor, tr("Cursor Position")))
        _cursor->setPositionY(editor.value());
}

void CursorPanel::setCursorShape()
{
    if (actnCursorVert->isChecked())
        _cursor->setShape(QCPCursor::VerticalLine);
    else if (actnCursorHorz->isChecked())
        _cursor->setShape(QCPCursor::HorizontalLine);
    else
        _cursor->setShape(QCPCursor::CrossLines);
}

CursorPanel::Mode CursorPanel::mode() const
{
    if (actnCursorVert->isChecked())
        return Vertical;
    else if (actnCursorHorz->isChecked())
        return Horizontal;
    return Both;
}

void CursorPanel::setMode(CursorPanel::Mode mode)
{
    switch (mode)
    {
    case Both:
        _cursor->setShape(QCPCursor::CrossLines);
        actnCursorBoth->setChecked(true);
        break;
    case Vertical:
        _cursor->setShape(QCPCursor::VerticalLine);
        actnCursorVert->setChecked(true);
        break;
    case Horizontal:
        _cursor->setShape(QCPCursor::HorizontalLine);
        actnCursorHorz->setChecked(true);
        break;
    }
}

bool CursorPanel::enabled() const
{
    return _cursor->visible();
}

void CursorPanel::setEnabled(bool on)
{
    _cursor->setVisible(on);
    actnShowCursor->setChecked(on);
}
