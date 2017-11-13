#include "testing/OriTestBase.h"
#include "../io/z_io_report.h"
#include "../io/z_io_xml_reader.h"
#include "../io/z_io_xml_writer.h"
#include "TestUtils.h"

#include <QtXml/QDomDocument>

namespace Z {
namespace Test {
namespace xml_io_general_tests {

////////////////////////////////////////////////////////////////////////////////
class TestXmlIO
{
public:
    QDomElement root;

    const Z::IO::Report& report() const { return _testReport; }
    bool ok() const { return _testReport.ok(); }
    bool hasWarnings() const { return _testReport.hasWarnings(); }

protected:
    Z::IO::Report _testReport;
};

class TestXmlReader : public TestXmlIO, public Z::IO::XML::Reader
{
public:
    TestXmlReader(const QString& xml) : Z::IO::XML::Reader(&_testReport)
    {
        loadFromString(xml);
        root = document()->firstChildElement();
    }
};

class TestXmlWriter : public TestXmlIO, public Z::IO::XML::Writer
{
public:
    TestXmlWriter() : Z::IO::XML::Writer(&_testReport)
    {
        root = makeRoot("root");
    }

    QString save()
    {
        return saveToString();
    }
};

////////////////////////////////////////////////////////////////////////////////

TEST_METHOD(getNode)
{
    TestXmlReader file("<root><node>test content</node></root>");

    QDomElement node;
    bool res = file.getNode(file.root, "node", node);
    ASSERT_IS_TRUE(res);
    ASSERT_IS_FALSE(node.isNull());

    res = file.getNode(file.root, "abc", node);
    ASSERT_IS_FALSE(res);
    ASSERT_IS_TRUE(node.isNull());
}

TEST_METHOD(readText)
{
    TestXmlReader file("<root><node>test content</node></root>");

    auto content = file.readText(file.root, "node");
    ASSERT_EQ_STR(content, "test content");

    auto emptyContent = file.readText(file.root, "abc");
    ASSERT_IS_TRUE(emptyContent.isEmpty());
}

TEST_METHOD(readDoubleAttribute)
{
    TestXmlReader file("<root value1=\"3.14159265358979\" value2=\"3,14159265358979\"/>");

    double value;
    bool res = file.readDoubleAttribute(file.root, "value1", value);
    ASSERT_IS_TRUE(res);
    ASSERT_IS_TRUE(file.ok());
    ASSERT_IS_FALSE(file.hasWarnings());
    ASSERT_EQ_DBL(value, 3.14159265358979);

    value = 2.73;
    res = file.readDoubleAttribute(file.root, "value2", value);
    ASSERT_IS_FALSE(res);
    ASSERT_IS_TRUE(file.hasWarnings());
    ASSERT_EQ_DBL(value, 2.73);

    res = file.readDoubleAttribute(file.root, "value3", value);
    ASSERT_IS_FALSE(res);
    ASSERT_EQ_DBL(value, 2.73);

    QDomElement emptyNode;
    res = file.readDoubleAttribute(emptyNode, "value1", value);
    ASSERT_IS_FALSE(res);
    ASSERT_EQ_DBL(value, 2.73);
}

TEST_METHOD(readDoubleAttributeDef)
{
    TestXmlReader file("<root value1=\"3.14\" value2=\"3,14\"/>");

    double value = file.readDoubleAttributeDef(file.root, "value1", 2.73);
    ASSERT_IS_TRUE(file.ok());
    ASSERT_IS_FALSE(file.hasWarnings());
    ASSERT_EQ_DBL(value, 3.14);

    value = file.readDoubleAttributeDef(file.root, "value2", 2.731);
    ASSERT_IS_TRUE(file.hasWarnings());
    ASSERT_EQ_DBL(value, 2.731);

    value = file.readDoubleAttributeDef(file.root, "value3", 2.732);
    ASSERT_EQ_DBL(value, 2.732);

    QDomElement emptyNode;
    value = file.readDoubleAttributeDef(emptyNode, "value1", 2.733);
    ASSERT_EQ_DBL(value, 2.733);
}

TEST_METHOD(readIntAttribute)
{
    TestXmlReader file("<root value1=\"3\" value2=\"3.14\"/>");

    int value;
    bool res = file.readIntAttribute(file.root, "value1", value);
    ASSERT_IS_TRUE(res);
    ASSERT_IS_TRUE(file.ok());
    ASSERT_IS_FALSE(file.hasWarnings());
    ASSERT_EQ_DBL(value, 3);

    value = 2;
    res = file.readIntAttribute(file.root, "value2", value);
    ASSERT_IS_FALSE(res);
    ASSERT_IS_TRUE(file.hasWarnings());
    ASSERT_EQ_DBL(value, 2);

    res = file.readIntAttribute(file.root, "value3", value);
    ASSERT_IS_FALSE(res);
    ASSERT_EQ_DBL(value, 2);

    QDomElement emptyNode;
    res = file.readIntAttribute(emptyNode, "value1", value);
    ASSERT_IS_FALSE(res);
    ASSERT_EQ_DBL(value, 2);
}

TEST_METHOD(readIntAttributeDef)
{
    TestXmlReader file("<root value1=\"3\" value2=\"3.14\"/>");

    double value = file.readIntAttributeDef(file.root, "value1", 2);
    ASSERT_IS_TRUE(file.ok());
    ASSERT_IS_FALSE(file.hasWarnings());
    ASSERT_EQ_DBL(value, 3);

    value = file.readIntAttributeDef(file.root, "value2", 2);
    ASSERT_IS_TRUE(file.hasWarnings());
    ASSERT_EQ_DBL(value, 2);

    value = file.readIntAttributeDef(file.root, "value3", 4);
    ASSERT_EQ_DBL(value, 4);

    QDomElement emptyNode;
    value = file.readIntAttributeDef(emptyNode, "value1", 5);
    ASSERT_EQ_DBL(value, 5);
}

TEST_METHOD(readBoolAttributeDef)
{
    TestXmlReader file("<root "
                "value1=\"true\" "
                "value2=\"false\" "
                "value3=\"TRUE\" "
                "value4=\"False\" "
                "value5=\"3.14\" "
                "value6=\"3\" "
                "value7=\"0\" "
                "value8=\"-1\" "
                "value9=\"-11\" "
                "value10=\"Hallo World\" "
                "value11=\" \" "
                "/>");
    ASSERT_IS_TRUE(file.readBoolAttributeDef(file.root, "value1", false));
    ASSERT_IS_FALSE(file.readBoolAttributeDef(file.root, "value2", true));
    ASSERT_IS_TRUE(file.readBoolAttributeDef(file.root, "value3", false));
    ASSERT_IS_FALSE(file.readBoolAttributeDef(file.root, "value4", true));
    ASSERT_IS_TRUE(file.readBoolAttributeDef(file.root, "value5", true));
    ASSERT_IS_TRUE(file.readBoolAttributeDef(file.root, "value6", false));
    ASSERT_IS_FALSE(file.readBoolAttributeDef(file.root, "value7", true));
    ASSERT_IS_TRUE(file.readBoolAttributeDef(file.root, "value8", false));
    ASSERT_IS_TRUE(file.readBoolAttributeDef(file.root, "value9", false));
    ASSERT_IS_FALSE(file.readBoolAttributeDef(file.root, "value10", false));
    ASSERT_IS_TRUE(file.readBoolAttributeDef(file.root, "value11", true));
    ASSERT_IS_TRUE(file.readBoolAttributeDef(file.root, "value12", true));
}

////////////////////////////////////////////////////////////////////////////////

#define test_read_parameter_invalid_case(xml)\
    TestXmlReader file(xml);\
    \
    Z::Parameter p(Z::Dims::linear(), "L", "", "", "");\
    p.setValue(Z::Value(100, Z::Units::mm()));\
    ASSERT_Z_VALUE_AND_UNIT(p.value(), 100, Z::Units::mm())\
    \
    file.readParameter(file.root, &p);\
    LOG_SCHEMA_FILE(file)\
    ASSERT_Z_VALUE_AND_UNIT(p.value(), 100, Z::Units::mm()) // value unchanged

TEST_METHOD(readParameter_no_parameter_node) {
    test_read_parameter_invalid_case("<root><R value=\"3.14\" unit=\"cm\"/></root>")
}

TEST_METHOD(readParameter_no_value_attribute) {
    test_read_parameter_invalid_case("<root><L unit=\"cm\"/></root>")
}

TEST_METHOD(readParameter_no_unit_attribute) {
    test_read_parameter_invalid_case("<root><L value=\"3,14\"/></root>")
}

TEST_METHOD(readParameter_invalid_value) {
    test_read_parameter_invalid_case("<root><L value=\"3,14\" unit=\"cm\"/></root>")
}

TEST_METHOD(readParameter_invalid_unit) {
    test_read_parameter_invalid_case("<root><L value=\"3.14\" unit=\"cmcm\"/></root>")
}

TEST_METHOD(readParameter_unsuitable_unit) {
    test_read_parameter_invalid_case("<root><L value=\"3.14\" unit=\"deg\"/></root>")
}

TEST_METHOD(readParameter)
{
    TestXmlReader file("<root><L value=\"3.14\" unit=\"cm\"/></root>");

    Z::Parameter p(Z::Dims::linear(), "L", "", "", "");
    p.setValue(Z::Value(100, Z::Units::mm()));
    ASSERT_Z_VALUE_AND_UNIT(p.value(), 100, Z::Units::mm())

    file.readParameter(file.root, &p);
    ASSERT_Z_VALUE_AND_UNIT(p.value(), 3.14, Z::Units::cm())
}

TEST_METHOD(writeParameter_readParameter)
{
    Z::Parameter p(Z::Dims::linear(), "L", "", "", "");

    // initial value
    p.setValue(Z::Value(3.14, Z::Units::cm()));

    // save
    TestXmlWriter file1;
    file1.writeParameter(file1.root, &p);
    auto xml = file1.save();
    TEST_LOG(xml)

    // change value
    p.setValue(Z::Value(100, Z::Units::mm()));
    ASSERT_Z_VALUE_AND_UNIT(p.value(), 100, Z::Units::mm())

    // load
    TestXmlReader file2(xml);
    file2.readParameter(file2.root, &p);
    LOG_SCHEMA_FILE(file2)

    // value must be restored
    ASSERT_Z_VALUE_AND_UNIT(p.value(), 3.14, Z::Units::cm())
}

TEST_METHOD(writeParameters_readParameters)
{
    Z::Parameter p1(Z::Dims::linear(), "a", "", "", "");
    Z::Parameter p2(Z::Dims::angular(), "b", "", "", "");
    Z::Parameter p3(Z::Dims::none(), "c", "", "", "");
    Z::Parameters params({&p1, &p2, &p3});

    // initial values
    p1.setValue(Z::Value(3.14, Z::Units::mkm()));
    p2.setValue(Z::Value(2.15, Z::Units::amin()));
    p3.setValue(Z::Value(1.16, Z::Units::none()));

    // save
    TestXmlWriter file1;
    file1.writeParameters(file1.root, "params", params);
    auto xml = file1.save();
    TEST_LOG(xml)

    // change values
    p1.setValue(Z::Value(100, Z::Units::mm()));
    p2.setValue(Z::Value(200, Z::Units::deg()));
    p3.setValue(Z::Value(300, Z::Units::none()));

    // load
    TestXmlReader file2(xml);
    file2.readParameters(file2.root, "params", params);
    LOG_SCHEMA_FILE(file2)

    // values must be restored
    ASSERT_Z_VALUE_AND_UNIT(p1.value(), 3.14, Z::Units::mkm())
    ASSERT_Z_VALUE_AND_UNIT(p2.value(), 2.15, Z::Units::amin())
    ASSERT_Z_VALUE_AND_UNIT(p3.value(), 1.16, Z::Units::none())
}

////////////////////////////////////////////////////////////////////////////////

TEST_METHOD(readValueTS)
{
    TestXmlReader file("<root><node T=\"3.14\" S=\"2.73\"/></root>");

    Z::PointTS val;
    file.readValueTS(file.root, "node", val);
    ASSERT_VALUE_T_S(val, 3.14, 2.73);

    val.T = 1.2, val.S = 3.4;
    file.readValueTS(file.root, "node1", val);
    LOG_SCHEMA_FILE(file)
    ASSERT_VALUE_T_S(val, 1.2, 3.4) // value unchanged
}

TEST_METHOD(writeValueTS)
{
    TestXmlWriter writer;
    Z::PointTS saved1(M_PI, M_E);
    Z::PointTS saved2(M_PI*1e100, M_E*1e-100);
    Z::PointTS saved3(-M_PI*1e100, -M_E*1e-100);
    writer.writeValueTS(writer.root, "value1", saved1);
    writer.writeValueTS(writer.root, "value2", saved2);
    writer.writeValueTS(writer.root, "value3", saved3);

    auto content = writer.save();
    TEST_LOG(content)

    TestXmlReader reader(content);
    LOG_SCHEMA_FILE(reader)
    ASSERT_IS_TRUE(reader.ok())

    Z::PointTS loaded1, loaded2, loaded3;
    reader.readValueTS(reader.root, "value1", loaded1);
    reader.readValueTS(reader.root, "value2", loaded2);
    reader.readValueTS(reader.root, "value3", loaded3);

    ASSERT_VALUE_TS(loaded1, saved1)
    ASSERT_VALUE_TS(loaded2, saved2)
    ASSERT_VALUE_TS(loaded3, saved3)
}

TEST_METHOD(readVariable)
{
    ASSERT_FAIL("TODO") // TODO
}

////////////////////////////////////////////////////////////////////////////////

TEST_GROUP("Read/write general data",
    ADD_TEST(getNode),
    ADD_TEST(readText),
    ADD_TEST(readDoubleAttribute),
    ADD_TEST(readDoubleAttributeDef),
    ADD_TEST(readIntAttribute),
    ADD_TEST(readIntAttributeDef),
    ADD_TEST(readBoolAttributeDef),
    ADD_TEST(readParameter_no_parameter_node),
    ADD_TEST(readParameter_no_value_attribute),
    ADD_TEST(readParameter_no_unit_attribute),
    ADD_TEST(readParameter_invalid_value),
    ADD_TEST(readParameter_invalid_unit),
    ADD_TEST(readParameter_unsuitable_unit),
    ADD_TEST(readParameter),
    ADD_TEST(writeParameter_readParameter),
    ADD_TEST(writeParameters_readParameters),
    ADD_TEST(readValueTS),
    ADD_TEST(readVariable),
    ADD_TEST(writeValueTS),
)

} // namespace xml_io_general_tests
} // namespace Test
} // namespace Z
