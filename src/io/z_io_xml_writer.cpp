#include <QFile>
#include <QTextStream>
#include <QtXml/QDomDocument>

#include "z_io_report.h"
#include "z_io_xml_writer.h"
#include "../core/Variable.h"

namespace Z {
namespace IO {
namespace XML {

Writer::Writer(IO::Report *report) : _report(report)
{
    _doc = new QDomDocument();
}

Writer::~Writer()
{
    delete _doc;
}

void Writer::saveToFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return _report->error(file.errorString());
    QTextStream stream(&file);
    _doc->save(stream, 4);
    file.close();
}

QString Writer::saveToString()
{
    QString text;
    QTextStream stream(&text);
    _doc->save(stream, 4);
    return text;
}

QDomElement Writer::makeRoot(const QString& name)
{
    auto root = _doc->createElement(name);
    document()->appendChild(root);
    return root;
}

QDomElement Writer::makeNode(QDomElement& root, const QString& name)
{
    auto node = _doc->createElement(name);
    root.appendChild(node);
    return node;
}

void Writer::writeText(QDomElement& root, const QString& name, const QString& value)
{
    auto node = makeNode(root, name);
    node.appendChild(_doc->createTextNode(value));
}

void Writer::writeValueNode(QDomElement& root, const QString& name, const QString& attr, const QString& value)
{
    auto node = makeNode(root, name);
    node.setAttribute(attr, value);
}

void Writer::writeParameter(QDomElement& root, Z::Parameter *param)
{
    auto node = makeNode(root, param->alias());
    node.setAttribute("value", string(param->value().value()));
    node.setAttribute("unit", param->value().unit()->alias());
}

void Writer::writeParameters(QDomElement& root, const QString& name, const Z::Parameters& params)
{
    auto node = makeNode(root, name);
    for (Z::Parameter* param : params)
        writeParameter(node, param);
}

void Writer::writeValueTS(QDomElement& root, const QString &name, const Z::PointTS& value)
{
    auto node = makeNode(root, name);
    node.setAttribute("T", string(value.T));
    node.setAttribute("S", string(value.S));
}

void Writer::writeVariable(QDomElement& root, const QString& name, const Z::Variable *var)
{
    Q_UNUSED(root)
    Q_UNUSED(name)
    Q_UNUSED(var)
// TODO:NEXT-VER
//    auto node = makeNode(root, name);
//    writeValueNode(node, "element", "index", string(var->element));
//    writeValueNode(node, "param", "index", string(var->param));
//    writeValueNode(node, "start", "value", string(var->start));
//    writeValueNode(node, "stop", "value", string(var->stop));
//    writeValueNode(node, "step", "value", string(var->step));
//    writeValueNode(node, "points", "value", string(var->points));
//    writeValueNode(node, "useStep", "value", string(var->useStep));
}

} // namespace XML
} // namespace IO
} // namespace Z
