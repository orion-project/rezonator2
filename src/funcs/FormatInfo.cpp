#include "../core/Element.h"
#include "../core/Format.h"

#include <QApplication>

namespace Z {
namespace Format {

QString matrix(const Z::Matrix& m)
{
    const char *matrixFmtString =
        "<table border=1 cellpadding=5 cellspacing=0>"
            "<tr><td align=right>%1</td><td align=right>%2</td></tr>"
            "<tr><td align=right>%3</td><td align=right>%4</td></tr>"
        "</table>";
    return QString::fromLatin1(matrixFmtString).arg(
        Z::format(m.A), Z::format(m.B), Z::format(m.C), Z::format(m.D));
}

QString matrices(const Z::Matrix& mt, const Z::Matrix& ms)
{
    const char *matricesFmtString =
        "<table cellpadding=6 valign=middle><tr>"
            "<td><b>M<sub>T</sub>&nbsp;&nbsp;=</b></td>"
            "<td>%1</td>"
            "<td>&nbsp;</td>"
            "<td><b>M<sub>S</sub>&nbsp;&nbsp;=</b></td>"
            "<td>%2</td>"
        "</tr></table>";
    return  QString::fromLatin1(matricesFmtString).arg(matrix(mt), matrix(ms));
}

QString linkViewMatrix(Element *elem)
{
    return QStringLiteral("<a href='func://viewmatrix?elem=%1'>%2</a>")
            .arg(elem->id()).arg(elem->displayLabel());
}

QString roundTrip(const Elements &elems, bool hyperlinks)
{
    QString result = "";
    for (int i = 0; i < elems.size(); i++)
    {
        Element *elem = elems[i];
        result += (hyperlinks? linkViewMatrix(elem): elem->displayLabel())
                % ' ' % Z::Strs::multDot() % ' ';
    }
    result.truncate(result.length()-2); // remove last "* "
    return result;
}

QString elementTitle(Element *elem)
{
    QString disabled;
    if (elem->disabled()) disabled = qApp->translate("Z::Format", "(disabled)");
    return QStringLiteral("<span style='color: %1'>%2%3</span>").arg(
        elem->disabled()? "gray": "navy", elem->displayLabelTitle(), disabled);
}

QString elementTitleAndMatrices(Element *elem)
{
    QString result;
    if (elem->disabled())
        result += "<div style='color: gray'>";
    result += elementTitle(elem) + matrices(elem->Mt(), elem->Ms());
    if (elem->disabled())
        result += "</div>";
    return result;
}

} // namespace Format
} // namespace Z

