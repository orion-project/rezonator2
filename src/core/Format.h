#ifndef Z_FORMAT_H
#define Z_FORMAT_H

#include "Complex.h"

#include <QString>

namespace Z {

/// Formats a value for display in logs (qDebug and Protocol)
inline QString str(const double& v) { return QString::number(v, 'g', 16); }
/// Formats a value for display in logs (qDebug and Protocol)
QString str(const Complex& v);

/// Formats a value to be stored into file (settings files, schema files)
inline QString storedStr(const double& v) { return QString::number(v, 'g', 16); }

inline QString str(bool v) { return v? "true": "false"; }

/// Format a value to be displayed to user
QString format(const double& v);
/// Format a value to be displayed to user
QString format(const Complex& v);


namespace Strs {

#ifdef Q_CC_MSVC
// Qt Docs says:
// > Some compilers have bugs encoding strings containing characters outside the US-ASCII character set
// > Make sure you prefix your string with u in those cases
// u"" doesn't work either...
inline QString lambda() { return QString("λ"); }
inline QString alpha() { return QString("α"); }
inline QString theta() { return QString("θ"); }
#else
inline QString lambda() { return QStringLiteral("λ"); }
inline QString alpha() { return QStringLiteral("α"); }
inline QString theta() { return QStringLiteral("θ"); }
#endif

inline QString homePageUrl() { return "http://rezonator.orion-project.org"; }
inline QString sourcesUrl() { return "https://github.com/orion-project/rezonator2"; }
inline QString newIssueUrl() { return "https://github.com/orion-project/rezonator2/issues/new"; }
inline QString email() { return "rezonator@orion-project.org"; }

inline QChar multDot() { return QChar(0x00B7); }
inline QChar multX() { return QChar(0x00D7); }

inline QString appName() { return "<span style='color:navy'>re<span style='color:red'>Z</span>onator</span>"; }

inline QString appVersion()
{
    return QString("%1.%2.%3").arg(APP_VER_MAJOR).arg(APP_VER_MINOR).arg(APP_VER_PATCH);
}

inline QString appVersionYear() { return QString::number(APP_VER_YEAR); }

inline QString appVersionDate() { return QString("%1").arg(BUILDDATE); }

} // namespace Strs
} // namespace Z

#endif // Z_FORMAT_H
