#include "Format.h"

#include "../AppSettings.h"

namespace Z {

QString format(const double& v)
{
    return QString::number(v, 'g', AppSettings::instance().numberPrecisionData);
}

} // namespace Z
