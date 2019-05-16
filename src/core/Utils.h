#ifndef Z_UTILS_H
#define Z_UTILS_H

#include <QString>
#include <QVariant>

namespace Z {
namespace Utils {

QString generateLabel(const QString& prefix, const QStringList& existedLabels);

} // namespace Utils
} // namespace Z


inline QVariant ptr2var(void* p)
{
    return qVariantFromValue(p);
}

template <typename T>
inline T var2ptr(const QVariant& v)
{
    static_assert(std::is_pointer<T>::value, "Template parameter must be a pointer type");
    return reinterpret_cast<T>(v.value<void*>());
}

#endif // Z_UTILS_H
