#include "Report.h"

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

} // namespace Z
