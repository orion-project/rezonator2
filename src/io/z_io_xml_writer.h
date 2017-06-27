#ifndef Z_IO_XML_WRITER_H
#define Z_IO_XML_WRITER_H

#include <QString>

QT_BEGIN_NAMESPACE
class QDomElement;
class QDomDocument;
QT_END_NAMESPACE

#include "../core/Parameters.h"

namespace Z {
class Variable;

namespace IO {
class Report;

namespace XML {

////////////////////////////////////////////////////////////////////////////////
/// Base methods for writing XML file.
///
class Writer
{
public:
    Writer(Z::IO::Report *report);
    ~Writer();

    /// Save XML document to file
    void saveToFile(const QString &fileName);

    /// Save XML document to string
    QString saveToString();

    QDomDocument* document() const { return _doc; }

    /// Write content of text node (<root><name>value</name></root>)
    void writeText(QDomElement& root, const QString& name, const QString& value);

     /// Write value node: <root><name attr="value"/></root>
    void writeValueNode(QDomElement& root, const QString& name, const QString& attr, const QString& value);

    /// Write parameter with value as a new node inside of root
    /// (<root><param_name value="param.value" unit="param.value.unit"/></root>)
    void writeParameter(QDomElement& root, Z::Parameter* param);

    /// Write parameters collection as a new node "name" inside of root
    void writeParameters(QDomElement& root, const QString& name, const Z::Parameters& params);

    /// Write T and S values (<root><name T="value.T" S="value.S"></root>)
    void writeValueTS(QDomElement& root, const QString& name, const Z::ValueTS& value);

    void writeVariable(QDomElement& root, const QString& name, const Z::Variable* var);

    QDomElement makeRoot(const QString& name);
    QDomElement makeNode(QDomElement& root, const QString& name);

protected:
    /// Make standard string representation of double value
    inline QString string(const double& value) { return QString::number(value, 'g', 16); }

    /// Make standard string representation of integer value
    inline QString string(int value) { return QString::number(value); }

    /// Make standard string representation of boolean value
    inline QString string(bool value) { return value? QStringLiteral("true"): QStringLiteral("false"); }

private:
    QDomDocument *_doc;
    Z::IO::Report *_report;
};

} // namespace XML
} // namespace IO
} // namespace Z

#endif // Z_IO_XML_WRITER_H
