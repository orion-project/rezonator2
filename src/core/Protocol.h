#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "Format.h"

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
QT_END_NAMESPACE

namespace Z {

class Protocol
{
public:
    static bool isEnabled;
    static void setView(QPlainTextEdit* view);

    enum RecordType { Report, Info, Note, Error, Warning };

public:
    Protocol(RecordType recordType): _recordType(recordType) {}
    ~Protocol() { writeToHtmlLog(); }

    inline Protocol& operator << (const char* v) { write(v); return *this; }
    inline Protocol& operator << (const QString& v) { write(v); return *this; }
    inline Protocol& operator << (const double& v) { write(Z::str(v)); return *this; }
    inline Protocol& operator << (int v) { write(QString::number(v)); return *this; }
    inline Protocol& operator << (long v) { write(QString::number(v)); return *this; }
    inline Protocol& operator << (long long v) { write(QString::number(v)); return *this; }
    inline Protocol& operator << (bool v) { write(v? QStringLiteral("true"): QStringLiteral("false")); return *this; }

    inline void write(const QString& str) { _record.append(str).append(' '); }

private:
    QString _record;
    RecordType _recordType;

    QString sanitizedHtml() const;
    QString messageFormat() const;
    void writeToHtmlLog();
};

} // namespace Z

// TODO: not sure what difference between REPORT, INFO and NOTE. Seems to be overkill.
#define Z_REPORT(p) if (Z::Protocol::isEnabled) { Z::Protocol(Z::Protocol::Report) << p; }
#define Z_INFO(p) if (Z::Protocol::isEnabled) { Z::Protocol(Z::Protocol::Info) << p; }
#define Z_NOTE(p) if (Z::Protocol::isEnabled) { Z::Protocol(Z::Protocol::Note) << p; }
#define Z_ERROR(p) if (Z::Protocol::isEnabled) { Z::Protocol(Z::Protocol::Error) << p; }
#define Z_WARNING(p) if (Z::Protocol::isEnabled) { Z::Protocol(Z::Protocol::Warning) << p; }

#endif // PROTOCOL_H
