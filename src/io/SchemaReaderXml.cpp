#include <QApplication>
#include <QtXml/QDomDocument>

#include "ISchemaStorable.h"
#include "SchemaReaderXml.h"
#include "z_io_utils.h"
#include "z_io_xml_reader.h"
#include "../WindowsManager.h"
#include "../core/ElementsCatalog.h"

SchemaReaderXml::SchemaReaderXml(Schema *schema, const QString &fileName) : SchemaFile(schema, fileName)
{
    _reader = new Z::IO::XML::Reader(&_report);
}

SchemaReaderXml::~SchemaReaderXml()
{
    delete _reader;
}

void SchemaReaderXml::read()
{
    _reader->loadFromFile(_fileName);
    readInternal();
}

void SchemaReaderXml::read(const QString &text)
{
    _reader->loadFromString(text);
    readInternal();
}

void SchemaReaderXml::readInternal()
{
    if (!_report.ok()) return;

    auto root = _reader->document()->firstChildElement("rezonatorSchema");
    if (root.isNull())
        return _report.error_InvalidFile(qApp->translate("IO", "Wrong root node"));

    Ori::Version version(root.attribute("version"));
    if (version > Z::IO::Utils::currentVersion())
        return _report.error_Version(version, Z::IO::Utils::currentVersion());

    SchemaLoadingProcess p(_schema, _fileName);

    readGeneral(root);
    readPump(root);
    readElements(root);
    readStorables(root);
}

void SchemaReaderXml::readGeneral(QDomElement& root)
{
    _schema->setTripType(readTripType(root, "trip"));
    _reader->readParameter(root, &_schema->wavelength());
}

TripType SchemaReaderXml::readTripType(QDomElement root, const QString& name)
{
    bool ok;
    auto tripTypeStr = _reader->readText(root, name);
    auto tripType = TripTypes::find(tripTypeStr, &ok);
    if (!ok)
        _reader->warning(root, qApp->translate("IO",
            "Unknown schema trip type: '%1'. %2 is ised instead.")
                .arg(tripTypeStr, TripTypes::info(tripType).alias()));
    return tripType;
}

// TODO:NEXT-VER
#define READ_PUMP_MODE(mode, param1, param2, param3)\
{\
    auto node = nodePump.firstChildElement(#mode);\
    if (!node.isNull())\
    {\
        if (!_reader->readValueTS(node, #param1, pump.mode.param1) ||\
            !_reader->readValueTS(node, #param2, pump.mode.param2) ||\
            !_reader->readValueTS(node, #param3, pump.mode.param3))\
        {\
            report(Issue::BadPumpParams, #mode);\
            setDefaultPumpParams(_schema, pump, Z::PumpMode_## mode);\
        }\
    }\
    else setDefaultPumpParams(_schema, pump, Z::PumpMode_## mode);\
}

void SchemaReaderXml::readPump(QDomElement& root)
{
    Q_UNUSED(root)

// TODO:NEXT-VER
//    if (_schema->tripType() != Schema::TripType::SP)
//        disableReport(); // no messages for resonators

//    BREAKABLE_BLOCK
//    {
//        auto nodePump = root.firstChildElement("pump");
//        if (nodePump.isNull())
//        {
//            report(Issue::NoPump);
//            // TODO
//            //setDefaultPump(_schema);
//            break;
//        }

//        bool ok;
//        auto modeName = nodePump.attribute("mode");
//        auto mode = ENUM_ITEM_BY_NAME(Z::Pump::PumpMode, modeName, &ok);
//        if (!ok)
//        {
//            report(Issue::UnknownPumpMode, modeName);
//            // TODO
//            //setDefaultPump(_schema);
//            break;
//        }

//        /* TODO
//        Z::Pump::Params pump;
//        pump.mode = mode;
//        READ_PUMP_MODE(waist, radius, distance, mi)
//        READ_PUMP_MODE(front, radius, curvature, mi)
//        READ_PUMP_MODE(complex, re, im, mi)
//        READ_PUMP_MODE(icomplex, re, im, mi)
//        READ_PUMP_MODE(vector, radius, angle, distance)
//        READ_PUMP_MODE(sections, radius_1, radius_2, distance)
//        _schema->setPump(pump);*/
//    }
//    enableReport();
}

void SchemaReaderXml::readElements(QDomElement& root)
{
    auto elemsNode = root.firstChildElement("elements");
    for (auto node = elemsNode.firstChildElement("element");
         !node.isNull(); node = node.nextSiblingElement("element"))
    {
        readElement(node);
    }
}

void SchemaReaderXml::readElement(QDomElement &root)
{
    auto elemType = root.attribute("type");
    auto elem = ElementsCatalog::instance().create(elemType);
    if (!elem) return _report.warning_UnknownElem(elemType);

    _schema->insertElement(elem, -1, false);

    ElementLocker lock(elem);

    elem->setLabel(_reader->readText(root, "label"));
    elem->setTitle(_reader->readText(root, "title"));

    QDomElement node = root.firstChildElement("options");
    elem->setDisabled(_reader->readBoolAttributeDef(node, "disabled", false));

    if (elem->hasParams())
        _reader->readParameters(root, "params", elem->params());

    // TODO: read misalignments
}

void SchemaReaderXml::readStorables(QDomElement& root)
{
    auto windowsNode = root.firstChildElement("windows");
    for (auto node = windowsNode.firstChildElement("window");
         !node.isNull(); node = node.nextSiblingElement("window"))
    {
        auto type = node.attribute("type");
        auto ctor = WindowsManager::getConstructor(type);
        if (!ctor)
        {
            _reader->warning(node, qApp->translate("IO",
                "Unable to load window of unknown type '%1', skipped.").arg(type));
            continue;
        }

        SchemaWindow* window = ctor(schema());
        ISchemaStorable* storable = dynamic_cast<ISchemaStorable*>(window);
        if (!storable)
        {
            _reader->warning(node, qApp->translate("IO",
                "Window of type '%1' is found stored in file but no amymore known how to load it, skipped.").arg(type));
            continue;
        }

        if (storable->read(_reader, node))
        {
            WindowsManager::instance().show(window);
        }
        else
        {
            _reader->warning(node, qApp->translate("IO",
                "Window of type '%1' was not loaded.").arg(type));
            delete window;
        }
    }
}
