#ifndef Z_IO_UTILS_H
#define Z_IO_UTILS_H

#include "core/OriVersion.h"

#include <QMetaEnum>

namespace Z {
namespace IO {
namespace Utils {

inline Ori::Version currentVersion() { return Ori::Version(2, 0); }
bool isOldSchema(const QString& fileName);
QString filtersForOpen();
QString filtersForSave();
inline QString suffix() { return QStringLiteral("rez"); }
inline QString suffixOld() { return QStringLiteral("she"); }
QString refineFileName(const QString& fileName, const QString &selectedFilter);
QString appendSuffix(const QString& fileName, const QString &selectedSuffix);
QString extractSuffix(const QString& filter);

template <typename TEnum> QString enumToStr(TEnum value)
{
    return QMetaEnum::fromType<TEnum>().key(value);
}

template <typename TEnum> TEnum enumFromStr(const QString& value, TEnum defaultValue)
{
    bool ok;
    int res = QMetaEnum::fromType<TEnum>().keyToValue(value.toLatin1().data(), &ok);
    return ok ? TEnum(res) : defaultValue;
}

} // namespace Utils
} // namespace IO
} // namespace Z

#endif // Z_IO_UTILS_H
