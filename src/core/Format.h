#ifndef FORMAT_H
#define FORMAT_H

#include <QString>

namespace Z {

/// Formats a value for display in logs (qDebug and Protocol)
inline QString str(const double& v) { return QString::number(v, 'g', 16); }

inline QString str(bool v) { return v? "true": "false"; }

/// Format a value to be displayed to user
inline QString format(const double& v) { return QString::number(v); }

/// Format a named value with units to be displayed to user
//inline QString formatParam(const QString& name, const double& value, const QString& unit)
//{
//    return name % " = " % format(value) % unit;
//}

namespace Strs {

inline QString lambda() { return QStringLiteral("λ"); }
inline QString alpha() { return QStringLiteral("α"); }

inline QString homepage() { return "http://rezonator.orion-project.org"; }
inline QString email() { return "rezonator@orion-project.org"; }

inline QChar multDot() { return QChar(0x00B7); }
inline QChar multX() { return QChar(0x00D7); }

} // namespace Fmt
} // namespace Z

#endif // FORMAT_H
