cmake_minimum_required(VERSION 3.16)

# Remove per-configuration suffix from output directory added by VSCode
# https://stackoverflow.com/questions/47175912/using-cmake-how-to-stop-the-debug-and-release-subdirectories
# https://cmake.org/cmake/help/v3.4/prop_tgt/RUNTIME_OUTPUT_DIRECTORY.html#prop_tgt:RUNTIME_OUTPUT_DIRECTORY
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_SOURCE_DIR}/bin>)

file(READ "release/version.txt" APP_VER_FULL)

project(rezonator VERSION ${APP_VER_FULL} LANGUAGES CXX C)

# Set policies for modern CMake behavior
# CMP0077: option() honors normal variables - allows parent projects to override options
# CMP0074: find_package() uses <PackageName>_ROOT variables for finding packages
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

string(TIMESTAMP BUILDDATE "%Y-%m-%d")
string(TIMESTAMP APP_VER_YEAR "%Y")
set(APP_VER_YEAR_0 2006)

string(REPLACE "." ";" APP_VER_LIST ${APP_VER_FULL})
list(GET APP_VER_LIST 0 APP_VER_MAJOR)
list(GET APP_VER_LIST 1 APP_VER_MINOR)
list(GET APP_VER_LIST 2 APP_VER_PATCH)

configure_file(
    release/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
    @ONLY
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable MSVC warnings about non-standard STL extensions globally
if(MSVC)
    add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
    add_compile_options(/wd4043)  # Disable STL4043 warning about stdext::checked_array_iterator
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Help Network PrintSupport Svg Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Help Network PrintSupport Svg Widgets)

# Add Python
# find_package(Python3 COMPONENTS Development REQUIRED)
# message(STATUS "Python3_FOUND: ${Python3_FOUND}")
# message(STATUS "Python3_VERSION: ${Python3_VERSION}")
# message(STATUS "Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
# message(STATUS "Python3_LIBRARIES: ${Python3_LIBRARIES}")
# message(STATUS "Python3_LIBRARY_DIRS: ${Python3_LIBRARY_DIRS}")

set(PROJECT_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
    src/app.rc
    src/app.qrc
    src/main.cpp
    src/app/Appearance.h src/app/Appearance.cpp
    src/app/AppSettings.h src/app/AppSettings.cpp
    src/app/CalcManager.h src/app/CalcManager.cpp
    src/app/CustomElemsManager.h src/app/CustomElemsManager.cpp
    src/app/HelpSystem.h src/app/HelpSystem.cpp
    src/app/MessageBus.h src/app/MessageBus.cpp
    src/app/PersistentState.h src/app/PersistentState.cpp
    src/app/ProjectOperations.h src/app/ProjectOperations.cpp
    src/core/Beam.h src/core/Beam.cpp
    src/core/CommonTypes.h src/core/CommonTypes.cpp
    src/core/Complex.h src/core/Complex.cpp
    src/core/Element.h src/core/Element.cpp
    src/core/ElementFilter.h src/core/ElementFilter.cpp
    src/core/ElementFormula.h src/core/ElementFormula.cpp
    src/core/Elements.h src/core/Elements.cpp
    src/core/ElementsCatalog.h src/core/ElementsCatalog.cpp
    src/core/Format.h src/core/Format.cpp
    src/core/Formula.h src/core/Formula.cpp
    src/core/LuaHelper.h src/core/LuaHelper.cpp
    src/core/Math.h src/core/Math.cpp
    src/core/Parameters.h src/core/Parameters.cpp
    src/core/Perf.h src/core/Perf.cpp
    src/core/Protocol.h src/core/Protocol.cpp
    src/core/Pump.h src/core/Pump.cpp
    src/core/PyModuleGlobal.h
    src/core/PyModuleSchema.h
    src/core/PyRunner.h src/core/PyRunner.cpp
    src/core/PyUtils.h
    src/core/Report.h src/core/Report.cpp
    src/core/Schema.h src/core/Schema.cpp
    src/core/Units.h src/core/Units.cpp
    src/core/Utils.h src/core/Utils.cpp
    src/core/Values.h src/core/Values.cpp
    src/core/Variable.h src/core/Variable.cpp
    src/funcs/BeamParamsAtElemsWindow.h src/funcs/BeamParamsAtElemsWindow.cpp
    src/funcs/BeamShapeExtension.h src/funcs/BeamShapeExtension.cpp
    src/funcs/BeamVariationWindow.h src/funcs/BeamVariationWindow.cpp
    src/funcs/CausticOptionsPanel.h
    src/funcs/CausticWindow.h src/funcs/CausticWindow.cpp
    src/funcs/CustomTableCodeWindow.h src/funcs/CustomTableCodeWindow.cpp
    src/funcs/CustomTableFuncWindow.h src/funcs/CustomTableFuncWindow.cpp
    src/funcs/FuncOptionsPanel.h src/funcs/FuncOptionsPanel.cpp
    src/funcs/FuncWindowHelpers.h src/funcs/FuncWindowHelpers.cpp
    src/funcs/InfoFuncWindow.h src/funcs/InfoFuncWindow.cpp
    src/funcs/MultibeamCausticWindow.h src/funcs/MultibeamCausticWindow.cpp
    src/funcs/MultiCausticParamsDlg.h src/funcs/MultiCausticParamsDlg.cpp
    src/funcs/MulticausticWindow.h src/funcs/MulticausticWindow.cpp
    src/funcs/MultirangeCausticWindow.h src/funcs/MultirangeCausticWindow.cpp
    src/funcs/PlotFuncWindow.h src/funcs/PlotFuncWindow.cpp
    src/funcs/PlotFuncWindowStorable.h src/funcs/PlotFuncWindowStorable.cpp
    src/funcs/StabilityMap2DWindow.h src/funcs/StabilityMap2DWindow.cpp
    src/funcs/StabilityMapWindow.h src/funcs/StabilityMapWindow.cpp
    src/funcs/TableFuncWindow.h src/funcs/TableFuncWindow.cpp
    src/io/Clipboard.h src/io/Clipboard.cpp
    src/io/CommonUtils.h src/io/CommonUtils.cpp
    src/io/ISchemaWindowStorable.h src/io/ISchemaWindowStorable.cpp
    src/io/JsonUtils.h src/io/JsonUtils.cpp
    src/io/SchemaReaderIni.h src/io/SchemaReaderIni.cpp
    src/io/SchemaReaderJson.h src/io/SchemaReaderJson.cpp
    src/io/SchemaWriterJson.h src/io/SchemaWriterJson.cpp
    src/math/AbcdBeamCalculator.h src/math/AbcdBeamCalculator.cpp
    src/math/BeamCalcWrapper.h src/math/BeamCalcWrapper.cpp
    src/math/BeamParamsAtElemsFunction.h src/math/BeamParamsAtElemsFunction.cpp
    src/math/BeamVariationFunction.h src/math/BeamVariationFunction.cpp
    src/math/CausticFunction.h src/math/CausticFunction.cpp
    src/math/CustomTableFunction.h src/math/CustomTableFunction.cpp
    src/math/FormatInfo.h src/math/FormatInfo.cpp
    src/math/FunctionBase.h src/math/FunctionBase.cpp
    src/math/FunctionGraph.h src/math/FunctionGraph.cpp
    src/math/FunctionUtils.h src/math/FunctionUtils.cpp
    src/math/GaussCalculator.h src/math/GaussCalculator.cpp
    src/math/GrinCalculator.h src/math/GrinCalculator.cpp
    src/math/InfoFunctions.h src/math/InfoFunctions.cpp
    src/math/LensCalculator.h
    src/math/MultibeamCausticFunction.h src/math/MultibeamCausticFunction.cpp
    src/math/MultirangeCausticFunction.h src/math/MultirangeCausticFunction.cpp
    src/math/PlotFuncRoundTripFunction.h src/math/PlotFuncRoundTripFunction.cpp
    src/math/PlotFunction.h src/math/PlotFunction.cpp
    src/math/PumpCalculator.h src/math/PumpCalculator.cpp
    src/math/RoundTripCalculator.h src/math/RoundTripCalculator.cpp
    src/math/StabilityMap2DFunction.h src/math/StabilityMap2DFunction.cpp
    src/math/StabilityMapFunction.h src/math/StabilityMapFunction.cpp
    src/math/TableFunction.h src/math/TableFunction.cpp
    src/math/tinyexpr.h src/math/tinyexpr.c
    src/tests/test_AbcdBeamCalculator.cpp
    src/tests/test_Adjuster.cpp
    src/tests/test_Element.cpp
    src/tests/test_ElementEventsLocker.cpp
    src/tests/test_ElementFilter.cpp
    src/tests/test_ElementFormula.cpp
    src/tests/test_Elements.cpp
    src/tests/test_ElementsImages.cpp
    src/tests/test_ElemSelectorWidget.cpp
    src/tests/test_Formula.cpp
    src/tests/test_GaussCalculator.cpp
    src/tests/test_GrinCalculator.cpp
    src/tests/test_InfoFunctions.cpp
    src/tests/test_LuaHelper.cpp
    src/tests/test_Math.cpp
    src/tests/test_ParamEditor.cpp
    src/tests/test_Parameters.cpp
    src/tests/test_ParamsEditor.cpp
    src/tests/test_PlotFunctions.cpp
    src/tests/test_ProjectOperations.cpp
    src/tests/test_PumpCalculator.cpp
    src/tests/test_PumpWindow.cpp
    src/tests/test_Report.cpp
    src/tests/test_RoundTripCalculator.cpp
    src/tests/test_Schema.cpp
    src/tests/test_SchemaReaderIni.cpp
    src/tests/test_SchemaReaderJson.cpp
    src/tests/test_TableFunction.cpp
    src/tests/test_TestUtils.cpp
    src/tests/test_Units.cpp
    src/tests/test_UnitWidgets.cpp
    src/tests/test_Utils.cpp
    src/tests/test_Values.cpp
    src/tests/TestSchemaListener.h
    src/tests/TestSuite.h
    src/tests/TestUtils.h
    src/tools/CalculatorWindow.h src/tools/CalculatorWindow.cpp
    src/tools/GaussCalculatorWindow.h src/tools/GaussCalculatorWindow.cpp
    src/tools/GrinLensWindow.h src/tools/GrinLensWindow.cpp
    src/tools/IrisWindow.h src/tools/IrisWindow.cpp
    src/tools/LensmakerWindow.h src/tools/LensmakerWindow.cpp
    src/widgets/ElementsTable.h src/widgets/ElementsTable.cpp
    src/widgets/ElementTypesListView.h src/widgets/ElementTypesListView.cpp
    src/widgets/ElemFormulaEditor.h src/widgets/ElemFormulaEditor.cpp
    src/widgets/ElemSelectorWidget.h src/widgets/ElemSelectorWidget.cpp
    src/widgets/FormulaEditor.h src/widgets/FormulaEditor.cpp
    src/widgets/FrozenStateButton.h src/widgets/FrozenStateButton.cpp
    src/widgets/GraphicsView.h src/widgets/GraphicsView.cpp
    src/widgets/ParamEditor.h src/widgets/ParamEditor.cpp
    src/widgets/ParamEditorEx.h src/widgets/ParamEditorEx.cpp
    src/widgets/ParamsEditor.h src/widgets/ParamsEditor.cpp
    src/widgets/ParamsListWidget.h src/widgets/ParamsListWidget.cpp
    src/widgets/ParamsTreeWidget.h src/widgets/ParamsTreeWidget.cpp
    src/widgets/PlotHelpers.h src/widgets/PlotHelpers.cpp
    src/widgets/PlotParamsPanel.h src/widgets/PlotParamsPanel.cpp
    src/widgets/PlotUtils.h src/widgets/PlotUtils.cpp
    src/widgets/PopupMessage.h src/widgets/PopupMessage.cpp
    src/widgets/RichTextItemDelegate.h src/widgets/RichTextItemDelegate.cpp
    src/widgets/SchemaElemsTable.h src/widgets/SchemaElemsTable.cpp
    src/widgets/SchemaLayout.h src/widgets/SchemaLayout.cpp
    src/widgets/SchemaLayoutAxicon.h
    src/widgets/SchemaLayoutCrystal.h
    src/widgets/SchemaLayoutDefs.h
    src/widgets/SchemaLayoutElems.h
    src/widgets/SchemaLayoutIfaces.h
    src/widgets/SchemaLayoutLens.h
    src/widgets/SchemaParamsTable.h src/widgets/SchemaParamsTable.cpp
    src/widgets/UnitWidgets.h src/widgets/UnitWidgets.cpp
    src/widgets/ValueEditor.h src/widgets/ValueEditor.cpp
    src/widgets/ValuesEditorTS.h src/widgets/ValuesEditorTS.cpp
    src/widgets/VariableRangeEditor.h src/widgets/VariableRangeEditor.cpp
    src/widgets/WidgetResult.h src/widgets/WidgetResult.cpp
    src/windows/AdjustmentWindow.h src/windows/AdjustmentWindow.cpp
    src/windows/AppSettingsDialog.h src/windows/AppSettingsDialog.cpp
    src/windows/CodeEditorWindow.h src/windows/CodeEditorWindow.cpp
    src/windows/CustomElemsWindow.h src/windows/CustomElemsWindow.cpp
    src/windows/ElementPropsDialog.h src/windows/ElementPropsDialog.cpp
    src/windows/ElementsCatalogDialog.h src/windows/ElementsCatalogDialog.cpp
    src/windows/ElemFormulaWindow.h src/windows/ElemFormulaWindow.cpp
    src/windows/FuncEditorWindow.h src/windows/FuncEditorWindow.cpp
    src/windows/HelpWindow.h src/windows/HelpWindow.cpp
    src/windows/MemoWindow.h src/windows/MemoWindow.cpp
    src/windows/ProjectWindow.h src/windows/ProjectWindow.cpp
    src/windows/ProtocolWindow.h src/windows/ProtocolWindow.cpp
    src/windows/PumpParamsDialog.h src/windows/PumpParamsDialog.cpp
    src/windows/PumpWindow.h src/windows/PumpWindow.cpp
    src/windows/RangeOperationsDialog.h src/windows/RangeOperationsDialog.cpp
    src/windows/RezonatorDialog.h src/windows/RezonatorDialog.cpp
    src/windows/SchemaParamsWindow.h src/windows/SchemaParamsWindow.cpp
    src/windows/SchemaPropsDialog.h src/windows/SchemaPropsDialog.cpp
    src/windows/SchemaViewWindow.h src/windows/SchemaViewWindow.cpp
    src/windows/SchemaWindows.h src/windows/SchemaWindows.cpp
    src/windows/StartWindow.h src/windows/StartWindow.cpp
    src/windows/WindowsManager.h src/windows/WindowsManager.cpp
)

set(ORI_WITH_SVG ON)
set(ORI_WITH_TESTS ON)
set(ORI_WITH_STYLESHEETS ON)
add_subdirectory(libs/orion)
add_subdirectory(libs/custom-plot-lab)

set(LIB_RESOURCES
    libs/orion/resources.qrc
    libs/orion/testing/test.qrc
    libs/custom-plot-lab/qcpl_images.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${LIB_RESOURCES}
    )
else()
    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${LIB_RESOURCES}
    )
endif()

# Add Lua library
find_package(Lua REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    orion
    custom-plot-lab
    lua
    Qt::Help
    Qt::Network
    Qt::PrintSupport
    Qt::Svg
    Qt::Widgets
    #Python3::Python
    ${LUA_LIBRARIES}
)
# if(APPLE)
#     # Required when linking Python on macos
#     find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
#     target_link_libraries(${PROJECT_NAME} PRIVATE ${CORE_FOUNDATION_LIBRARY})
# endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.orion-project.rezonator
    MACOSX_BUNDLE_BUNDLE_VERSION ${APP_VER_FULL}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    BUILDDATE="${BUILDDATE}"
    APP_VER_MAJOR=${APP_VER_MAJOR}
    APP_VER_MINOR=${APP_VER_MINOR}
    APP_VER_PATCH=${APP_VER_PATCH}
    APP_VER_YEAR=${APP_VER_YEAR}
    APP_VER_YEAR_0=${APP_VER_YEAR_0}
    QT_USE_QSTRINGBUILDER
    ORI_USE_STYLE_SHEETS
    #Z_USE_PYTHON
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
