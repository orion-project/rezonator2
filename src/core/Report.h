#ifndef Z_REPORT_H
#define Z_REPORT_H

#include <QString>
#include <QList>

namespace Z {

class Report
{
public:
    class Event
    {
    public:
        enum Type { Info, Warning, Error };
        static QString str(Type type);

        Event(Type type, const QString& message): _type(type), _message(message) {}

        Type type() const { return _type; }
        const QString& message() const { return _message;  }

        QString str() const;

    private:
        Type _type;
        QString _message;
    };

public:
    bool hasWarnings() const { return hasEvents(Event::Warning); }
    bool hasErrors() const { return hasEvents(Event::Error); }
    bool hasInfo() const { return hasEvents(Event::Info); }
    bool isEmpty() const { return _events.isEmpty(); }

    void info(const QString& message);
    void warning(const QString& message);
    void error(const QString& message);

    QString str() const;

    const QList<Event>& events() const { return _events; }

private:
    QList<Event> _events;

    bool hasEvents(Event::Type type) const;
};

} // namespace Z

#endif // Z_REPORT_H
