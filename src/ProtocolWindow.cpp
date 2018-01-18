#include "ProtocolWindow.h"
#include "core/Protocol.h"
#include "helpers/OriWidgets.h"

#include <QPlainTextEdit>

ProtocolWindow* ProtocolWindow::_instance = nullptr;

ProtocolWindow* ProtocolWindow::create()
{
    if (!_instance)
        _instance = new ProtocolWindow();
    return _instance;
}

ProtocolWindow::ProtocolWindow() : BasicMdiChild(InitOptions(initNoDefaultWidget))
{
    setTitleAndIcon(tr("Protocol", "Window title"), ":/toolbar/protocol");

    _log = new QPlainTextEdit;
    _log->setReadOnly(true);

    Ori::Gui::setFontMonospace(_log);

    setContent(_log);

    Z::Protocol::setView(_log);
}

ProtocolWindow::~ProtocolWindow()
{
    _instance = nullptr;

    Z::Protocol::setView(nullptr);
}
