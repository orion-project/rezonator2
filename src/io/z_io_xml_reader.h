#ifndef Z_IO_XML_READER_H
#define Z_IO_XML_READER_H

#include <QString>

QT_BEGIN_NAMESPACE
class QDomElement;
class QDomDocument;
QT_END_NAMESPACE

#include "../core/Parameters.h"

namespace Z {
struct Variable;

namespace IO {
class Report;

namespace XML {

////////////////////////////////////////////////////////////////////////////////
/// Base methods for reading XML file.
///
class Reader
{
public:
    Reader(Z::IO::Report *report);
    ~Reader();

    void loadFromFile(const QString& fileName);
    void loadFromString(const QString& text);

    QDomDocument* document() const { return _doc; }

    bool getNode(QDomElement& root, const QString& name, QDomElement& node);

    /// Read content of text node (<root><name>content</name></root>)
    QString readText(QDomElement& root, const QString& name) const;

    double readDoubleAttributeDef(QDomElement& node, const QString& name, const double& def);
    bool readDoubleAttribute(QDomElement& node, const QString& name, double& value);
    bool readIntAttribute(QDomElement& node, const QString& name, int& value);
    int readIntAttributeDef(QDomElement& node, const QString& name, int def);
    bool readBoolAttributeDef(QDomElement& node, const QString& name, bool def);

    void readParameter(QDomElement& root, Z::Parameter *param);
    void readParameters(QDomElement& root, const QString& name, const Z::Parameters& params);

    void readValueTS(QDomElement& root, const QString& name, Z::PointTS& value);
    void readVariable(QDomElement& root, const QString& name, Z::Variable* var);

    void warning(const QDomElement& node, const QString& message);

private:
    QDomDocument *_doc;
    Z::IO::Report *_report;

    QString requiredAttribute(const QDomElement& node, const QString name);

    void errorXml(const QString& message, int line, int col);

    QString nodePath(const QDomElement& node) const;
};

} // namespace XML
} // namespace IO
} // namespace Z

#endif // Z_IO_XML_READER_H
