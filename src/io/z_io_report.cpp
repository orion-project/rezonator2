#include "z_io_report.h"

#include <QApplication>

namespace Z {
namespace IO {

void Report::error_Version(const Ori::Version& loading, const Ori::Version& max)
{
    error(qApp->translate("IO",
        "File version %1 is not supported, max supported version: %2").arg(loading.str(), max.str()));
}

void Report::error_Version(const Ori::Version& loading, const Ori::Version& min, const Ori::Version& max)
{
    error(qApp->translate("IO",
        "File version %1 is not supported, supported versions: %2 - %3").arg(loading.str(), min.str(), max.str()));
}

void Report::error_InvalidFile(const QString& reason)
{
    error(qApp->translate("IO", "Invalid schema file: %1").arg(reason));
}

void Report::warning_UnknownElem(const QString& elemType)
{
    warning(qApp->translate("IO", "Unknown element type '%1', element skipped").arg(elemType));
}

} // namespace IO
} // namespace Z
