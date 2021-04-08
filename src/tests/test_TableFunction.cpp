#include "testing/OriTestBase.h"
#include "TestUtils.h"
#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../core/Utils.h"
#include "../io/SchemaReaderJson.h"
#include "../funcs/BeamParamsAtElemsFunction.h"
#include "../AppSettings.h"

#define READ_TEST_FILE(func_type, file_name, expected_result_count)\
    Schema schema;\
    bool oldSkip = AppSettings::instance().skipFuncWindowsLoading;\
    AppSettings::instance().skipFuncWindowsLoading = true;\
    TEST_FILE(fullFileName, file_name)\
    SchemaReaderJson reader(&schema);\
    reader.readFromFile(fullFileName);\
    AppSettings::instance().skipFuncWindowsLoading = oldSkip;\
    LOG_SCHEMA_READER(reader)\
    ASSERT_IS_FALSE(reader.report().hasErrors())\
    func_type func(&schema);\
    func.calcDebugResults = true;\
    func.calculate();\
    ASSERT_EQ_STR(func.errorText(), "")\
    ASSERT_IS_TRUE(func.ok())\
    for (const auto& r : func.results()) {TEST_LOG(r.str())}\
    ASSERT_EQ_INT(func.results().size(), expected_result_count)


#define ASSERT_TABLE_RES(index1, index2, ignore_sign) {\
    auto r1 = func.results().at(index1);\
    auto r2 = func.results().at(index2);\
    TEST_LOG(QString("Check {%1 %2} vs {%3 %4}").arg(\
        r1.element->displayLabel(), \
        TableFunction::resultPositionInfo(r1.position).ascii.trimmed(),\
        r2.element->displayLabel(), \
        TableFunction::resultPositionInfo(r2.position).ascii.trimmed()\
    ))\
    ASSERT_EQ_INT(r1.values.size(), r2.values.size())\
    for (int i = 0; i < r1.values.size(); i++) {\
        auto v1 = r1.values.at(i);\
        auto v2 = r2.values.at(i);\
        if (ignore_sign) {\
            ASSERT_EQ_DBL(qAbs(v1.T), qAbs(v2.T))\
            ASSERT_EQ_DBL(qAbs(v1.S), qAbs(v2.S))\
        } else {\
            ASSERT_EQ_DBL(v1.T, v2.T)\
            ASSERT_EQ_DBL(v1.S, v2.S)\
        }\
    }\
}

BOOL_PARAM(IgnoreSign)
INT_PARAM(ResultsCount)

//------------------------------------------------------------------------------

namespace Z {
namespace Tests {
namespace TableFunctionTests {

namespace BeamParamsAtElems {

TEST_CASE_METHOD(must_respect_medium_ior__mirrors_at_ends, QString fileName)
{
    READ_TEST_FILE(BeamParamsAtElemsFunction, fileName, ResultsCount(5))

    // First two results must be the same - left end mirror and origin of medium
    ASSERT_TABLE_RES(0, 1, IgnoreSign(false))

    // Last two results must be the same - right end mirror and ending of medium
    // It's ok that signs are different here because beam direction changes
    ASSERT_TABLE_RES(3, 4, IgnoreSign(true))
}

TEST_CASE(must_respect_medium_ior__sw__curved_ends__simple, must_respect_medium_ior__mirrors_at_ends, "calc_beamdata_elems_and_media__1_1.rez")
TEST_CASE(must_respect_medium_ior__sw__curved_ends__grin, must_respect_medium_ior__mirrors_at_ends, "calc_beamdata_elems_and_media__1_2.rez")
TEST_CASE(must_respect_medium_ior__sw__curved_ends__thermo, must_respect_medium_ior__mirrors_at_ends, "calc_beamdata_elems_and_media__1_3.rez")
TEST_CASE(must_respect_medium_ior__sw__curved_left_flat_right, must_respect_medium_ior__mirrors_at_ends, "calc_beamdata_elems_and_media__2_1.rez")
TEST_CASE(must_respect_medium_ior__sw__curved_right_flat_left, must_respect_medium_ior__mirrors_at_ends, "calc_beamdata_elems_and_media__2_2.rez")

TEST_CASE_METHOD(must_respect_medium_ior__elem_in_middle, QString fileName)
{
    READ_TEST_FILE(BeamParamsAtElemsFunction, fileName, ResultsCount(14))

    // The end of left-half medium must match left side of element
    ASSERT_TABLE_RES(5, 6, IgnoreSign(false))

    // Right side of element must match the start of right-half medium
    ASSERT_TABLE_RES(7, 8, IgnoreSign(false))
}

TEST_CASE(must_respect_medium_ior__sw__flat_in_middle, must_respect_medium_ior__elem_in_middle, "calc_beamdata_elems_and_media__3_1.rez")
TEST_CASE(must_respect_medium_ior__sw__lens_in_middle, must_respect_medium_ior__elem_in_middle, "calc_beamdata_elems_and_media__3_2.rez")

TEST_GROUP("BeamParamsAtElemsFunction",
           ADD_TEST(must_respect_medium_ior__sw__curved_ends__simple),
           ADD_TEST(must_respect_medium_ior__sw__curved_ends__grin),
           ADD_TEST(must_respect_medium_ior__sw__curved_ends__thermo),
           ADD_TEST(must_respect_medium_ior__sw__curved_left_flat_right),
           ADD_TEST(must_respect_medium_ior__sw__curved_right_flat_left),
           ADD_TEST(must_respect_medium_ior__sw__flat_in_middle),
           ADD_TEST(must_respect_medium_ior__sw__lens_in_middle),
           )
}

//------------------------------------------------------------------------------

TEST_GROUP("Table function",
           ADD_GROUP(BeamParamsAtElems),
           )

} // TableFunctionTests
} // Tests
} // Z
