QT += core gui widgets printsupport network help

win32-msvc* {
    # MSVC is more strict about c++ features
    # GCC understands some c++20 features (e.g. aggregate initialization)
    # even in c++17 mode, while MSVC doesn't
    CONFIG += c++20
    # CONFIG seem's not fully working, need to pass the flag explicitly
    QMAKE_CXXFLAGS += /std:c++20
} else {
    # For GCC try to support older versions, like 8.4
    # So stick to the earlier standard
    CONFIG += c++17
    
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas
}

#------------------------------------------------------------
# Definition of output

TARGET = rezonator
TEMPLATE = app
DESTDIR = $$_PRO_FILE_PWD_/bin
QMAKE_TARGET_BUNDLE_PREFIX = org.orion-project

unix: LIBS += -ldl

#------------------------------------------------------------
# Submodules

#--------
# orion (https://github.com/orion-project/orion-qt)
ORION = $$_PRO_FILE_PWD_/libs/orion
DEFINES += ORI_USE_STYLE_SHEETS
include($$ORION/orion.pri)
include($$ORION/orion_testing.pri)
include($$ORION/orion_tests.pri)
include($$ORION/orion_svg.pri)

#--------
# custom-plot-lab (https://github.com/orion-project/custom-plot-lab)

# qcustomplot.cpp is so large that it even fails to build in debug mode
# If debug mode is required, you have to use QCustomPlot as shared library:
# cd libs\custom-plot-lab\qcustomplot; qmake; mingw32-make release
# Then enable "qcustomplotlab_shared" options and rebuild rezonator
#win32: CONFIG += qcustomplotlab_shared

include($$_PRO_FILE_PWD_/libs/custom-plot-lab/custom-plot-lab.pri)

#--------
# lua
include($$_PRO_FILE_PWD_/libs/lua.pri)

#------------------------------------------------------------
# Version information

include(release/version.pri)

#------------------------------------------------------------

win32: RC_FILE = src/app.rc
macx: ICON = img/icon/main_2.icns

#------------------------------------------------------------
# Translation

#TRANSLATIONS = $$DESTDIR/languages/rezonator_ru.ts

#------------------------------------------------------------
# Sources

RESOURCES += \
    src/app.qrc

HEADERS += \
    src/app/Appearance.h \
    src/app/AppSettings.h \
    src/app/CalcManager.h \
    src/app/CustomElemsManager.h \
    src/app/HelpSystem.h \
    src/app/MessageBus.h \
    src/app/PersistentState.h \
    src/app/ProjectOperations.h \
    src/core/Beam.h \
    src/core/CommonTypes.h \
    src/core/Complex.h \
    src/core/Element.h \
    src/core/ElementFilter.h \
    src/core/ElementFormula.h \
    src/core/Elements.h \
    src/core/ElementsCatalog.h \
    src/core/Format.h \
    src/core/Formula.h \
    src/core/LuaHelper.h \
    src/core/Math.h \
    src/core/Parameters.h \
    src/core/Perf.h \
    src/core/Protocol.h \
    src/core/Pump.h \
    src/core/Report.h \
    src/core/Schema.h \
    src/core/Units.h \
    src/core/Utils.h \
    src/core/Values.h \
    src/core/Variable.h \
    src/funcs/BeamParamsAtElemsWindow.h \
    src/funcs/BeamShapeExtension.h \
    src/funcs/BeamVariationWindow.h \
    src/funcs/CausticOptionsPanel.h \
    src/funcs/CausticWindow.h \
    src/funcs/FuncOptionsPanel.h \
    src/funcs/FuncWindowHelpers.h \
    src/funcs/InfoFuncWindow.h \
    src/funcs/MultibeamCausticWindow.h \
    src/funcs/MultiCausticParamsDlg.h \
    src/funcs/MulticausticWindow.h \
    src/funcs/MultirangeCausticWindow.h \
    src/funcs/PlotFuncWindow.h \
    src/funcs/PlotFuncWindowStorable.h \
    src/funcs/StabilityMap2DWindow.h \
    src/funcs/StabilityMapWindow.h \
    src/funcs/TableFuncWindow.h \
    src/math/AbcdBeamCalculator.h \
    src/math/BeamParamsAtElemsFunction.h \
    src/math/BeamVariationFunction.h \
    src/math/CausticFunction.h \
    src/math/FormatInfo.h \
    src/math/FunctionBase.h \
    src/math/FunctionGraph.h \
    src/math/FunctionUtils.h \
    src/math/GaussCalculator.h \
    src/math/GrinCalculator.h \
    src/math/InfoFunctions.h \
    src/math/LensCalculator.h \
    src/math/MultibeamCausticFunction.h \
    src/math/MultirangeCausticFunction.h \
    src/math/PlotFuncRoundTripFunction.h \
    src/math/PlotFunction.h \
    src/math/PumpCalculator.h \
    src/math/RoundTripCalculator.h \
    src/math/StabilityMap2DFunction.h \
    src/math/StabilityMapFunction.h \
    src/math/TableFunction.h \
    src/io/Clipboard.h \
    src/io/CommonUtils.h \
    src/io/ISchemaWindowStorable.h \
    src/io/JsonUtils.h \
    src/io/SchemaReaderIni.h \
    src/io/SchemaReaderJson.h \
    src/io/SchemaWriterJson.h \
    src/tests/TestSchemaListener.h \
    src/tests/TestSuite.h \
    src/tests/TestUtils.h \
    src/tools/CalculatorWindow.h \
    src/tools/GaussCalculatorWindow.h \
    src/tools/GrinLensWindow.h \
    src/tools/IrisWindow.h \
    src/tools/LensmakerWindow.h \
    src/widgets/ElementsTable.h \
    src/widgets/ElementTypesListView.h \
    src/widgets/ElemFormulaEditor.h \
    src/widgets/ElemSelectorWidget.h \
    src/widgets/FormulaEditor.h \
    src/widgets/FrozenStateButton.h \
    src/widgets/GraphicsView.h \
    src/widgets/ParamEditor.h \
    src/widgets/ParamEditorEx.h \
    src/widgets/ParamsEditor.h \
    src/widgets/ParamsListWidget.h \
    src/widgets/ParamsTreeWidget.h \
    src/widgets/PlotHelpers.h \
    src/widgets/PlotParamsPanel.h \
    src/widgets/PlotUtils.h \
    src/widgets/PopupMessage.h \
    src/widgets/RichTextItemDelegate.h \
    src/widgets/SchemaElemsTable.h \
    src/widgets/SchemaLayout.h \
    src/widgets/SchemaLayoutAxicon.h \
    src/widgets/SchemaLayoutCrystal.h \
    src/widgets/SchemaLayoutDefs.h \
    src/widgets/SchemaLayoutElems.h \
    src/widgets/SchemaLayoutIfaces.h \
    src/widgets/SchemaLayoutLens.h \
    src/widgets/SchemaParamsTable.h \
    src/widgets/UnitWidgets.h \
    src/widgets/ValueEditor.h \
    src/widgets/ValuesEditorTS.h \
    src/widgets/VariableRangeEditor.h \
    src/widgets/WidgetResult.h \
    src/windows/AdjustmentWindow.h \
    src/windows/AppSettingsDialog.h \
    src/windows/CustomElemsWindow.h \
    src/windows/ElementPropsDialog.h \
    src/windows/ElementsCatalogDialog.h \
    src/windows/ElemFormulaWindow.h \
    src/windows/HelpWindow.h \
    src/windows/MemoWindow.h \
    src/windows/ProjectWindow.h \
    src/windows/ProtocolWindow.h \
    src/windows/PumpParamsDialog.h \
    src/windows/PumpWindow.h \
    src/windows/RezonatorDialog.h \
    src/windows/SchemaParamsWindow.h \
    src/windows/SchemaPropsDialog.h \
    src/windows/SchemaViewWindow.h \
    src/windows/SchemaWindows.h \
    src/windows/StartWindow.h \
    src/windows/WindowsManager.h
	
SOURCES += \
    src/app/Appearance.cpp \
    src/app/AppSettings.cpp \
    src/app/CalcManager.cpp \
    src/app/CustomElemsManager.cpp \
    src/app/HelpSystem.cpp \
    src/app/MessageBus.cpp \
    src/app/PersistentState.cpp \
    src/app/ProjectOperations.cpp \
    src/core/Beam.cpp \
    src/core/CommonTypes.cpp \
    src/core/Complex.cpp \
    src/core/Element.cpp \
    src/core/ElementFilter.cpp \
    src/core/ElementFormula.cpp \
    src/core/Elements.cpp \
    src/core/ElementsCatalog.cpp \
    src/core/Format.cpp \
    src/core/Formula.cpp \
    src/core/LuaHelper.cpp \
    src/core/Math.cpp \
    src/core/Parameters.cpp \
    src/core/Perf.cpp \
    src/core/Protocol.cpp \
    src/core/Pump.cpp \
    src/core/Report.cpp \
    src/core/Schema.cpp \
    src/core/Units.cpp \
    src/core/Utils.cpp \
    src/core/Values.cpp \
    src/core/Variable.cpp \
    src/funcs/BeamParamsAtElemsWindow.cpp \
    src/funcs/BeamShapeExtension.cpp \
    src/funcs/BeamVariationWindow.cpp \
    src/funcs/CausticWindow.cpp \
    src/funcs/FuncOptionsPanel.cpp \
    src/funcs/FuncWindowHelpers.cpp \
    src/funcs/InfoFuncWindow.cpp \
    src/funcs/MultibeamCausticWindow.cpp \
    src/funcs/MultiCausticParamsDlg.cpp \
    src/funcs/MulticausticWindow.cpp \
    src/funcs/MultirangeCausticWindow.cpp \
    src/funcs/PlotFuncWindow.cpp \
    src/funcs/PlotFuncWindowStorable.cpp \
    src/funcs/StabilityMap2DWindow.cpp \
    src/funcs/StabilityMapWindow.cpp \
    src/funcs/TableFuncWindow.cpp \
    src/math/AbcdBeamCalculator.cpp \
    src/math/BeamParamsAtElemsFunction.cpp \
    src/math/BeamVariationFunction.cpp \
    src/math/CausticFunction.cpp \
    src/math/FormatInfo.cpp \
    src/math/FunctionBase.cpp \
    src/math/FunctionGraph.cpp \
    src/math/FunctionUtils.cpp \
    src/math/GaussCalculator.cpp \
    src/math/GrinCalculator.cpp \
    src/math/InfoFunctions.cpp \
    src/math/MultibeamCausticFunction.cpp \
    src/math/MultirangeCausticFunction.cpp \
    src/math/PlotFuncRoundTripFunction.cpp \
    src/math/PlotFunction.cpp \
    src/math/PumpCalculator.cpp \
    src/math/RoundTripCalculator.cpp \
    src/math/StabilityMap2DFunction.cpp \
    src/math/StabilityMapFunction.cpp \
    src/math/TableFunction.cpp \
    src/io/Clipboard.cpp \
    src/io/CommonUtils.cpp \
    src/io/ISchemaWindowStorable.cpp \
    src/io/JsonUtils.cpp \
    src/io/SchemaReaderIni.cpp \
    src/io/SchemaReaderJson.cpp \
    src/io/SchemaWriterJson.cpp \
    src/main.cpp \
    src/tests/test_AbcdBeamCalculator.cpp \
    src/tests/test_Adjuster.cpp \
    src/tests/test_Element.cpp \
    src/tests/test_ElementEventsLocker.cpp \
    src/tests/test_ElementFilter.cpp \
    src/tests/test_ElementFormula.cpp \
    src/tests/test_Elements.cpp \
    src/tests/test_ElementsImages.cpp \
    src/tests/test_ElemSelectorWidget.cpp \
    src/tests/test_Formula.cpp \
    src/tests/test_GaussCalculator.cpp \
    src/tests/test_GrinCalculator.cpp \
    src/tests/test_InfoFunctions.cpp \
    src/tests/test_LuaHelper.cpp \
    src/tests/test_Math.cpp \
    src/tests/test_ParamEditor.cpp \
    src/tests/test_Parameters.cpp \
    src/tests/test_ParamsEditor.cpp \
    src/tests/test_PlotFunctions.cpp \
    src/tests/test_ProjectOperations.cpp \
    src/tests/test_PumpCalculator.cpp \
    src/tests/test_PumpWindow.cpp \
    src/tests/test_Report.cpp \
    src/tests/test_RoundTripCalculator.cpp \
    src/tests/test_Schema.cpp \
    src/tests/test_SchemaReaderIni.cpp \
    src/tests/test_SchemaReaderJson.cpp \
    src/tests/test_TableFunction.cpp \
    src/tests/test_TestUtils.cpp \
    src/tests/test_Units.cpp \
    src/tests/test_UnitWidgets.cpp \
    src/tests/test_Utils.cpp \
    src/tests/test_Values.cpp \
    src/tools/CalculatorWindow.cpp \
    src/tools/GaussCalculatorWindow.cpp \
    src/tools/GrinLensWindow.cpp \
    src/tools/IrisWindow.cpp \
    src/tools/LensmakerWindow.cpp \
    src/widgets/ElementsTable.cpp \
    src/widgets/ElementTypesListView.cpp \
    src/widgets/ElemFormulaEditor.cpp \
    src/widgets/ElemSelectorWidget.cpp \
    src/widgets/FormulaEditor.cpp \
    src/widgets/FrozenStateButton.cpp \
    src/widgets/GraphicsView.cpp \
    src/widgets/ParamEditor.cpp \
    src/widgets/ParamEditorEx.cpp \
    src/widgets/ParamsEditor.cpp \
    src/widgets/ParamsListWidget.cpp \
    src/widgets/ParamsTreeWidget.cpp \
    src/widgets/PlotHelpers.cpp \
    src/widgets/PlotParamsPanel.cpp \
    src/widgets/PlotUtils.cpp \
    src/widgets/PopupMessage.cpp \
    src/widgets/RichTextItemDelegate.cpp \
    src/widgets/SchemaElemsTable.cpp \
    src/widgets/SchemaLayout.cpp \
    src/widgets/SchemaParamsTable.cpp \
    src/widgets/UnitWidgets.cpp \
    src/widgets/ValueEditor.cpp \
    src/widgets/ValuesEditorTS.cpp \
    src/widgets/VariableRangeEditor.cpp \
    src/widgets/WidgetResult.cpp \
    src/windows/AdjustmentWindow.cpp \
    src/windows/AppSettingsDialog.cpp \
    src/windows/CustomElemsWindow.cpp \
    src/windows/ElementPropsDialog.cpp \
    src/windows/ElementsCatalogDialog.cpp \
    src/windows/ElemFormulaWindow.cpp \
    src/windows/HelpWindow.cpp \
    src/windows/MemoWindow.cpp \
    src/windows/ProjectWindow.cpp \
    src/windows/ProtocolWindow.cpp \
    src/windows/PumpParamsDialog.cpp \
    src/windows/PumpWindow.cpp \
    src/windows/RezonatorDialog.cpp \
    src/windows/SchemaParamsWindow.cpp \
    src/windows/SchemaPropsDialog.cpp \
    src/windows/SchemaViewWindow.cpp \
    src/windows/SchemaWindows.cpp \
    src/windows/StartWindow.cpp \
    src/windows/WindowsManager.cpp

DISTFILES += \
    release/history.json \
    tips/tips.json
