#include "Utils.h"

#include <QStringList>

namespace Z {
namespace Utils {

QString generateLabel(const QString& prefix, const QStringList& existedLabels)
{
    int maxNum = 0;
    for (const QString& label : existedLabels)
        if (label.startsWith(prefix))
        {
            QStringRef ref(&label, prefix.length(), label.length() - prefix.length());
            bool isInt = false;
            int num = ref.toInt(&isInt);
            if (isInt && num > maxNum)
                maxNum = num;
        }
    return QString("%1%2").arg(prefix).arg(maxNum + 1);
}

} // namespace Utils
} // namespace Z
