#include <QtXml/QDomDocument>

#include "ISchemaStorable.h"
#include "SchemaWriterXml.h"
#include "z_io_utils.h"
#include "z_io_xml_writer.h"
#include "../WindowsManager.h"

SchemaWriterXml::SchemaWriterXml(Schema *schema, const QString& fileName) : SchemaFile(schema, fileName)
{
    _writer = new Z::IO::XML::Writer(&_report);
}

SchemaWriterXml::SchemaWriterXml(Schema *schema) : SchemaWriterXml(schema, QString())
{
}

SchemaWriterXml::~SchemaWriterXml()
{
    delete _writer;
}

void SchemaWriterXml::write()
{
    writeInternal();
    _writer->saveToFile(_fileName);
    _schema->setFileName(_fileName);
    _schema->events().raise(SchemaEvents::Saved);
}

QString SchemaWriterXml::writeToString()
{
    writeInternal();
    return _writer->saveToString();
}

void SchemaWriterXml::writeInternal()
{
    auto root = _writer->makeRoot("rezonatorSchema");
    root.setAttribute("version", Z::IO::Utils::currentVersion().str());

    writeGeneral(root);
// TODO:NEXT-VER writePump(root);
    writeElements(root);
    writeWindows(root);
}

void SchemaWriterXml::writeGeneral(QDomElement& root)
{
    _writer->writeText(root, "trip", TripTypes::info(_schema->tripType()).alias());
    _writer->writeParameter(root, &_schema->wavelength());
}

// TODO:NEXT-VER
#define WRITE_PUMP_MODE(mode, param1, param2, param3) \
{\
    auto node = document()->createElement(# mode);\
    writeValueTS(node, # param1, _schema->pumpParams().mode.param1);\
    writeValueTS(node, # param2, _schema->pumpParams().mode.param2);\
    writeValueTS(node, # param3, _schema->pumpParams().mode.param3);\
    nodePump.appendChild(node);\
}

void SchemaWriterXml::writePump(QDomElement& root)
{
    Q_UNUSED(root)

// TODO:NEXT-VER
//    auto nodePump = document()->createElement("pump");
//    nodePump.setAttribute("mode", ENUM_ITEM_NAME(Z::Pump::PumpMode, _schema->pump().mode));

//    WRITE_PUMP_MODE(waist, radius, distance, mi)
//    WRITE_PUMP_MODE(front, radius, curvature, mi)
//    WRITE_PUMP_MODE(complex, re, im, mi)
//    WRITE_PUMP_MODE(icomplex, re, im, mi)
//    WRITE_PUMP_MODE(vector, radius, angle, distance)
//    WRITE_PUMP_MODE(sections, radius_1, radius_2, distance)

//    root.appendChild(nodePump);
}

void SchemaWriterXml::writeElements(QDomElement& root)
{
    if (_schema->elements().isEmpty()) return;

    auto node = _writer->makeNode(root, "elements");
    for (Element *elem : _schema->elements())
        writeElement(node, elem);
}

void SchemaWriterXml::writeElement(QDomElement& root, Element *elem)
{
    auto nodeElem = _writer->makeNode(root, "element");
    nodeElem.setAttribute("type", elem->type());
    _writer->writeText(nodeElem, "label", elem->label());
    _writer->writeText(nodeElem, "title", elem->title());

    auto nodeOpts = _writer->makeNode(nodeElem, "options");
    nodeOpts.setAttribute("disabled", elem->disabled());

    if (elem->hasParams())
        _writer->writeParameters(nodeElem, "params", elem->params());

    // TODO:NEXT-VER: write misalignments
}

void SchemaWriterXml::writeWindows(QDomElement& root)
{
    auto windowsNode = _writer->makeNode(root, "windows");

    auto windows = WindowsManager::instance().schemaWindows(_schema);
    for (auto window : windows)
    {
        auto storable = dynamic_cast<ISchemaStorable*>(window);
        if (!storable) continue;

        auto node = _writer->makeNode(windowsNode, "window");
        node.setAttribute("type", storable->type());
        storable->write(_writer, node);
    }
}
