#include "testing/OriTestBase.h"
#include "tests/orion_tests.h"

namespace Z {
namespace Tests {

USE_GROUP(ReportTests)                             // test_Report.cpp
USE_GROUP(UnitsTests)                              // test_Units.cpp
USE_GROUP(UnitWidgetsTests)                        // test_UnitWidgets.cpp
USE_GROUP(MathTests)                               // test_Math.cpp
USE_GROUP(ValuesTests)                             // test_Values.cpp
USE_GROUP(ParametersTests)                         // test_Parameters.cpp
USE_GROUP(ElementTests)                            // test_Element.cpp
USE_GROUP(ElementsTests)                           // test_Elements.cpp
USE_GROUP(ElementFormulaTests)                     // test_ElementFormula.cpp
USE_GROUP(ElementFilterTests)                      // test_ElementFilter.cpp
USE_GROUP(ElementsImagesTests)                     // test_ElementsImages.cpp
USE_GROUP(SchemaTests)                             // test_Schema.cpp
USE_GROUP(SchemaReaderIniTests)                    // test_SchemaReaderIni.cpp
USE_GROUP(SchemaReaderJsonTests)                   // test_SchemaReaderJson.cpp
USE_GROUP(RoundTripCalculatorTests)                // test_RoundTripCalculator.cpp
USE_GROUP(GaussCalculatorTests)                    // test_GaussCalculator.cpp
USE_GROUP(PumpCalculatorTests)                     // test_PumpCalculator.cpp
USE_GROUP(InfoFunctionsTests)                      // test_InfoFunctions.cpp
USE_GROUP(ElementSelectorWidgetTests)              // test_ElemSelectorWidget.cpp
USE_GROUP(PumpWindowTests)                         // test_PumpWindow.cpp
USE_GROUP(LuaHelperTests)                          // test_LuaHelper.cpp
USE_GROUP(ProjectOperationsTests)                  // test_ProjectOperations.cpp

TEST_SUITE(
    ADD_GROUP(Ori::Tests::All),
    ADD_GROUP(ReportTests),
    ADD_GROUP(UnitsTests),
    ADD_GROUP(UnitWidgetsTests),
    ADD_GROUP(MathTests),
    ADD_GROUP(ValuesTests),
    ADD_GROUP(ParametersTests),
    ADD_GROUP(ElementTests),
    ADD_GROUP(ElementsTests),
    ADD_GROUP(ElementFormulaTests),
    ADD_GROUP(ElementFilterTests),
    ADD_GROUP(ElementsImagesTests),
    ADD_GROUP(SchemaTests),
    ADD_GROUP(SchemaReaderIniTests),
    ADD_GROUP(SchemaReaderJsonTests),
    ADD_GROUP(RoundTripCalculatorTests),
    ADD_GROUP(GaussCalculatorTests),
    ADD_GROUP(PumpCalculatorTests),
    ADD_GROUP(InfoFunctionsTests),
    ADD_GROUP(ElementSelectorWidgetTests),
    ADD_GROUP(PumpWindowTests),
    ADD_GROUP(LuaHelperTests),
    ADD_GROUP(ProjectOperationsTests),
)

} // namespace Tests
} // namespace Z

