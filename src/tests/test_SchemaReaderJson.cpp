#include "../core/Schema.h"
#include "../io/SchemaReaderJson.h"
#include "../tests/TestUtils.h"

#include <QApplication>
#include <QFile>

namespace Z {
namespace Tests {
namespace SchemaReaderJsonTests {

#define READ_AND_ASSERT(file_name)\
    TEST_FILE(fullFileName, file_name)\
    SchemaReaderJson reader(&schema);\
    reader.readFromFile(fullFileName);\
    LOG_SCHEMA_READER(reader)\
    ASSERT_IS_FALSE(reader.report().hasErrors())

//------------------------------------------------------------------------------

#define READ_PUMPS\
    Schema schema;\
    READ_AND_ASSERT("test_various_pumps.rez")

#define ASSERT_PUMP_TYPE(index, type, label_text, title_text)\
    PumpParams_##type *pump = dynamic_cast<PumpParams_##type*>(schema.pumps()->at(index));\
    ASSERT_IS_NOT_NULL(pump)\
    ASSERT_EQ_STR(pump->label(), label_text)\
    ASSERT_EQ_STR(pump->title(), title_text)

#define ASSERT_PUMP_PARAM(param, expected_valueT, expected_valueS, expected_unit)\
    ASSERT_NEAR_DBL(pump->param()->value().rawValueT(), expected_valueT, 1e-6)\
    ASSERT_NEAR_DBL(pump->param()->value().rawValueS(), expected_valueS, 1e-6)\
    ASSERT_EQ_UNIT(pump->param()->value().unit(), Z::Units::expected_unit())

TEST_METHOD(read_pumps)
{
    READ_PUMPS
    ASSERT_EQ_INT(schema.pumps()->size(), 6)
    ASSERT_EQ_PTR(schema.activePump(), schema.pumps()->at(2))
}

TEST_METHOD(read_pumps_Waist)
{
    READ_PUMPS
    ASSERT_PUMP_TYPE(0, Waist, "P1", "waist")
    ASSERT_PUMP_PARAM(waist, 100, 120, mkm)
    ASSERT_PUMP_PARAM(distance, 99, 101, mm)
    ASSERT_PUMP_PARAM(MI, 1.1, 1.2, none)
}

TEST_METHOD(read_pumps_Front)
{
    READ_PUMPS
    ASSERT_PUMP_TYPE(1, Front, "P2", "front")
    ASSERT_PUMP_PARAM(beamRadius, 1001, 1002, mkm)
    ASSERT_PUMP_PARAM(frontRadius, 98, 102, mm)
    ASSERT_PUMP_PARAM(MI, 1.3, 1.4, none)
}

TEST_METHOD(read_pumps_RayVector)
{
    READ_PUMPS
    ASSERT_PUMP_TYPE(2, RayVector, "P3", "ray vector")
    ASSERT_PUMP_PARAM(radius, 1, 2, mm)
    ASSERT_PUMP_PARAM(angle, 10, 5, mrad)
    ASSERT_PUMP_PARAM(distance, 100, 120, mm)
}

TEST_METHOD(read_pumps_TwoSections)
{
    READ_PUMPS
    ASSERT_PUMP_TYPE(3, TwoSections, "P4", "two sections")
    ASSERT_PUMP_PARAM(radius1, 1.1, 2.1, mm)
    ASSERT_PUMP_PARAM(radius2, 2, 2.5, mm)
    ASSERT_PUMP_PARAM(distance, 12, 13, cm)
}

TEST_METHOD(read_pumps_Complex)
{
    READ_PUMPS
    ASSERT_PUMP_TYPE(4, Complex, "P5", "complex")
    ASSERT_PUMP_PARAM(real, 106, 107, mkm)
    ASSERT_PUMP_PARAM(imag, -23.5, -24.6, mkm)
    ASSERT_PUMP_PARAM(MI, 1.5, 1.6, none)
}

TEST_METHOD(read_pumps_InvComplex)
{
    READ_PUMPS
    ASSERT_PUMP_TYPE(5, InvComplex, "P6", "inv complex")
    ASSERT_PUMP_PARAM(real, 0.009, 0.008, mkm)
    ASSERT_PUMP_PARAM(imag, 0.003, 0.004, mkm)
    ASSERT_PUMP_PARAM(MI, 50, 60, none)
}

//------------------------------------------------------------------------------

TEST_METHOD(convert_lens_roc_inf)
{
    Schema schema;
    READ_AND_ASSERT("migrate_20_to_21.rez")
    auto elem = schema.elementByLabel("F1");
    auto r1 = elem->param("R1");
    auto r2 = elem->param("R2");
    ASSERT_IS_NOT_NULL(r1);
    ASSERT_IS_NOT_NULL(r2);
    ASSERT_IS_TRUE(qIsInf(r1->value().value()));
    ASSERT_IS_TRUE(qIsInf(r2->value().value()));
    // Units are not broken after changing value
    ASSERT_EQ_PTR(r1->value().unit(), Z::Units::mm());
    ASSERT_EQ_PTR(r2->value().unit(), Z::Units::m());
}

TEST_METHOD(convert_lens_roc_sign)
{
    Schema schema;
    READ_AND_ASSERT("migrate_20_to_21.rez")
    auto elem = schema.elementByLabel("F2");
    auto r1 = elem->param("R1");
    auto r2 = elem->param("R2");
    ASSERT_IS_NOT_NULL(r1);
    ASSERT_IS_NOT_NULL(r2);
    ASSERT_IS_TRUE(r1->value().value() > 0);
    ASSERT_IS_TRUE(r2->value().value() < 0);
    // Units are not broken after changing value
    ASSERT_EQ_PTR(r1->value().unit(), Z::Units::cm());
    ASSERT_EQ_PTR(r2->value().unit(), Z::Units::m());
}

TEST_METHOD(convert_intf_roc_sign)
{
    Schema schema;
    READ_AND_ASSERT("migrate_20_to_21.rez")
    auto e1 = schema.elementByLabel("s1");
    auto r1 = e1->param("R");
    ASSERT_IS_NOT_NULL(r1);
    ASSERT_IS_TRUE(r1->value().value() < 0);
    auto e2 = schema.elementByLabel("s2");
    auto r2 = e2->param("R");
    ASSERT_IS_NOT_NULL(r2);
    ASSERT_IS_TRUE(r2->value().value() > 0);
    // Units are not broken after changing value
    ASSERT_EQ_PTR(r1->value().unit(), Z::Units::mm());
    ASSERT_EQ_PTR(r2->value().unit(), Z::Units::cm());
}

//------------------------------------------------------------------------------

TEST_GROUP("SchemaReaderJson",
    ADD_TEST(read_pumps),
    ADD_TEST(read_pumps_Waist),
    ADD_TEST(read_pumps_Front),
    ADD_TEST(read_pumps_RayVector),
    ADD_TEST(read_pumps_TwoSections),
    ADD_TEST(read_pumps_Complex),
    ADD_TEST(read_pumps_InvComplex),
    ADD_TEST(convert_lens_roc_inf),
    ADD_TEST(convert_lens_roc_sign),
    ADD_TEST(convert_intf_roc_sign),
)

} // namespace SchemaReaderJsonTests
} // namespace Tests
} // namespace Z
