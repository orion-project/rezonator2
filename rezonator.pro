QT += core gui widgets printsupport network

CONFIG += c++17

#------------------------------------------------------------
# Definition of output

TARGET = rezonator
TEMPLATE = app
DESTDIR = $$_PRO_FILE_PWD_/bin

#------------------------------------------------------------
# Submodules

# orion (https://github.com/orion-project/orion-qt)
ORION = $$_PRO_FILE_PWD_/libs/orion
include($$ORION/orion.pri)
include($$ORION/orion_testing.pri)
include($$ORION/orion_tests.pri)
include($$ORION/orion_svg.pri)

# custom-plot-lab (https://github.com/orion-project/custom-plot-lab)
include($$_PRO_FILE_PWD_/libs/custom-plot-lab/custom-plot-lab.pri)

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
    src/images.qrc

HEADERS += \
    src/AdjustmentWindow.h \
    src/AppSettings.h \
    src/CalcManager.h \
    src/CalculatorWindow.h \
    src/CustomElemsManager.h \
    src/CustomElemsWindow.h \
    src/CustomPrefs.h \
    src/ElemFormulaWindow.h \
    src/ElementPropsDialog.h \
    src/ElementsCatalogDialog.h \
    src/GrinLensWindow.h \
    src/HelpSystem.h \
    src/MemoWindow.h \
    src/MessageBus.h \
    src/ProjectOperations.h \
    src/ProjectWindow.h \
    src/ProtocolWindow.h \
    src/PumpParamsDialog.h \
    src/PumpWindow.h \
    src/RezonatorDialog.h \
    src/SchemaParamsWindow.h \
    src/SchemaViewWindow.h \
    src/SchemaWindows.h \
    src/WindowsManager.h \
    src/core/Beam.h \
    src/core/CommonTypes.h \
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
    src/core/Protocol.h \
    src/core/Pump.h \
    src/core/Report.h \
    src/core/Schema.h \
    src/core/Units.h \
    src/core/Values.h \
    src/core/Variable.h \
    src/funcs/BeamParamsAtElemsFunction.h \
    src/funcs/CausticFunction.h \
    src/funcs/FormatInfo.h \
    src/funcs/FunctionBase.h \
    src/funcs/FunctionGraph.h \
    src/funcs/FunctionUtils.h \
    src/funcs/GaussCalculator.h \
    src/funcs/GrinCalculator.h \
    src/funcs/InfoFunctions.h \
    src/funcs/MultibeamCausticFunction.h \
    src/funcs/MultirangeCausticFunction.h \
    src/funcs/PlotFuncRoundTripFunction.h \
    src/funcs/PlotFunction.h \
    src/funcs/PumpCalculator.h \
    src/funcs/RoundTripCalculator.h \
    src/funcs/StabilityMap2DFunction.h \
    src/funcs/StabilityMapFunction.h \
    src/funcs/TableFunction.h \
    src/funcs_window/BeamShapeExtension.h \
    src/funcs_window/CausticWindow.h \
    src/funcs_window/FuncWindowHelpers.h \
    src/funcs_window/InfoFuncWindow.h \
    src/funcs_window/MultiCausticParamsDlg.h \
    src/funcs_window/MultibeamCausticWindow.h \
    src/funcs_window/MulticausticWindow.h \
    src/funcs_window/MultirangeCausticWindow.h \
    src/funcs_window/PlotFuncWindow.h \
    src/funcs_window/PlotFuncWindowStorable.h \
    src/funcs_window/StabilityMap2DWindow.h \
    src/funcs_window/StabilityMapWindow.h \
    src/funcs_window/TableFuncWindow.h \
    src/io/ISchemaWindowStorable.h \
    src/io/SchemaReaderIni.h \
    src/io/SchemaReaderJson.h \
    src/io/SchemaWriterJson.h \
    src/tests/TestSuite.h \
    src/tests/TestUtils.h \
    src/Appearance.h \
    src/widgets/ElemFormulaEditor.h \
    src/widgets/ElemSelectorWidget.h \
    src/widgets/ElementImagesProvider.h \
    src/widgets/ElementTypesListView.h \
    src/widgets/ElementsTable.h \
    src/widgets/FormulaEditor.h \
    src/widgets/FrozenStateButton.h \
    src/widgets/ParamEditor.h \
    src/widgets/ParamEditorEx.h \
    src/widgets/ParamsEditor.h \
    src/widgets/ParamsListWidget.h \
    src/widgets/PlotHelpers.h \
    src/widgets/PlotParamsPanel.h \
    src/widgets/PopupMessage.h \
    src/widgets/RichTextItemDelegate.h \
    src/widgets/SchemaElemsTable.h \
    src/widgets/SchemaLayout.h \
    src/widgets/SchemaLayoutAxicon.h \
    src/widgets/SchemaLayoutCrystal.h \
    src/widgets/SchemaLayoutElems.h \
    src/widgets/SchemaLayoutIfaces.h \
    src/widgets/SchemaLayoutLens.h \
    src/widgets/SchemaParamsTable.h \
    src/widgets/UnitWidgets.h \
    src/widgets/ValueEditor.h \
    src/widgets/ValuesEditorTS.h \
    src/widgets/WidgetResult.h \
    src/tests/TestSchemaListener.h \
    src/GaussCalculatorWindow.h \
    src/widgets/VariableRangeEditor.h \
    src/funcs_window/CausticOptionsPanel.h \
    src/funcs_window/FuncOptionsPanel.h \
    src/StartWindow.h \
    src/CommonData.h \
    src/AppSettingsDialog.h \
    src/SchemaPropsDialog.h \
    src/io/JsonUtils.h \
    src/io/CommonUtils.h \
    src/io/Clipboard.h \
    src/core/Utils.h \
    src/widgets/PlotUtils.h \
    src/funcs/BeamVariationFunction.h \
    src/funcs_window/BeamVariationWindow.h \
    src/funcs/AbcdBeamCalculator.h \
    src/widgets/ParamsTreeWidget.h
	
SOURCES += \
    src/AdjustmentWindow.cpp \
    src/AppSettings.cpp \
    src/CalcManager.cpp \
    src/CalculatorWindow.cpp \
    src/CustomElemsManager.cpp \
    src/CustomElemsWindow.cpp \
    src/CustomPrefs.cpp \
    src/ElemFormulaWindow.cpp \
    src/ElementPropsDialog.cpp \
    src/ElementsCatalogDialog.cpp \
    src/GrinLensWindow.cpp \
    src/HelpSystem.cpp \
    src/MemoWindow.cpp \
    src/MessageBus.cpp \
    src/ProjectOperations.cpp \
    src/ProjectWindow.cpp \
    src/ProtocolWindow.cpp \
    src/PumpParamsDialog.cpp \
    src/PumpWindow.cpp \
    src/RezonatorDialog.cpp \
    src/SchemaParamsWindow.cpp \
    src/SchemaViewWindow.cpp \
    src/SchemaWindows.cpp \
    src/WindowsManager.cpp \
    src/core/Beam.cpp \
    src/core/CommonTypes.cpp \
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
    src/core/Protocol.cpp \
    src/core/Pump.cpp \
    src/core/Report.cpp \
    src/core/Schema.cpp \
    src/core/Units.cpp \
    src/core/Values.cpp \
    src/core/Variable.cpp \
    src/funcs/BeamParamsAtElemsFunction.cpp \
    src/funcs/CausticFunction.cpp \
    src/funcs/FormatInfo.cpp \
    src/funcs/FunctionBase.cpp \
    src/funcs/FunctionGraph.cpp \
    src/funcs/FunctionUtils.cpp \
    src/funcs/GaussCalculator.cpp \
    src/funcs/GrinCalculator.cpp \
    src/funcs/InfoFunctions.cpp \
    src/funcs/MultibeamCausticFunction.cpp \
    src/funcs/MultirangeCausticFunction.cpp \
    src/funcs/PlotFuncRoundTripFunction.cpp \
    src/funcs/PlotFunction.cpp \
    src/funcs/PumpCalculator.cpp \
    src/funcs/RoundTripCalculator.cpp \
    src/funcs/StabilityMap2DFunction.cpp \
    src/funcs/StabilityMapFunction.cpp \
    src/funcs/TableFunction.cpp \
    src/funcs_window/BeamShapeExtension.cpp \
    src/funcs_window/CausticWindow.cpp \
    src/funcs_window/FuncWindowHelpers.cpp \
    src/funcs_window/InfoFuncWindow.cpp \
    src/funcs_window/MultiCausticParamsDlg.cpp \
    src/funcs_window/MultibeamCausticWindow.cpp \
    src/funcs_window/MulticausticWindow.cpp \
    src/funcs_window/MultirangeCausticWindow.cpp \
    src/funcs_window/PlotFuncWindow.cpp \
    src/funcs_window/PlotFuncWindowStorable.cpp \
    src/funcs_window/StabilityMap2DWindow.cpp \
    src/funcs_window/StabilityMapWindow.cpp \
    src/funcs_window/TableFuncWindow.cpp \
    src/io/SchemaReaderIni.cpp \
    src/io/SchemaReaderJson.cpp \
    src/io/SchemaWriterJson.cpp \
    src/main.cpp \
    src/tests/test_AbcdBeamCalculator.cpp \
    src/tests/test_ElemSelectorWidget.cpp \
    src/tests/test_Element.cpp \
    src/tests/test_ElementFilter.cpp \
    src/tests/test_ElementFormula.cpp \
    src/tests/test_Elements.cpp \
    src/tests/test_ElementsImages.cpp \
    src/tests/test_GrinCalculator.cpp \
    src/tests/test_InfoFunctions.cpp \
    src/tests/test_LuaHelper.cpp \
    src/tests/test_Math.cpp \
    src/tests/test_ParamEditor.cpp \
    src/tests/test_Parameters.cpp \
    src/tests/test_ParamsEditor.cpp \
    src/tests/test_PlotFunctions.cpp \
    src/tests/test_ProjectOperations.cpp \
    src/tests/test_Report.cpp \
    src/tests/test_RoundTripCalculator.cpp \
    src/tests/test_Schema.cpp \
    src/tests/test_SchemaReaderIni.cpp \
    src/tests/test_TableFunction.cpp \
    src/tests/test_TestUtils.cpp \
    src/tests/test_UnitWidgets.cpp \
    src/tests/test_Units.cpp \
    src/tests/test_Values.cpp \
    src/Appearance.cpp \
    src/widgets/ElemFormulaEditor.cpp \
    src/widgets/ElemSelectorWidget.cpp \
    src/widgets/ElementTypesListView.cpp \
    src/widgets/ElementsTable.cpp \
    src/widgets/FormulaEditor.cpp \
    src/widgets/FrozenStateButton.cpp \
    src/widgets/ParamEditor.cpp \
    src/widgets/ParamEditorEx.cpp \
    src/widgets/ParamsEditor.cpp \
    src/widgets/ParamsListWidget.cpp \
    src/widgets/PlotHelpers.cpp \
    src/widgets/PlotParamsPanel.cpp \
    src/widgets/PopupMessage.cpp \
    src/widgets/RichTextItemDelegate.cpp \
    src/widgets/SchemaElemsTable.cpp \
    src/widgets/SchemaLayout.cpp \
    src/widgets/SchemaParamsTable.cpp \
    src/widgets/UnitWidgets.cpp \
    src/widgets/ValueEditor.cpp \
    src/widgets/ValuesEditorTS.cpp \
    src/widgets/WidgetResult.cpp \
    src/tests/test_GaussCalculator.cpp \
    src/tests/test_PumpCalculator.cpp \
    src/tests/test_SchemaReaderJson.cpp \
    src/tests/test_PumpWindow.cpp \
    src/GaussCalculatorWindow.cpp \
    src/widgets/VariableRangeEditor.cpp \
    src/funcs_window/FuncOptionsPanel.cpp \
    src/io/ISchemaWindowStorable.cpp \
    src/StartWindow.cpp \
    src/CommonData.cpp \
    src/AppSettingsDialog.cpp \
    src/SchemaPropsDialog.cpp \
    src/io/JsonUtils.cpp \
    src/io/CommonUtils.cpp \
    src/io/Clipboard.cpp \
    src/core/Utils.cpp \
    src/widgets/PlotUtils.cpp \
    src/funcs/BeamVariationFunction.cpp \
    src/funcs_window/BeamVariationWindow.cpp \
    src/funcs/AbcdBeamCalculator.cpp \
    src/widgets/ParamsTreeWidget.cpp

DISTFILES += \
    release/history.json \
    src/StartWindow.qss \
    tips/tips.json
