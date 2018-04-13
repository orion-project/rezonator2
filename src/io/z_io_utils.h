#ifndef Z_IO_UTILS_H
#define Z_IO_UTILS_H

#include "core/OriVersion.h"

namespace Z {
namespace IO {
namespace Utils {

inline Ori::Version currentVersion() { return Ori::Version(2, 0); }
bool isOldSchema(const QString& fileName);
QString filtersForOpen();
QString filtersForSave();
inline QString suffix() { return QStringLiteral("shex"); }
inline QString suffixOld() { return QStringLiteral("she"); }
QString refineFileName(const QString& fileName, const QString &selectedFilter);
QString appendSuffix(const QString& fileName, const QString &selectedSuffix);
QString extractSuffix(const QString& filter);

} // namespace Utils
} // namespace IO
} // namespace Z

#endif // Z_IO_UTILS_H
