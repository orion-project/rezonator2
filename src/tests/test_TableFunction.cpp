#include "../app/AppSettings.h"
#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../io/SchemaReaderJson.h"
#include "../math/BeamParamsAtElemsFunction.h"
#include "../math/FunctionUtils.h"
#include "../tests/TestUtils.h"

#include "core/OriTemplates.h"
#include "testing/OriTestBase.h"

#define READ_TEST_FILE(file_name)\
    Schema schema;\
    bool oldSkip = AppSettings::instance().skipFuncWindowsLoading;\
    AppSettings::instance().skipFuncWindowsLoading = true;\
    TEST_FILE(fullFileName, file_name)\
    SchemaReaderJson reader(&schema);\
    reader.readFromFile(fullFileName);\
    AppSettings::instance().skipFuncWindowsLoading = oldSkip;\
    LOG_SCHEMA_READER(reader)\
    ASSERT_IS_FALSE(reader.report().hasErrors())

#define CALC_FUNC(expected_result_count)\
    func.calculate();\
    ASSERT_EQ_STR(func.errorText(), "")\
    ASSERT_IS_TRUE(func.ok())\
    for (const auto& r : func.results()) {TEST_LOG(r.str())}\
    if (expected_result_count >= 0)\
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

TEST_METHOD(interfaces_sw)
{
    READ_TEST_FILE("calc_beamdata_interfaces.rez")
    schema.setTripType(TripType::SW);
    BeamParamsAtElemsFunction func(&schema);
    TableFunction::Params params;
    params.calcMediumEnds = true;
    params.calcEmptySpaces = true;
    params.calcSpaceMids = true;
    func.setParams(params);
    CALC_FUNC(ResultsCount(25))

    // normal interface
    ASSERT_TABLE_RES(3, 4, IgnoreSign(false))
    ASSERT_TABLE_RES(5, 6, IgnoreSign(false))

    // brewster interface
    ASSERT_TABLE_RES(8, 9, IgnoreSign(false))
    ASSERT_TABLE_RES(10, 11, IgnoreSign(false))

    // tilted interface
    ASSERT_TABLE_RES(13, 14, IgnoreSign(false))
    ASSERT_TABLE_RES(15, 16, IgnoreSign(false))

    // spherical interface
    ASSERT_TABLE_RES(18, 19, IgnoreSign(false))
    ASSERT_TABLE_RES(20, 21, IgnoreSign(false))
}

TEST_CASE_METHOD(interfaces_sp_rr, TripType tripType)
{
    READ_TEST_FILE("calc_beamdata_interfaces.rez")
    schema.setTripType(tripType);
    BeamParamsAtElemsFunction func(&schema);
    TableFunction::Params params;
    params.calcMediumEnds = true;
    params.calcEmptySpaces = true;
    params.calcSpaceMids = true;
    func.setParams(params);
    CALC_FUNC(ResultsCount(27))

    // normal interface
    ASSERT_TABLE_RES(4, 5, IgnoreSign(false))
    ASSERT_TABLE_RES(6, 7, IgnoreSign(false))

    // brewster interface
    ASSERT_TABLE_RES(9, 10, IgnoreSign(false))
    ASSERT_TABLE_RES(11, 12, IgnoreSign(false))

    // tilted interface
    ASSERT_TABLE_RES(14, 15, IgnoreSign(false))
    ASSERT_TABLE_RES(16, 17, IgnoreSign(false))

    // spherical interface
    ASSERT_TABLE_RES(19, 20, IgnoreSign(false))
    ASSERT_TABLE_RES(21, 22, IgnoreSign(false))
}
TEST_CASE(interfaces_sp, interfaces_sp_rr, TripType::SP)
TEST_CASE(interfaces_rr, interfaces_sp_rr, TripType::RR)

TEST_CASE_METHOD(must_respect_medium_ior__sw__mirrors_at_ends, QString fileName)
{
    READ_TEST_FILE(fileName)
    schema.setTripType(TripType::SW);
    BeamParamsAtElemsFunction func(&schema);
    TableFunction::Params params;
    params.calcMediumEnds = true;
    params.calcSpaceMids = true;
    func.setParams(params);
    CALC_FUNC(ResultsCount(5))

    // First two results must be the same - left end mirror and origin of medium
    ASSERT_TABLE_RES(0, 1, IgnoreSign(false))

    // Last two results must be the same - right end mirror and ending of medium
    // It's ok that signs are different here because beam direction changes
    ASSERT_TABLE_RES(3, 4, IgnoreSign(true))
}
TEST_CASE(must_respect_medium_ior__sw__curved_ends__simple, must_respect_medium_ior__sw__mirrors_at_ends, "calc_beamdata_elems_and_media__1_1.rez")
TEST_CASE(must_respect_medium_ior__sw__curved_ends__grin, must_respect_medium_ior__sw__mirrors_at_ends, "calc_beamdata_elems_and_media__1_2.rez")
TEST_CASE(must_respect_medium_ior__sw__curved_ends__thermo, must_respect_medium_ior__sw__mirrors_at_ends, "calc_beamdata_elems_and_media__1_3.rez")
TEST_CASE(must_respect_medium_ior__sw__curved_left_flat_right, must_respect_medium_ior__sw__mirrors_at_ends, "calc_beamdata_elems_and_media__2_1.rez")
TEST_CASE(must_respect_medium_ior__sw__curved_right_flat_left, must_respect_medium_ior__sw__mirrors_at_ends, "calc_beamdata_elems_and_media__2_2.rez")

// Why these tests? Such kind of schema is not valid for SP
// There should be interfaces at the medium ends, not mirrors
// TEST_CASE_METHOD(must_respect_medium_ior__sp__mirrors_at_ends, QString fileName)
// {
//     READ_TEST_FILE(fileName)
//     schema.setTripType(TripType::SP);
//     BeamParamsAtElemsFunction func(&schema);
//     TableFunction::Params params;
//     params.calcMediumEnds = true;
//     params.calcSpaceMids = true;
//     func.setParams(params);
//     CALC_FUNC(ResultsCount(7))

//     // First two results must be the same - left end mirror and origin of medium
//     ASSERT_TABLE_RES(1, 2, IgnoreSign(false))

//     // Last two results must be the same - right end mirror and ending of medium
//     // It's ok that signs are different here because beam direction changes
//     ASSERT_TABLE_RES(4, 5, IgnoreSign(true))
// }
// TEST_CASE(must_respect_medium_ior__sp__curved_ends__simple, must_respect_medium_ior__sp__mirrors_at_ends, "calc_beamdata_elems_and_media__1_1.rez")
// TEST_CASE(must_respect_medium_ior__sp__curved_ends__grin, must_respect_medium_ior__sp__mirrors_at_ends, "calc_beamdata_elems_and_media__1_2.rez")
// TEST_CASE(must_respect_medium_ior__sp__curved_ends__thermo, must_respect_medium_ior__sp__mirrors_at_ends, "calc_beamdata_elems_and_media__1_3.rez")
// TEST_CASE(must_respect_medium_ior__sp__curved_left_flat_right, must_respect_medium_ior__sp__mirrors_at_ends, "calc_beamdata_elems_and_media__2_1.rez")
// TEST_CASE(must_respect_medium_ior__sp__curved_right_flat_left, must_respect_medium_ior__sp__mirrors_at_ends, "calc_beamdata_elems_and_media__2_2.rez")

TEST_CASE_METHOD(must_respect_medium_ior__sw__elem_in_middle, QString fileName)
{
    READ_TEST_FILE(fileName)
    schema.setTripType(TripType::SW);
    BeamParamsAtElemsFunction func(&schema);
    TableFunction::Params params;
    params.calcMediumEnds = true;
    params.calcSpaceMids = true;
    func.setParams(params);
    CALC_FUNC(ResultsCount(14))

    // The end of left-half medium must match left side of element
    ASSERT_TABLE_RES(5, 6, IgnoreSign(false))

    // Right side of element must match the start of right-half medium
    ASSERT_TABLE_RES(7, 8, IgnoreSign(false))
}
TEST_CASE(must_respect_medium_ior__sw__flat_in_middle, must_respect_medium_ior__sw__elem_in_middle, "calc_beamdata_elems_and_media__3_1.rez")
TEST_CASE(must_respect_medium_ior__sw__lens_in_middle, must_respect_medium_ior__sw__elem_in_middle, "calc_beamdata_elems_and_media__3_2.rez")

TEST_CASE_METHOD(must_respect_medium_ior__sp_rr__elem_in_middle, QString fileName, TripType tripType)
{
    READ_TEST_FILE(fileName)
    schema.setTripType(tripType);
    BeamParamsAtElemsFunction func(&schema);
    TableFunction::Params params;
    params.calcMediumEnds = true;
    params.calcSpaceMids = true;
    func.setParams(params);
    CALC_FUNC(ResultsCount(16))

    // The end of left-half medium must match left side of element
    ASSERT_TABLE_RES(6, 7, IgnoreSign(false))

    // Right side of element must match the start of right-half medium
    ASSERT_TABLE_RES(8, 9, IgnoreSign(false))
}

TEST_CASE(must_respect_medium_ior__sp__flat_in_middle, must_respect_medium_ior__sp_rr__elem_in_middle, "calc_beamdata_elems_and_media__3_1.rez", TripType::SP)
TEST_CASE(must_respect_medium_ior__sp__lens_in_middle, must_respect_medium_ior__sp_rr__elem_in_middle, "calc_beamdata_elems_and_media__3_2.rez", TripType::SP)
TEST_CASE(must_respect_medium_ior__rr__flat_in_middle, must_respect_medium_ior__sp_rr__elem_in_middle, "calc_beamdata_elems_and_media__3_1.rez", TripType::RR)
TEST_CASE(must_respect_medium_ior__rr__lens_in_middle, must_respect_medium_ior__sp_rr__elem_in_middle, "calc_beamdata_elems_and_media__3_2.rez", TripType::RR)

// This test includes `interfaces_*` tests
TEST_CASE_METHOD(complare_between_elems, QString fileName)
{
    READ_TEST_FILE(fileName)
    BeamParamsAtElemsFunction func(&schema);
    TableFunction::Params params;
    params.calcMediumEnds = true;
    params.calcSpaceMids = false;
    params.calcEmptySpaces = true;
    func.setParams(params);
    CALC_FUNC(ResultsCount(-1))
    
    const int valCount = 3;
    using Pos = TableFunction::ResultPosition;
    
    auto areSameResults = [](const Z::PointTS &v1, const Z::PointTS &v2) {
        const double eps = 1e-7;
        bool t = false;
        bool s = false;
        if (qIsInf(v1.T) && qIsInf(v2.T))
            t = true;
        else if (qAbs(qAbs(v1.T) - qAbs(v2.T)) <= eps)
            t = true;
        if (qIsInf(v1.S) && qIsInf(v2.S))
            s = true;
        else if (qAbs(qAbs(v1.S) - qAbs(v2.S)) <= eps)
            s = true;
        return t && s;
    };

    for (int i = 0; i < schema.count(); i++)
    {
        auto elem = schema.element(i);
        
        if (elem->disabled())
        {
            continue;
        }
        if (elem->hasOption(Element_Complex))
        {
            // Skip elements like ElemGauss*
            // I'm not even sure if complex matrixes are valid for the ABCD-method.
            // There is a modified ABCDGH-method for all-complex elements,
            // but it requires a lot of work and a bit more literature to adopt it.
            continue;
        }
        
        auto res = func.results(elem);
        decltype(res)::mapped_type leftVals, rightVals;

        bool comparePrev = true;
        bool compareNext = true;

        bool compareLeftRight = false;
        bool compareSizeT = true;
        bool compareSizeS = true;
        bool compareFrontT = !elem->hasOption(Element_ChangesWavefront);
        bool compareFrontS = !elem->hasOption(Element_ChangesWavefront);
        bool compareAngleT = !elem->hasOption(Element_ChangesWavefront);
        bool compareAngleS = !elem->hasOption(Element_ChangesWavefront);
        
        if (auto range = elem->asRange(); range)
        {
            if (Z::Utils::isSpace(elem))
            {
                // Spaces are not calculated "outside"
                leftVals = res.at(Pos::LEFT_INSIDE);
                rightVals = res.at(Pos::RIGHT_INSIDE);
                // Empty space doesn't change angle in the far-field
                compareAngleT = true;
                compareAngleS = true;
            }
            else if (auto medium = Z::Utils::asMedium(elem))
            {
                // Mediums are not calculated "outside"
                leftVals = res.at(Pos::LEFT_INSIDE);
                rightVals = res.at(Pos::RIGHT_INSIDE);
                // Medium without IOR doesn't change angle in the far-field
                if (medium->ior() == 1)
                {
                    compareAngleT = true;
                    compareAngleS = true;
                }
            }
            else
            {
                leftVals = res.at(Pos::LEFT_OUTSIDE);
                rightVals = res.at(Pos::RIGHT_OUTSIDE);
                // Plate without IOR doesn't change angle in the far-field
                if (range->ior() == 1)
                {
                    compareAngleT = true;
                    compareAngleS = true;
                }
            }
        }
        else
        {
            // These are flat mirrors, plane interfaces, planes etc.
            // Beam parameter before and after such element must be the same
            // Parameters after the prev elem must be the same as ones before this elem
            // Parameters before the next elem must be the same as ones after this elem

            if (elem->isInterface())
            {
                // Interfaces always display "left" and "right"
                compareLeftRight = true;
                // Some interfaces change beamsize in T plane
                if (dynamic_cast<ElemBrewsterInterface*>(elem))
                    compareSizeT = false;
                else if (auto intf = dynamic_cast<ElemTiltedInterface*>(elem); intf)
                {
                    // Tilted interface without angle works as plane
                    if (!Double(intf->alpha()).isZero())
                        compareSizeT = false;
                }
                else if (auto intf = dynamic_cast<ElemSphericalInterface*>(elem); intf)
                {
                    // Curved interface without radius works as plane
                    if (Double(intf->radius()).isInfinity())
                    {
                        compareFrontT = true;
                        compareFrontS = true;
                        compareAngleT = true;
                        compareAngleS = true;
                    }
                }
                leftVals = res.at(Pos::IFACE_LEFT);
                rightVals = res.at(Pos::IFACE_RIGHT);
                if (i == 0)
                    comparePrev = false;
                else if (i == schema.count()-1) 
                    compareNext = false;
            }
            else if (i == 0)
            {
                // First "thin" element does not display "left" result
                comparePrev = false;
                if (schema.tripType() == TripType::SW)
                    rightVals = res.at(Pos::ELEMENT);
                else rightVals = res.at(Pos::RIGHT);
            }
            else if (i == schema.count()-1)
            {
                // Last "thin" element does not display "right" result
                compareNext = false;
                if (schema.tripType() == TripType::SW)
                    leftVals = res.at(Pos::ELEMENT);
                else leftVals = res.at(Pos::LEFT);
            }
            else
            {
                compareLeftRight = true;
                leftVals = res.at(Pos::LEFT);
                rightVals = res.at(Pos::RIGHT);
            }
            // Some element change wavefront only in one plane
            // so value in another plane should be unchanged
            if (dynamic_cast<ElemCylinderLensS*>(elem))
            {
                compareFrontT = true;
                compareAngleT = true;
            }
            if (dynamic_cast<ElemCylinderLensT*>(elem))
            {
                compareFrontS = true;
                compareAngleS = true;
            }
        }

        TEST_LOG(QString("*** Check element %1").arg(elem->label()))
        
        if (compareLeftRight)
        {
            const auto &sizeLeft = leftVals.at(0);
            const auto &frontLeft = leftVals.at(1);
            const auto &angleLeft = leftVals.at(2);
            const auto &sizeRight = rightVals.at(0);
            const auto &frontRight = rightVals.at(1);
            const auto &angleRight = rightVals.at(2);
            if (compareSizeT) {
                TEST_LOG(QString("Wt: %1 == %2").arg(sizeLeft.T).arg(sizeRight.T))
                ASSERT_NEAR_DBL(sizeLeft.T, sizeRight.T, 1e-16)
            }
            if (compareSizeS) {
                TEST_LOG(QString("Ws: %1 == %2").arg(sizeLeft.S).arg(sizeRight.S))
                ASSERT_NEAR_DBL(sizeLeft.S, sizeRight.S, 1e-16)
            }
            if (compareFrontT) {
                TEST_LOG(QString("Rt: %1 == %2").arg(frontLeft.T).arg(frontRight.T))
                ASSERT_NEAR_DBL(frontLeft.T, frontRight.T, 1e-16)
            }
            if (compareFrontS) {
                TEST_LOG(QString("Rs: %1 == %2").arg(frontLeft.S).arg(frontRight.S))
                ASSERT_NEAR_DBL(frontLeft.S, frontRight.S, 1e-16)
            }
            if (compareAngleT) {
                TEST_LOG(QString("Vt: %1 == %2").arg(angleLeft.T).arg(angleRight.T))
                ASSERT_NEAR_DBL(angleLeft.T, angleRight.T, 1e-16)
            }
            if (compareAngleS) {
                TEST_LOG(QString("Vs: %1 == %2").arg(angleLeft.S).arg(angleRight.S))
                ASSERT_NEAR_DBL(angleLeft.S, angleRight.S, 1e-16)
            }
        }
        if (comparePrev)
        {
            ASSERT_EQ_INT(leftVals.size(), valCount);

            auto prevElem = FunctionUtils::prevElem(&schema, elem);
            ASSERT_IS_NOT_NULL(prevElem)
            TEST_LOG(QString("Prev elem: %1").arg(prevElem->label()));
            auto prevRes = func.results(prevElem);
            ASSERT_IS_FALSE(prevRes.empty());
            auto prevPos = Pos::RIGHT;
            // Spaces and mediums are not calculated "outside"
            if (Z::Utils::isSpace(prevElem) || Z::Utils::isMedium(prevElem))
                prevPos = Pos::RIGHT_INSIDE;
            else if (prevElem->isRange())
                prevPos = Pos::RIGHT_OUTSIDE;
            else if (prevElem->isInterface())
                prevPos = Pos::IFACE_RIGHT;
            else if (schema.indexOf(prevElem) == 0 && schema.tripType() == TripType::SW)
                prevPos = Pos::ELEMENT;
            auto prevVals = prevRes.at(prevPos);
            ASSERT_EQ_INT(prevVals.size(), valCount);
            ASSERT_EQ_LIST_EX(leftVals, prevVals, areSameResults)
        }
        if (compareNext)
        {
            ASSERT_EQ_INT(rightVals.size(), valCount);

            auto nextElem = FunctionUtils::nextElem(&schema, elem);
            TEST_LOG(QString("Next elem: %1").arg(nextElem->label()));
            ASSERT_IS_NOT_NULL(nextElem)
            auto nextRes = func.results(nextElem);
            ASSERT_IS_FALSE(nextRes.empty());
            auto nextPos = Pos::LEFT;
            // Spaces and mediums are not calculated "outside"
            if (Z::Utils::isSpace(nextElem) || Z::Utils::isMedium(nextElem))
                nextPos = Pos::LEFT_INSIDE;
            else if (nextElem->isRange())
                nextPos = Pos::LEFT_OUTSIDE;
            else if (nextElem->isInterface())
                nextPos = Pos::IFACE_LEFT;
            else if (schema.indexOf(nextElem) == schema.count()-1 && schema.tripType() == TripType::SW)
                nextPos = Pos::ELEMENT;
            auto nextVals = nextRes.at(nextPos);
            ASSERT_EQ_INT(nextVals.size(), valCount);
            ASSERT_EQ_LIST_EX(rightVals, nextVals, areSameResults)
        }
    }
}

TEST_CASE(complare_between_elems__sw, complare_between_elems, "calc_beamdata_elems_sw.rez")
TEST_CASE(complare_between_elems__sp, complare_between_elems, "calc_beamdata_elems_sp.rez")
TEST_CASE(complare_between_elems__1_1, complare_between_elems, "calc_beamdata_elems_and_media__1_1.rez")
TEST_CASE(complare_between_elems__1_2, complare_between_elems, "calc_beamdata_elems_and_media__1_2.rez")
TEST_CASE(complare_between_elems__1_3, complare_between_elems, "calc_beamdata_elems_and_media__1_3.rez")
TEST_CASE(complare_between_elems__2_1, complare_between_elems, "calc_beamdata_elems_and_media__2_1.rez")
TEST_CASE(complare_between_elems__2_2, complare_between_elems, "calc_beamdata_elems_and_media__2_2.rez")
TEST_CASE(complare_between_elems__3_1, complare_between_elems, "calc_beamdata_elems_and_media__3_1.rez")
TEST_CASE(complare_between_elems__3_2, complare_between_elems, "calc_beamdata_elems_and_media__3_2.rez")

TEST_GROUP("BeamParamsAtElemsFunction",
           ADD_TEST(interfaces_sw),
           ADD_TEST(interfaces_sp),
           ADD_TEST(interfaces_rr),
           ADD_TEST(must_respect_medium_ior__sw__curved_ends__simple),
           ADD_TEST(must_respect_medium_ior__sw__curved_ends__grin),
           ADD_TEST(must_respect_medium_ior__sw__curved_ends__thermo),
           ADD_TEST(must_respect_medium_ior__sw__curved_left_flat_right),
           ADD_TEST(must_respect_medium_ior__sw__curved_right_flat_left),
           // ADD_TEST(must_respect_medium_ior__sp__curved_ends__simple),
           // ADD_TEST(must_respect_medium_ior__sp__curved_ends__grin),
           // ADD_TEST(must_respect_medium_ior__sp__curved_ends__thermo),
           // ADD_TEST(must_respect_medium_ior__sp__curved_left_flat_right),
           // ADD_TEST(must_respect_medium_ior__sp__curved_right_flat_left),
           ADD_TEST(must_respect_medium_ior__sw__flat_in_middle),
           ADD_TEST(must_respect_medium_ior__sw__lens_in_middle),
           ADD_TEST(must_respect_medium_ior__sp__flat_in_middle),
           ADD_TEST(must_respect_medium_ior__sp__lens_in_middle),
           ADD_TEST(must_respect_medium_ior__rr__flat_in_middle),
           ADD_TEST(must_respect_medium_ior__rr__lens_in_middle),
           ADD_TEST(complare_between_elems__sw),
           ADD_TEST(complare_between_elems__sp),
           ADD_TEST(complare_between_elems__1_1),
           ADD_TEST(complare_between_elems__1_2),
           ADD_TEST(complare_between_elems__1_3),
           ADD_TEST(complare_between_elems__2_1),
           ADD_TEST(complare_between_elems__2_2),
           ADD_TEST(complare_between_elems__3_1),
           ADD_TEST(complare_between_elems__3_2),
           )
}

//------------------------------------------------------------------------------

TEST_GROUP("Table function",
           ADD_GROUP(BeamParamsAtElems),
           )

} // TableFunctionTests
} // Tests
} // Z
