#include "Report.h"

#include <QDebug>
#include <QStringList>

namespace Z {

QString Report::Event::str(Type type)
{
    switch (type)
    {
    case Info:    return QString(QStringLiteral("Info"));
    case Warning: return QString(QStringLiteral("Warning"));
    case Error:   return QString(QStringLiteral("Error"));
    }
    return QString();
}

QString Report::Event::str() const
{
    return str(_type) % ": " % _message;
}

bool Report::hasEvents(Event::Type type) const
{
    for (const Event& event : _events)
        if (event.type() == type)
            return true;
    return false;
}

void Report::info(const QString& message)
{
    _events.append(Event(Event::Info, message));
}

void Report::warning(const QString& message)
{
    _events.append(Event(Event::Warning, message));
}

void Report::error(const QString& message)
{
    _events.append(Event(Event::Error, message));
}

QString Report::str() const
{
    QStringList s;
    for (const Event& event : _events)
        s << event.str();
    return s.join("\n");
}

void Report::writeToStdout() const
{
    for (const Z::Report::Event& event: events())
    {
        QString m = event.message();
        switch (event.type())
        {
        case Z::Report::Event::Info: qInfo() << m; break;
        case Z::Report::Event::Warning: qWarning() << m; break;
        case Z::Report::Event::Error: qCritical() << m; break;
        }
    }
}

} // namespace Z
