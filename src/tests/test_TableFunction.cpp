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
    ASSERT_IS_TRUE(func.ok())\
    ASSERT_EQ_STR(func.errorText(), "")\
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

TEST_CASE(must_respect_medium_ior__curved_mirrors_at_ends__simple, must_respect_medium_ior__mirrors_at_ends, "calc_beamdata_elems_and_media__1_1.rez")
TEST_CASE(must_respect_medium_ior__curved_mirrors_at_ends__grin, must_respect_medium_ior__mirrors_at_ends, "calc_beamdata_elems_and_media__1_2.rez")
TEST_CASE(must_respect_medium_ior__curved_mirrors_at_ends__thermo, must_respect_medium_ior__mirrors_at_ends, "calc_beamdata_elems_and_media__1_3.rez")
TEST_CASE(must_respect_medium_ior__curved_and_flat_mirror_at_end__right, must_respect_medium_ior__mirrors_at_ends, "calc_beamdata_elems_and_media__2_1.rez")
TEST_CASE(must_respect_medium_ior__curved_and_flat_mirror_at_end__left, must_respect_medium_ior__mirrors_at_ends, "calc_beamdata_elems_and_media__2_2.rez")

TEST_GROUP("BeamParamsAtElemsFunction",
           ADD_TEST(must_respect_medium_ior__curved_mirrors_at_ends__simple),
           ADD_TEST(must_respect_medium_ior__curved_mirrors_at_ends__grin),
           ADD_TEST(must_respect_medium_ior__curved_mirrors_at_ends__thermo),
           ADD_TEST(must_respect_medium_ior__curved_and_flat_mirror_at_end__right),
           ADD_TEST(must_respect_medium_ior__curved_and_flat_mirror_at_end__left),
           )
}

//------------------------------------------------------------------------------

TEST_GROUP("Table function",
           ADD_GROUP(BeamParamsAtElems),
           )

} // TableFunctionTests
} // Tests
} // Z
