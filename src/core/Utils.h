#ifndef Z_UTILS_H
#define Z_UTILS_H

#include <QString>
#include <QVariant>

namespace Z {
namespace Utils {

QString generateLabel(const QString& prefix, const QStringList& existedLabels);

} // namespace Utils
} // namespace Z


inline QVariant ptr2var(const void* p)
{
    return QVariant::fromValue(const_cast<void*>(p));
}

template <typename T>
inline T var2ptr(const QVariant& v)
{
    static_assert(std::is_pointer<T>::value, "Template parameter must be a pointer type");
    return reinterpret_cast<T>(v.value<void*>());
}

template <typename TList> void swapItems(TList& list, int i, int j)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
    list.swapItemsAt(i, j);
#else
    list.swap(i, j);
#endif
}


#endif // Z_UTILS_H
