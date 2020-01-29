#include "Protocol.h"

#include <QDebug>
#include <QPlainTextEdit>

namespace Z {

static QPlainTextEdit* __logView;
bool Protocol::isEnabled;

void Protocol::setView(QPlainTextEdit* view)
{
    __logView = view;
    isEnabled = view;
}

void Protocol::writeToHtmlLog()
{
    if (__logView)
        __logView->appendHtml(messageFormat().arg(sanitizedHtml()));
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
    qDebug() << str;
    _record.append(str).append(' ');
}

} // namespace Z
