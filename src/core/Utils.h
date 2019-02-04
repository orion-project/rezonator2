#ifndef Z_UTILS_H
#define Z_UTILS_H

#include <QString>

namespace Z {
namespace Utils {

QString generateLabel(const QString& prefix, const QStringList& existedLabels);

} // namespace Utils
} // namespace Z

#endif // Z_UTILS_H
