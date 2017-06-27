#include "Protocol.h"

#include <QPlainTextEdit>

namespace Z {

QPlainTextEdit* __logView;
bool Protocol::isEnabled;

void Protocol::setView(QPlainTextEdit* view)
{
    __logView = view;
    isEnabled = view;
}

Protocol::~Protocol()
{
    if (!__logView) return;

    switch (_recordType)
    {
    case Report: __logView->appendHtml("<b>" + _record + "</b>"); break;
    case Info: __logView->appendPlainText(_record); break;
    case Note: __logView->appendHtml("<font color=gray>" + _record + "</font>"); break;
    case Error: __logView->appendHtml("<font color=red>" + _record + "</font>"); break;
    case Warning: __logView->appendHtml("<font color=magenta>" + _record + "</font>"); break;
    }
}

} // namespace Z
