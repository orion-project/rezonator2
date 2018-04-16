#include "testing/OriTestBase.h"
#include "../core/Elements.h"
#include "../core/Schema.h"
#include "../io/SchemaReaderIni.h"
#include "TestSchemaListener.h"
#include "TestUtils.h"

#include <QApplication>
#include <QFile>

namespace Z {
namespace Test {
namespace SchemaReaderIniTests {

#define TEST_FILE(var, file_name)\
    QString var = qApp->applicationDirPath() % "/test/" % file_name;\
    if (!QFile::exists(var))\
        ASSERT_FAIL("File not exists: " + var)

#define READ_AND_ASSERT(file_name)\
    TEST_FILE(fullFileName, file_name)\
    SchemaReaderIni file(&schema);\
    file.readFromFile(fullFileName);\
    LOG_SCHEMA_FILE(file)\
    ASSERT_IS_TRUE(file.ok())

#define READ_SCHEMA(file_name)\
    Schema schema;\
    READ_AND_ASSERT(file_name)

#define READ_AND_ASSERT_WARNINGS(file_name)\
    READ_AND_ASSERT(file_name)\
    ASSERT_IS_TRUE(file.report().hasWarnings())

#define ASSERT_PARAM(param_name, expected_value, expected_unit) {\
    Z::Parameter* p;\
    ASSERT_IS_NOT_NULL(p = elem->params().byAlias(param_name))\
    TEST_LOG(p->str())\
    double expected_value_in_units = expected_value;\
    if (p->dim() == Z::Dims::angular())\
        expected_value_in_units = expected_unit->fromSi(expected_value);\
    ASSERT_Z_VALUE_AND_UNIT(p->value(), expected_value_in_units, expected_unit)\
}

//------------------------------------------------------------------------------

TEST_METHOD(file_not_exists)
{
    SchemaReaderIni file(nullptr);
    file.readFromFile("some_not_existed_file_name");
    LOG_SCHEMA_FILE(file)
    ASSERT_IS_FALSE(file.ok())
}

TEST_CASE_METHOD(test_read_invalid_she_file, const QString& file_name)
{
    Schema s;
    TEST_FILE(fileName, file_name)
    SchemaReaderIni file(&s);
    ASSERT_IS_TRUE(file.ok())
    file.readFromFile(fileName);
    LOG_SCHEMA_FILE(file)
    ASSERT_IS_FALSE(file.ok())
}
TEST_CASE(read_invalid_section,         test_read_invalid_she_file, "invalid_section.she")
TEST_CASE(read_invalid_version_too_new, test_read_invalid_she_file, "invalid_version_new.she")
TEST_CASE(read_invalid_version_too_old, test_read_invalid_she_file, "invalid_version_old.she")

TEST_METHOD(read_invalid_lambda)
{
    Schema schema;
    schema.wavelength().setValue(Z::Value(640, Z::Units::nm()));
    READ_AND_ASSERT_WARNINGS("invalid_lambda.she")
    schema.wavelength().setValue(Z::Value(640, Z::Units::nm())); // value unchanged
}

TEST_METHOD(read_invalid_elem_type)
{
    Schema schema;
    READ_AND_ASSERT_WARNINGS("invalid_elem_type.she")
    ASSERT_IS_TRUE(schema.isEmpty())
}

TEST_METHOD(read_invalid_elem_section)
{
    Schema schema;
    READ_AND_ASSERT_WARNINGS("invalid_elem_section.she")
    ASSERT_EQ_INT(schema.elements().count(), 2)
}

TEST_METHOD(read_invalid_elem_param)
{
    Schema schema;
    READ_AND_ASSERT_WARNINGS("invalid_elem_param.she")
    ASSERT_EQ_INT(schema.elements().count(), 1)
}

//------------------------------------------------------------------------------

TEST_METHOD(read_general)
{
    TEST_FILE(fileName, "no_elems.she")
    SCHEMA_AND_LISTENER
    SchemaReaderIni file(&schema);
    file.readFromFile(fileName);
    LOG_SCHEMA_FILE(file)
    ASSERT_IS_TRUE(file.ok())
    ASSERT_IS_TRUE(schema.isEmpty())

    //ASSERT_EQ_STR(schema.title(), "Empty schema")
    //ASSERT_EQ_STR(schema.comment(), "Multi-line comment is not supported")
    ASSERT_EQ_INT(schema.tripType(), TripType::SP)
    ASSERT_Z_VALUE_AND_UNIT(schema.wavelength().value(), 1064, Z::Units::Ao())
}

TEST_CASE_METHOD(read_schema, const QString& fileName,
    Z::Unit expectedLinearUnit, Z::Unit expectedAngularUnit, Z::Unit expectedLambdaUnit,
    TripType expectedTripType)
{
    Z::Unit lin = expectedLinearUnit;
    Z::Unit ang = expectedAngularUnit;
    Z::Unit none = Z::Units::none();

    READ_SCHEMA(fileName)

    TEST_LOG(schema.wavelength().str())
    ASSERT_Z_VALUE_AND_UNIT(schema.wavelength().value(), 1064, expectedLambdaUnit)
    ASSERT_EQ_INT(schema.tripType(), expectedTripType)

    Element *elem;
    ASSERT_ELEMENT(0, EmptyRange, "L_1", "Empty range", false)
    ASSERT_PARAM("L", 55, lin);

    ASSERT_ELEMENT(1, Plate, "Cr_1", "Plate of matter", true)
    ASSERT_PARAM("L", 100, lin);
    ASSERT_PARAM("n", 1.2, none);
    //TODO: ASSERT_PARAM("D", 9);
    //TODO: check misalignments

    ASSERT_ELEMENT(2, FlatMirror, "M_1", "Flat mirror", true)
    //TODO: ASSERT_PARAM("D", 11);
    //TODO: check misalignments

    ASSERT_ELEMENT(3, CurveMirror, "M_2", "Spherical mirror", false)
    ASSERT_PARAM("R", 125, lin);
    ASSERT_PARAM("Alpha", 0.130899693899575, ang);
    //TODO: ASSERT_PARAM("D", 12.1);
    //TODO: check misalignments

    ASSERT_ELEMENT(4, ThinLens, "F_1", "Spherical lens", true)
    ASSERT_PARAM("F", 135.6, lin);
    ASSERT_PARAM("Alpha", 0.0959931088596881, ang);
    //TODO: ASSERT_PARAM("D", 18.1);
    //TODO: check misalignments

    ASSERT_ELEMENT(5, CylinderLensT, "F_c_t", "Cylindrical tangential lens", false)
    ASSERT_PARAM("F", 18, lin);
    ASSERT_PARAM("Alpha", 0.436332312998582, ang);
    //TODO: ASSERT_PARAM("D", 13);
    //TODO: check misalignments

    ASSERT_ELEMENT(6, CylinderLensS, "F_c_s", "Cylindrical sagittal lens", false)
    ASSERT_PARAM("F", 19, lin);
    ASSERT_PARAM("Alpha", -0.418879020478639, ang);
    //TODO: ASSERT_PARAM("D", 14);
    //TODO: check misalignments

    ASSERT_ELEMENT(7, TiltedCrystal, "Cr_2", "Tiltel crystal", true)
    ASSERT_PARAM("L", 13, lin);
    ASSERT_PARAM("n", 2.3, none);
    ASSERT_PARAM("Alpha", 0.314159265358979, ang);
    //TODO: ASSERT_PARAM("D", 11);
    //TODO: check misalignments

    ASSERT_ELEMENT(8, TiltedPlate, "Cr_3", "Tiltel plate", true)
    ASSERT_PARAM("L", 14, lin);
    ASSERT_PARAM("n", 2.2, none);
    ASSERT_PARAM("Alpha", 0.296705972839036, ang);
    //TODO: ASSERT_PARAM("D", 12);
    //TODO: check misalignments

    ASSERT_ELEMENT(9, BrewsterCrystal, "Cr_4", "Brewster crystal", false)
    ASSERT_PARAM("L", 15, lin);
    ASSERT_PARAM("n", 2.1, none);
    //TODO: ASSERT_PARAM("D", 13);
    //TODO: check misalignments

    ASSERT_ELEMENT(10, BrewsterPlate, "Cr_5", "Brewster plate", false)
    ASSERT_PARAM("L", 16, lin);
    ASSERT_PARAM("n", 2, none);
    //TODO: ASSERT_PARAM("D", 14);
    //TODO: check misalignments

    ASSERT_ELEMENT(11, MediumRange, "L_2", "Range in medium", true)
    ASSERT_PARAM("L", 56, lin);
    ASSERT_PARAM("n", 1.4, none);

    ASSERT_ELEMENT(12, Point, "Pt", "Point or plane", true)
    //TODO: check misalignments

    ASSERT_ELEMENT(13, Matrix, "Mat_1", "Matrix element", true)
    ASSERT_PARAM("At", 1.237, none);
    ASSERT_PARAM("Bt", 2.155, none);
    ASSERT_PARAM("Ct", 3.245, none);
    ASSERT_PARAM("Dt", 4.155, none);
    ASSERT_PARAM("As", 5.155, none);
    ASSERT_PARAM("Bs", 6.152, none);
    ASSERT_PARAM("Cs", 7.415, none);
    ASSERT_PARAM("Ds", 8.155, none);
    //TODO: check misalignments

    // TODO: check grin lens
}

TEST_CASE(read_schema_1_1, read_schema, "all_elems_ver-1-1.she", Z::Units::mm(), Z::Units::deg(), Z::Units::nm(), TripType::RR)
TEST_CASE(read_schema_1_2, read_schema, "all_elems_ver-1-2.she", Z::Units::mkm(), Z::Units::amin(), Z::Units::mkm(), TripType::SP)

//------------------------------------------------------------------------------

TEST_GROUP("SchemaReaderIni",
    ADD_TEST(file_not_exists),
    ADD_TEST(read_invalid_section),
    ADD_TEST(read_invalid_version_too_new),
    ADD_TEST(read_invalid_version_too_old),
    ADD_TEST(read_invalid_lambda),
    ADD_TEST(read_invalid_elem_type),
    ADD_TEST(read_invalid_elem_section),
    ADD_TEST(read_invalid_elem_param),
    ADD_TEST(read_general),
    ADD_TEST(read_schema_1_1),
    ADD_TEST(read_schema_1_2),
)

} // namespace SchemaReaderIniTests
} // namespace Test
} // namespace Z
