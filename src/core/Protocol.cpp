#include "Protocol.h"

#include <QApplication>
#include <QPlainTextEdit>
#include <QThread>

namespace Z {

static QPlainTextEdit* __logView = nullptr;
bool Protocol::isEnabled = false;
bool Protocol::isDebugEnabled = false;

void Protocol::setView(QPlainTextEdit* view)
{
    __logView = view;
    isEnabled = view != nullptr;
}

void Protocol::writeToHtmlLog()
{
    if (!__logView) return;
    
    QString msg = messageFormat().arg(sanitizedHtml());
    
    if (QThread::currentThread() != qApp->instance()->thread()) {
        QMetaObject::invokeMethod(qApp, [msg]{
            __logView->appendHtml(msg);
        });
    } else {
        __logView->appendHtml(msg);
    }
}

QString Protocol::sanitizedHtml() const
{
    return QString(_record)
            .replace(QStringLiteral("<"), QStringLiteral("&lt;"))
            .replace(QStringLiteral(">"), QStringLiteral("&gt;"))
            .replace(QStringLiteral("\n"), QStringLiteral("<br>"))
            .replace(QStringLiteral("    "), QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;"));
}

QString Protocol::messageFormat() const
{
    switch (_recordType)
    {
    case Report:  return QStringLiteral("<p><b>R: %1</b></p>");
    case Info:    return QStringLiteral("<p>I: %1</p>");
    case Note:    return QStringLiteral("<p><font color=gray>N: %1</font></p>");
    case Error:   return QStringLiteral("<p><font color=red>E: %1</font></p>");
    case Warning: return QStringLiteral("<p><font color=magenta>W: %1</font></p>");
    }
    return QString();
}

void Protocol::write(const QString& str)
{
    _record.append(str).append(' ');
}

} // namespace Z
