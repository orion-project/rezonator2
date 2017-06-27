#ifndef Z_IO_REPORT_H
#define Z_IO_REPORT_H

#include "core/OriVersion.h"
#include "../core/Report.h"

namespace Z {
namespace IO {

class Report : public Z::Report
{
public:
    bool ok() const  { return !hasErrors(); }
    void error_Version(const Ori::Version& loading, const Ori::Version& max);
    void error_Version(const Ori::Version& loading, const Ori::Version& min, const Ori::Version& max);
    void error_InvalidFile(const QString& reason);
    void warning_UnknownElem(const QString& elemType);
};

} // namespace IO
} // namespace Z

#endif // Z_IO_REPORT_H
