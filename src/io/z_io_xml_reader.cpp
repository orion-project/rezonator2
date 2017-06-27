#include <QApplication>
#include <QFile>
#include <QtXml/QDomDocument>

#include "z_io_xml_reader.h"
#include "z_io_report.h"
#include "../core/Variable.h"

namespace Z {
namespace IO {
namespace XML {

Reader::Reader(Z::IO::Report *report) : _report(report)
{
    _doc = new QDomDocument();
}

Reader::~Reader()
{
    delete _doc;
}

void Reader::loadFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return _report->error(file.errorString());
    QString errorMsg;
    int errorLine, errorCol;
    if (!_doc->setContent(&file, false, &errorMsg, &errorLine, &errorCol))
        errorXml(errorMsg, errorLine, errorCol);
    file.close();
}

void Reader::loadFromString(const QString& text)
{
    QString errorMsg;
    int errorLine, errorCol;
    if (!_doc->setContent(text, false, &errorMsg, &errorLine, &errorCol))
        errorXml(errorMsg, errorLine, errorCol);
}

void Reader::errorXml(const QString& message, int line, int col)
{
    _report->error(qApp->translate("IO", "Error in file at line %1, column %2: %3").arg(line).arg(col).arg(message));
}

void Reader::warning(const QDomElement& node, const QString& message)
{
    _report->warning(message % "\n    " % nodePath(node));
}

QString Reader::nodePath(const QDomElement& node) const
{
    auto path = node.nodeName();
    auto parent = node.parentNode();
    while (!parent.isNull())
    {
        path = parent.nodeName() + "/" + path;
        parent = parent.parentNode();
    }
    return path;
}

bool Reader::getNode(QDomElement& root, const QString& name, QDomElement& node)
{
    node = root.firstChildElement(name);
    bool ok = !node.isNull();
    if (!ok)
        warning(root, qApp->translate("IO", "Unable to find node <%1> in <%2>").arg(name, root.nodeName()));
    return ok;
}

#define GET_NODE(var, root, name)\
    QDomElement var; if (!getNode(root, name, var)) return;

QString Reader::requiredAttribute(const QDomElement& node, const QString name)
{
    if (!node.hasAttribute(name))
    {
        warning(node, qApp->translate("IO", "Required attribute not found: '%1'").arg(name));
        return QString();
    }
    return name;
}

#define REQUIRED_ATTRIBUTE(var, node, name)\
    auto var = requiredAttribute(node, name); if (var.isEmpty()) return;

QString Reader::readText(QDomElement& root, const QString& name) const
{
    auto node = root.firstChildElement(name);
    return !node.isNull()? node.text(): QString();
}

#define CONVERT_ATTRIBUTE(node, name, method, result)\
    bool ok; auto tmp = node.attribute(name).method(&ok);\
    if (!ok) warning(node, qApp->translate("IO", "Invalid value for attribute '%1'").arg(name));\
    else result = tmp;\
    return ok;

bool Reader::readDoubleAttribute(QDomElement& node, const QString& name, double& value)
{
    CONVERT_ATTRIBUTE(node, name, toDouble, value)
}

double Reader::readDoubleAttributeDef(QDomElement& node, const QString& name, const double& def)
{
    double value;
    return readDoubleAttribute(node, name, value)? value: def;
}

bool Reader::readIntAttribute(QDomElement& node, const QString& name, int& value)
{
    CONVERT_ATTRIBUTE(node, name, toInt, value)
}

int Reader::readIntAttributeDef(QDomElement& node, const QString& name, int def)
{
    int value;
    return readIntAttribute(node, name, value)? value: def;
}

bool Reader::readBoolAttributeDef(QDomElement& node, const QString& name, bool def)
{
    auto s = node.attribute(name);
    if (s.compare("true", Qt::CaseInsensitive) == 0) return true;
    if (s.compare("false", Qt::CaseInsensitive) == 0) return false;
    return readIntAttributeDef(node, name, def);
}

void Reader::readParameter(QDomElement& root, Z::Parameter *param)
{
    GET_NODE(node, root, param->alias())
    REQUIRED_ATTRIBUTE(value_attr, node, "value")
    REQUIRED_ATTRIBUTE(unit_attr, node, "unit")

    auto unit_str = node.attribute(unit_attr);
    auto unit = param->dim()->unitByAlias(unit_str);
    if (!unit)
        return warning(node, qApp->translate("IO",
            "Unit '%1' is unacceptable for parameter %2").arg(unit_str, param->alias()));

    double tmp;
    if (!readDoubleAttribute(node, value_attr, tmp)) return;
    Z::Value value(tmp, unit);
    auto res = param->verify(value);
    if (!res.isEmpty())
        return warning(node, qApp->translate("IO",
            "Value %1 is unacceptable for parameter %2: %3").arg(value.str(), param->alias(), res));
    param->setValue(value);
}

void Reader::readParameters(QDomElement& root, const QString& name, const Z::Parameters& params)
{
    GET_NODE(node, root, name)
    for (Z::Parameter *param : params)
        readParameter(node, param);
}

void Reader::readValueTS(QDomElement& root, const QString& name, Z::ValueTS& value)
{
    GET_NODE(node, root, name)
    readDoubleAttribute(node, "T", value.T);
    readDoubleAttribute(node, "S", value.S);
}

void Reader::readVariable(QDomElement& root, const QString& name, Z::Variable* var)
{
    GET_NODE(node, root, name)
    // TODO
}

} // namespace XML
} // namespace IO
} // namespace Z
