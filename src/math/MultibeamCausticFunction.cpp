#include "MultibeamCausticFunction.h"

#include "../core/Pump.h"

MultibeamCausticFunction::MultibeamCausticFunction(Schema *schema) : MultirangeCausticFunction(schema)
{
}

QString MultibeamCausticFunction::calculateSpecPoints(const SpecPointParams& params)
{
    QString report;
    QTextStream stream(&report);
    auto p = pump();
    stream << "<p><span class='title'>" << p->label();
    if (auto title = p->title(); !title.isEmpty())
        stream << " (" + title << ')';
    stream << "</span><br>";
    stream << p->displayStr().replace("; ", "<br>");
    stream << "<hr>";
    stream << MultirangeCausticFunction::calculateSpecPoints(params);
    return report;
}
