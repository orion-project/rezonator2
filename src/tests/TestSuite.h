#include "testing/OriTestBase.h"
#include "tests/orion_tests.h"

namespace Z {
namespace Tests {

USE_GROUP(ReportTests)                             // test_Report.cpp
USE_GROUP(UnitsTests)                              // test_Units.cpp
USE_GROUP(MathTests)                               // test_Math.cpp
USE_GROUP(ValuesTests)                             // test_Values.cpp
USE_GROUP(ParametersTests)                         // test_Parameters.cpp
USE_GROUP(ElementTests)                            // test_Element.cpp
USE_GROUP(ElementsTests)                           // test_Elements.cpp
USE_GROUP(ElementFilterTests)                      // test_ElementFilter.cpp
USE_GROUP(ElementsImagesTests)                     // test_ElementsImages.cpp
USE_GROUP(SchemaTests)                             // test_Schema.cpp
USE_GROUP(SchemaReaderIniTests)                    // test_SchemaReaderIni.cpp
USE_GROUP(RoundTripCalculatorTests)                // test_RoundTripCalculator.cpp
USE_GROUP(GaussCalculatorTests)                    // test_GaussCalculatorTests.cpp
USE_GROUP(InfoFunctionsTests)                      // test_InfoFunctions.cpp
USE_GROUP(ElementSelectorWidgetTests)              // test_ElemSelectorWidget.cpp

TEST_SUITE(
    ADD_GROUP(Ori::Tests::All),
    ADD_GROUP(ReportTests),
    ADD_GROUP(UnitsTests),
    ADD_GROUP(MathTests),
    ADD_GROUP(ValuesTests),
    ADD_GROUP(ParametersTests),
    ADD_GROUP(ElementTests),
    ADD_GROUP(ElementsTests),
    ADD_GROUP(ElementFilterTests),
    ADD_GROUP(ElementsImagesTests),
    ADD_GROUP(SchemaTests),
    ADD_GROUP(SchemaReaderIniTests),
    ADD_GROUP(RoundTripCalculatorTests),
    ADD_GROUP(GaussCalculatorTests),
    ADD_GROUP(InfoFunctionsTests),
    ADD_GROUP(ElementSelectorWidgetTests),
)

} // namespace Tests
} // namespace Z

