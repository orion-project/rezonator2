#include "../core/Elements.h"
#include "../core/Schema.h"
#include "../io/SchemaReaderIni.h"
#include "TestUtils.h"

#include <QApplication>
#include <QFile>

namespace Z {
namespace Tests {
namespace SchemaReaderIniTests {

#define READ_AND_ASSERT(file_name)\
    TEST_FILE(fullFileName, file_name)\
    SchemaReaderIni reader(&schema);\
    reader.readFromFile(fullFileName);\
    LOG_SCHEMA_READER(reader)\
    ASSERT_IS_TRUE(reader.ok())

#define ASSERT_PARAM(param_name, expected_value, expected_unit) {\
    Z::Parameter* p;\
    ASSERT_IS_NOT_NULL(p = elem->params().byAlias(param_name))\
    TEST_LOG(p->str())\
    double expected_value_in_units = expected_value;\
    if (p->dim() == Z::Dims::angular())\
        expected_value_in_units = expected_unit->fromSi(expected_value);\
    ASSERT_EQ_ZVALUE(p->value(), Z::Value(expected_value_in_units, expected_unit))\
}

#define ASSERT_ELEMENT(elem_index, expected_type, expected_label, expected_title, expected_disabled) { \
    TEST_LOG_SEPARATOR\
    TEST_LOG(QString("Element #%1").arg(elem_index))\
    ASSERT_IS_TRUE(elem_index < schema.elements().size())\
    elem = schema.element(elem_index);\
    TEST_LOG(elem->type())\
    ASSERT_IS_TYPE(elem, Elem ## expected_type)\
    TEST_LOG(elem->displayLabelTitle())\
    ASSERT_EQ_STR(elem->label(), expected_label)\
    ASSERT_EQ_STR(elem->title(), expected_title)\
    ASSERT_IS_TRUE(elem->disabled() == expected_disabled)\
}

//------------------------------------------------------------------------------

TEST_METHOD(file_not_exists)
{
    SchemaReaderIni reader(nullptr);
    reader.readFromFile("some_not_existed_file_name");
    LOG_SCHEMA_READER(reader)
    ASSERT_IS_FALSE(reader.ok())
}

TEST_CASE_METHOD(test_read_invalid_she_file, const QString& file_name)
{
    Schema s;
    TEST_FILE(fileName, file_name)
    SchemaReaderIni reader(&s);
    ASSERT_IS_TRUE(reader.ok())
    reader.readFromFile(fileName);
    LOG_SCHEMA_READER(reader)
    ASSERT_IS_FALSE(reader.ok())
}
TEST_CASE(read_invalid_section,         test_read_invalid_she_file, "invalid_section.she")
TEST_CASE(read_invalid_version_too_new, test_read_invalid_she_file, "invalid_version_new.she")
TEST_CASE(read_invalid_version_too_old, test_read_invalid_she_file, "invalid_version_old.she")

TEST_METHOD(read_invalid_lambda)
{
    Schema schema;
    schema.wavelength().setValue(Z::Value(640, Z::Units::nm()));
    READ_AND_ASSERT("invalid_lambda.she")
    ASSERT_IS_TRUE(reader.report().hasWarnings())
    schema.wavelength().setValue(Z::Value(640, Z::Units::nm())); // value unchanged
}

TEST_METHOD(read_invalid_elem_type)
{
    Schema schema;
    READ_AND_ASSERT("invalid_elem_type.she")
    ASSERT_IS_TRUE(reader.report().hasWarnings())
    ASSERT_IS_TRUE(schema.isEmpty())
}

TEST_METHOD(read_invalid_elem_section)
{
    Schema schema;
    READ_AND_ASSERT("invalid_elem_section.she")
    ASSERT_IS_TRUE(reader.report().hasWarnings())
    ASSERT_EQ_INT(schema.elements().count(), 2)
}

TEST_METHOD(read_invalid_elem_param)
{
    Schema schema;
    READ_AND_ASSERT("invalid_elem_param.she")
    ASSERT_IS_TRUE(reader.report().hasWarnings())
    ASSERT_EQ_INT(schema.elements().count(), 1)
}

//------------------------------------------------------------------------------

TEST_METHOD(read_general)
{
    Schema schema;
    READ_AND_ASSERT("no_elems.she")
    ASSERT_IS_TRUE(schema.isEmpty())

    //TODO? ASSERT_EQ_STR(schema.title(), "Empty schema")
    //TODO? ASSERT_EQ_STR(schema.comment(), "Multi-line comment is not supported")
    ASSERT_EQ_INT(schema.tripType(), TripType::SP)
    ASSERT_EQ_ZVALUE(schema.wavelength().value(), 1064_Ao)
}

TEST_CASE_METHOD(read_schema, const QString& fileName,
    Z::Unit expectedLinearUnit, Z::Unit expectedAngularUnit, Z::Unit expectedLambdaUnit,
    TripType expectedTripType)
{
    Z::Unit lin = expectedLinearUnit;
    Z::Unit ang = expectedAngularUnit;
    Z::Unit none = Z::Units::none();

    Schema schema;
    READ_AND_ASSERT(fileName)

    TEST_LOG(schema.wavelength().str())
    ASSERT_EQ_ZVALUE(schema.wavelength().value(), Z::Value(1064, expectedLambdaUnit))
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

#define ASSERT_PUMP_PARAM(param, expected_valueT, expected_valueS, expected_unit)\
    ASSERT_NEAR_DBL(pump->param()->value().rawValueT(), expected_valueT, 1e-6)\
    ASSERT_NEAR_DBL(pump->param()->value().rawValueS(), expected_valueS, 1e-6)\
    ASSERT_EQ_UNIT(pump->param()->value().unit(), Z::Units::expected_unit())

#define ASSERT_PUMP(file_name, mode, param1, value1T, value1S, unit1, param2, value2T, value2S, unit2, param3, value3T, value3S, unit3)\
    Schema schema;\
    READ_AND_ASSERT(file_name)\
    ASSERT_IS_NOT_NULL(schema.activePump())\
    auto pump = dynamic_cast<Z::PumpParams_##mode*>(schema.activePump());\
    ASSERT_IS_NOT_NULL(pump)\
    ASSERT_PUMP_PARAM(param1, value1T, value1S, unit1)\
    ASSERT_PUMP_PARAM(param2, value2T, value2S, unit2)\
    ASSERT_PUMP_PARAM(param3, value3T, value3S, unit3)

TEST_METHOD(read_pump_waist)
{
    ASSERT_PUMP("pump_waist.she", Waist,
                waist, 120, 130, mkm,
                distance, 99, 101, mm,
                MI, 1.01, 1.02, none)
}

TEST_METHOD(read_pump_front)
{
    ASSERT_PUMP("pump_front.she", Front,
                beamRadius, 432.834155, 463.240637, mkm,
                frontRadius, 105.638714, 104.887779, mm,
                MI, 1.35, 1.45, none)
}

TEST_METHOD(read_pump_ray_vector)
{
    ASSERT_PUMP("pump_ray_vector.she", RayVector,
                radius, 2.5, 3.4, mm,
                angle, 5.5, 6.8, deg,
                distance, 0, 0, cm)
}

TEST_METHOD(read_pump_two_sections)
{
    ASSERT_PUMP("pump_two_sections.she", TwoSections,
                radius1, 9.51, 11.3, mm,
                radius2, 15.6, 17.99, mm,
                distance, 105, 104.9, cm)
}

TEST_METHOD(read_pump_complex)
{
    ASSERT_PUMP("pump_complex.she", Complex,
                real, 102.487, 103.556, mm,
                imag, -14.602, -11.347, mm,
                MI, 1.5, 1.7, none)
}

TEST_METHOD(read_pump_inv_complex)
{
    ASSERT_PUMP("pump_inv_complex.she", InvComplex,
                real, 0.009563, 0.009657, mm,
                imag, 0.001363, 0.001071, mm,
                MI, 1.5, 1.7, none)
}

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
    ADD_TEST(read_pump_waist),
    ADD_TEST(read_pump_front),
    ADD_TEST(read_pump_ray_vector),
    ADD_TEST(read_pump_two_sections),
    ADD_TEST(read_pump_complex),
    ADD_TEST(read_pump_inv_complex),
)

} // namespace SchemaReaderIniTests
} // namespace Tests
} // namespace Z
