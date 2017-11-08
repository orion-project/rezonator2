#ifndef Z_FORMAT_H
#define Z_FORMAT_H

#include <QString>

namespace Z {

/// Formats a value for display in logs (qDebug and Protocol)
inline QString str(const double& v) { return QString::number(v, 'g', 16); }

/// Formats a value to be stored into file (settings files, schema files)
inline QString storedStr(const double& v) { return QString::number(v, 'g', 16); }

inline QString str(bool v) { return v? "true": "false"; }

/// Format a value to be displayed to user
inline QString format(const double& v) { return QString::number(v); }

namespace Strs {

inline QString lambda() { return QStringLiteral("λ"); }
inline QString alpha() { return QStringLiteral("α"); }

inline QString homepage() { return "http://rezonator.orion-project.org"; }
inline QString email() { return "rezonator@orion-project.org"; }

inline QChar multDot() { return QChar(0x00B7); }
inline QChar multX() { return QChar(0x00D7); }

} // namespace Fmt
} // namespace Z

#endif // Z_FORMAT_H
