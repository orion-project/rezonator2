#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "Format.h"

#include <QDebug>

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
    inline Protocol& operator << (const std::string& v) { write(QString::fromStdString(v)); return *this; }

    void write(const QString& str);

private:
    QString _record;
    RecordType _recordType;

    QString sanitizedHtml() const;
    QString messageFormat() const;
    void writeToHtmlLog();
};

} // namespace Z

#define Z_REPORT(p) { \
    qDebug() << p; \
    if (Z::Protocol::isEnabled) { Z::Protocol(Z::Protocol::Report) << p; } \
}

#define Z_INFO(p) {\
    qInfo() << p; \
    if (Z::Protocol::isEnabled) { Z::Protocol(Z::Protocol::Info) << p; } \
}

#define Z_WARNING(p) { \
    qWarning() << p; \
    if (Z::Protocol::isEnabled) { Z::Protocol(Z::Protocol::Warning) << p; } \
}

#define Z_ERROR(p) { \
    qCritical() << p; \
    if (Z::Protocol::isEnabled) { Z::Protocol(Z::Protocol::Error) << p; } \
}

#endif // PROTOCOL_H
