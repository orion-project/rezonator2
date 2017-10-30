#include "testing/OriTestBase.h"
#include "../core/ElementsCatalog.h"
#include "../core/Schema.h"
#include "../io/ISchemaStorable.h"
#include "../io/SchemaReaderXml.h"
#include "../io/SchemaWriterXml.h"
#include "../io/z_io_xml_writer.h"
#include "../io/z_io_xml_reader.h"

#include "TestUtils.h"

#include <QtXml/QDomDocument>

namespace Z {
namespace Test {
namespace xml_io_schema_tests {

#define SAVE_SCHEMA(schema, fileName) {\
    SchemaWriterXml file(&schema, fileName);\
    file.write();\
    if (!file.report().IsEmpty())\
        test->logMessage(file.report().str());\
    ASSERT_IS_TRUE(file.ok());\
}

#define LOAD_SCHEMA(schema, fileName) {\
    SchemaReaderXml file(&schema, fileName);\
    file.read();\
    if (!file.report().IsEmpty())\
        test->logMessage(file.report().str());\
    ASSERT_IS_TRUE(file.ok());\
    ASSERT_IS_FALSE(schema.modified());\
}

//------------------------------------------------------------------------------

Z::Unit randomUnit(Z::Dim dim)
{
    return dim->units().at(qrand() % dim->units().size());
}

Element* createTestElem(Element* type)
{
    Element *elem = ElementsCatalog::instance().create(type->type());
    elem->setLabel(type->type());
    elem->setTitle(type->typeName());
    elem->setDisabled(true);
    for (Z::Parameter* p: elem->params())
        p->setValue(Z::Value(frand(-100, 100), randomUnit(p->dim())));
    // TODO assign misalignments
    return elem;
}

// TODO verify misalignments
#define ASSERT_EQUAL_ELEMS(elem1, elem2)                                        \
{                                                                               \
    test->logMessage(elem1->label());                                           \
    ASSERT_EQ_STR(elem1->type(), elem2->type());                                \
    ASSERT_EQ_STR(elem1->label(), elem2->label());                              \
    ASSERT_EQ_STR(elem1->title(), elem2->title());                              \
    ASSERT_EQ_INT(elem1->disabled(), elem2->disabled());                        \
    ASSERT_EQ_INT(elem1->params().size(), elem2->params().size());              \
    for (int __i = 0; __i < elem1->params().size(); __i++)                      \
    {                                                                           \
        auto __p1 = elem1->params().at(__i);                                    \
        auto __p2 = elem2->params().at(__i);                                    \
        test->logMessage(QString("%1: %2 ?= %3").arg(__p1->name())              \
                                                .arg(__p1->value().str())       \
                                                .arg(__p2->value().str()));     \
        ASSERT_EQ_DBL(__p1->value().value(), __p2->value().value());            \
        ASSERT_EQ_PTR(__p1->value().unit(), __p2->value().unit());              \
    }                                                                           \
}

/**
    Test saving and loading schema with all types of elements from new XML-based *.SHEX file.
    Test events during schema saving and loading.
*/
TEST_METHOD(save_and_load_all_elements)
{
    QString fileName("./test/_tmp_all_elems.shex");

    Schema schema1;
    for (Element* type: ElementsCatalog::instance().elements())
        schema1.insertElement(createTestElem(type), -1, false);
    SAVE_SCHEMA(schema1, fileName)

    Schema schema2;
    LOAD_SCHEMA(schema2, fileName)

    ASSERT_EQ_INT(schema2.elements().size(), schema1.elements().size());
    for (int i = 0; i < schema2.elements().size(); i++)
    {
        Element* elem1 = schema1.elements().at(i);
        Element* elem2 = schema2.elements().at(i);
        ASSERT_EQUAL_ELEMS(elem2, elem1);
    }
}

//------------------------------------------------------------------------------

namespace save_and_load_storable_clients {

/*class TestSchemaStorable : public ISchemaStorable
{
public:
    QString savedType, testValue;
    bool readSuccessWasCalled = false;

    QString type() const override { return "test-storable-type"; }

    void write(Z::IO::XML::Writer* writer, QDomElement& node) override
    {
        savedType = node.attribute("type");
        writer->writeText(node, "test-value-node", testValue);
    }

    bool read(Z::IO::XML::Reader* reader, QDomElement& node) override
    {
        testValue = reader->readText(node, "test-value-node");
        return true;
    }

    void readSuccess() override { readSuccessWasCalled = true; }

    static SchemaStorable* constructor(Schema*) { return new TestSchemaStorable; }
};*/

TEST_METHOD(success_scenario)
{
    ASSERT_FAIL("TODO")
    /*QString fileName("./test/_tmp_storables.shex");

    Schema schema1;
    TestSchemaStorable client1;
    client1.testValue = "test stored value";
    schema1.clients().append(&client1);
    SAVE_SCHEMA(schema1, fileName)
    ASSERT_EQ_STR(client1.savedType, client1.type())

    SchemaStorable::registerConstructor(client1.type(), &TestSchemaStorable::constructor);

    Schema schema2;
    LOAD_SCHEMA(schema2, fileName)
    auto clients2 = schema2.clients().get<TestSchemaStorable>();
    ASSERT_EQ_INT(clients2.size(), 1)
    auto client2 = clients2.first();
    ASSERT_EQ_STR(client2->testValue, client1.testValue)
    ASSERT_IS_TRUE(client2->readSuccessWasCalled);*/
}


bool __schemaStorable_readFailed = false;
bool __schemaStorable_destructed = false;

/*class TestSchemaStorableFailed : public ISchemaStorable
{
public:
    ~TestSchemaStorableFailed() { __schemaStorable_destructed = true; }

    QString type() const override { return "test-storable-faled-type"; }

    void write(Z::IO::XML::Writer*, QDomElement&) override {}
    bool read(Z::IO::XML::Reader*, QDomElement&) override { return false; }

    void readFailed() override { __schemaStorable_readFailed = true; }

    static SchemaStorable* constructor(Schema*) { return new TestSchemaStorableFailed; }
};*/

TEST_METHOD(failed_scenario)
{
    ASSERT_FAIL("TODO")
    /*QString fileName("./test/_tmp_storables_failed.shex");

    Schema schema1;
    TestSchemaStorableFailed client1;
    schema1.clients().append(&client1);
    SAVE_SCHEMA(schema1, fileName)

    SchemaStorable::registerConstructor(client1.type(), &TestSchemaStorableFailed::constructor);

    __schemaStorable_readFailed = false;
    __schemaStorable_destructed = false;
    Schema schema2;
    LOAD_SCHEMA(schema2, fileName)
    ASSERT_EQ_INT(schema2.clients().items().size(), 0)
    ASSERT_IS_TRUE(__schemaStorable_readFailed)
    ASSERT_IS_TRUE(__schemaStorable_destructed)*/
}

} // namespace SchemaStorableTests

//------------------------------------------------------------------------------

TEST_METHOD(schemaEventsSaveAndLoad)
{
    ASSERT_FAIL("TODO")
    /*ASSERT_IS_NOT_NULL(test);

    CString fileName = "./test/for_events.shex";

    Schema schema1;
    TestSchemaListener listener1;
    schema1.registerListener(&listener1);
    foreach (Element* type, ElementsCatalog::instance().elements(NULL))
        schema1.insertElement(createTestElem(type), -1, false, false);
    SchemaFileWriterXml file1(fileName);
    file1.write(&schema1);
    test->logMessage(file1.errors());
    ASSERT_IS_TRUE(file1.ok());
    ASSERT_IS_TRUE(listener1.saved);
    ASSERT_IS_TRUE(listener1.schema == &schema1);

    Schema schema2;
    TestSchemaListener listener2;
    schema2.registerListener(&listener2);
    SchemaFileReaderXml file2(fileName);
    file2.read(&schema2);
    test->logMessage(file2.errors());
    ASSERT_IS_TRUE(file2.ok());
    ASSERT_IS_TRUE(listener2.loading);
    ASSERT_IS_TRUE(listener2.loaded);
    ASSERT_IS_TRUE(listener2.schema == &schema2);
    ASSERT_IS_FALSE(listener2.elemCreated);
    ASSERT_IS_FALSE(listener2.elemChanged);
    ASSERT_IS_FALSE(schema2.modified());
    ASSERT_ARE_EQUAL(schema2.elements().size(), schema1.elements().size());
    for (int i = 0; i < schema2.elements().size(); i++)
        ASSERT_EQUAL_ELEMS(schema2.elements().at(i), schema1.elements().at(i));*/
}

TEST_METHOD(read_write_schema_version)
{
    ASSERT_FAIL("TODO")
}

//------------------------------------------------------------------------------

#define SET_PUMP_PARAMS(pump, mode, param1, param2, param3)\
{\
    double value = Z::PumpMode_ ## mode * M_E * 10 + 1;\
    pump.mode.param1.set(value * 1 * M_PI, value * 2 * M_PI);\
    pump.mode.param2.set(value * 3 * M_PI, value * 4 * M_PI);\
    pump.mode.param3.set(value * 5 * M_PI, value * 6 * M_PI);\
    TEST_LOG(QString("%1.%2 = %3").arg(#mode).arg(#param1).arg(pump.mode.param1.str()))\
    TEST_LOG(QString("%1.%2 = %3").arg(#mode).arg(#param2).arg(pump.mode.param2.str()))\
    TEST_LOG(QString("%1.%2 = %3").arg(#mode).arg(#param3).arg(pump.mode.param3.str()))\
}

#define ASSERT_PUMP_PARAMS(schema, expected, mode, param1, param2, param3)\
{\
    ASSERT_VALUE_TS(schema.pumpParams().mode.param1, expected.pumpParams().mode.param1)\
    ASSERT_VALUE_TS(schema.pumpParams().mode.param2, expected.pumpParams().mode.param2)\
    ASSERT_VALUE_TS(schema.pumpParams().mode.param3, expected.pumpParams().mode.param3)\
}

TEST_METHOD(write_read_pump)
{
    ASSERT_FAIL("TODO")
/* TODO
    Z::PumpParams pump;
    pump.mode = Z::PumpMode_complex;
    SET_PUMP_PARAMS(pump, waist, radius, distance, mi)
    SET_PUMP_PARAMS(pump, front, radius, curvature, mi)
    SET_PUMP_PARAMS(pump, complex, re, im, mi)
    SET_PUMP_PARAMS(pump, icomplex, re, im, mi)
    SET_PUMP_PARAMS(pump, vector, radius, angle, distance)
    SET_PUMP_PARAMS(pump, sections, radius_1, radius_2, distance)

    Schema schema1;
    schema1.setPumpParams(pump);


    SchemaWriterXml writer(&schema1);
    QString content;
    writer.write(&content);
    ASSERT_SCHEMA_FILE(writer)

    TEST_LOG(content)

    Schema schema2;
    SchemaReaderXml reader(&schema2);
    reader.read(content);
    ASSERT_SCHEMA_FILE(reader)

    ASSERT_EQ_INT(schema2.pumpParams().mode, schema1.pumpParams().mode)
    ASSERT_PUMP_PARAMS(schema2, schema1, waist, radius, distance, mi)
    ASSERT_PUMP_PARAMS(schema2, schema1, front, radius, curvature, mi)
    ASSERT_PUMP_PARAMS(schema2, schema1, complex, re, im, mi)
    ASSERT_PUMP_PARAMS(schema2, schema1, icomplex, re, im, mi)
    ASSERT_PUMP_PARAMS(schema2, schema1, vector, radius, angle, distance)
    ASSERT_PUMP_PARAMS(schema2, schema1, sections, radius_1, radius_2, distance)*/
}

TEST_METHOD(set_default_pump_when_no_pump_node)
{
    ASSERT_FAIL("TODO")
}

//------------------------------------------------------------------------------

TEST_GROUP("Read/write schema data",
    ADD_TEST(save_and_load_all_elements),
    ADD_TEST(save_and_load_storable_clients::success_scenario),
    ADD_TEST(save_and_load_storable_clients::failed_scenario),
    ADD_TEST(schemaEventsSaveAndLoad),
    ADD_TEST(read_write_schema_version),
    ADD_TEST(write_read_pump),
    ADD_TEST(set_default_pump_when_no_pump_node)
)

} // namespace xml_io_schema_tests
} // namespace Test
} // namespace Z


