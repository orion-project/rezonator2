QT += core gui widgets printsupport

#------------------------------------------------------------
# Definition of output

TARGET = rezonator
TEMPLATE = app
DESTDIR = $$_PRO_FILE_PWD_/bin

#------------------------------------------------------------
# Submodules

# orion
ORION = $$_PRO_FILE_PWD_/orion
include($$ORION/orion.pri)
include($$ORION/orion_testing.pri)
include($$ORION/orion_tests.pri)
include($$ORION/orion_svg.pri)

# lua
LUA = $$_PRO_FILE_PWD_/libs/lua-5.3.4
INCLUDEPATH += $$LUA/src
LIBS += -L$$LUA/src -llua
win32 {
}
else {
# `dl` is required for Lua
LIBS += -ldl
}

#------------------------------------------------------------
# Version information

include(release/version.pri)
DEFINES += "APP_VER_MAJOR=$$APP_VER_MAJOR"
DEFINES += "APP_VER_MINOR=$$APP_VER_MINOR"
DEFINES += "APP_VER_PATCH=$$APP_VER_PATCH"
DEFINES += "APP_VER_CODENAME=\"\\\"$$APP_VER_CODENAME\\\"\""

win32 {
    DEFINES += "BUILDDATE=\"\\\"$$system(date /T)\\\"\""
    DEFINES += "BUILDTIME=\"\\\"$$system(time /T)\\\"\""
}
else {
    DEFINES += "BUILDDATE=\"\\\"$$system(date '+%F')\\\"\""
    DEFINES += "BUILDTIME=\"\\\"$$system(date '+%T')\\\"\""
}

win32: RC_FILE = release/version.rc

#------------------------------------------------------------
# Translation

#TRANSLATIONS = $$DESTDIR/languages/rezonator_ru.ts

#------------------------------------------------------------
# Sources

RESOURCES += \
    src/images.qrc

HEADERS += \
    libs/qcustomplot/qcpcursor.h \
    libs/qcustomplot/qcustomplot.h \
    src/AppSettings.h \
    src/CalcManager.h \
    src/CustomPrefs.h \
    src/ElementPropsDialog.h \
    src/ElementsCatalogDialog.h \
    src/HelpSystem.h \
    src/ProjectOperations.h \
    src/ProjectWindow.h \
    src/ProtocolWindow.h \
    src/PumpParamsDialog.h \
    src/PumpWindow.h \
    src/RezonatorDialog.h \
    src/SchemaParamsWindow.h \
    src/SchemaViewWindow.h \
    src/SchemaWindows.h \
    src/VariableDialog.h \
    src/WindowsManager.h \
    src/core/CommonTypes.h \
    src/core/Element.h \
    src/core/ElementFilter.h \
    src/core/Elements.h \
    src/core/ElementsCatalog.h \
    src/core/Format.h \
    src/core/Formula.h \
    src/core/Math.h \
    src/core/Parameters.h \
    src/core/Protocol.h \
    src/core/Pump.h \
    src/core/Report.h \
    src/core/Schema.h \
    src/core/SchemaClient.h \
    src/core/Units.h \
    src/core/Values.h \
    src/core/Variable.h \
    src/funcs/CausticFunction.h \
    src/funcs/FormatInfo.h \
    src/funcs/FunctionBase.h \
    src/funcs/GaussCalculator.h \
    src/funcs/InfoFunctions.h \
    src/funcs/PlotFunction.h \
    src/funcs/PumpCalculator.h \
    src/funcs/RoundTripCalculator.h \
    src/funcs/StabilityMap2DFunction.h \
    src/funcs/StabilityMapFunction.h \
    src/funcs_window/CausticWindow.h \
    src/funcs_window/InfoFuncWindow.h \
    src/funcs_window/PlotFuncWindow.h \
    src/funcs_window/PlotFuncWindowStorable.h \
    src/funcs_window/StabilityMap2DWindow.h \
    src/funcs_window/StabilityMapWindow.h \
    src/io/ISchemaWindowStorable.h \
    src/io/SchemaReaderIni.h \
    src/io/SchemaReaderJson.h \
    src/io/SchemaWriterJson.h \
    src/io/z_io_json.h \
    src/io/z_io_utils.h \
    src/tests/TestSuite.h \
    src/tests/TestUtils.h \
    src/widgets/Appearance.h \
    src/widgets/CursorPanel.h \
    src/widgets/ElemSelectorWidget.h \
    src/widgets/ElementImagesProvider.h \
    src/widgets/ElementTypesListView.h \
    src/widgets/FormulaEditor.h \
    src/widgets/FrozenStateButton.h \
    src/widgets/GraphDataGrid.h \
    src/widgets/ParamEditor.h \
    src/widgets/ParamEditorEx.h \
    src/widgets/ParamsEditor.h \
    src/widgets/ParamsListWidget.h \
    src/widgets/Plot.h \
    src/widgets/PlotParamsPanel.h \
    src/widgets/RichTextItemDelegate.h \
    src/widgets/SchemaElemsTable.h \
    src/widgets/SchemaLayout.h \
    src/widgets/SchemaParamsTable.h \
    src/widgets/UnitWidgets.h \
    src/widgets/ValueEditor.h \
    src/widgets/ValuesEditorTS.h \
    src/widgets/VariableEditor.h \
    src/widgets/WidgetResult.h \
    src/tests/TestSchemaListener.h \
    src/GaussCalculatorWindow.h \
    src/funcs_window/MultiCausticWindow.h \
    src/funcs/MultiCausticFunction.h \
    src/widgets/VariableRangeEditor.h \
    src/funcs_window/CausticOptionsPanel.h \
    src/funcs_window/FuncOptionsPanel.h \
    src/StartWindow.h \
    src/CommonData.h \
    src/AppSettingsDialog.h \
    src/SchemaPropsDialog.h
	
SOURCES += \
    libs/qcustomplot/qcpcursor.cpp \
    libs/qcustomplot/qcustomplot.cpp \
    src/AppSettings.cpp \
    src/CalcManager.cpp \
    src/CustomPrefs.cpp \
    src/ElementPropsDialog.cpp \
    src/ElementsCatalogDialog.cpp \
    src/HelpSystem.cpp \
    src/ProjectOperations.cpp \
    src/ProjectWindow.cpp \
    src/ProtocolWindow.cpp \
    src/PumpParamsDialog.cpp \
    src/PumpWindow.cpp \
    src/RezonatorDialog.cpp \
    src/SchemaParamsWindow.cpp \
    src/SchemaViewWindow.cpp \
    src/SchemaWindows.cpp \
    src/VariableDialog.cpp \
    src/WindowsManager.cpp \
    src/core/CommonTypes.cpp \
    src/core/Element.cpp \
    src/core/ElementFilter.cpp \
    src/core/Elements.cpp \
    src/core/ElementsCatalog.cpp \
    src/core/Formula.cpp \
    src/core/Math.cpp \
    src/core/Parameters.cpp \
    src/core/Protocol.cpp \
    src/core/Pump.cpp \
    src/core/Report.cpp \
    src/core/Schema.cpp \
    src/core/Units.cpp \
    src/core/Values.cpp \
    src/core/Variable.cpp \
    src/funcs/CausticFunction.cpp \
    src/funcs/FormatInfo.cpp \
    src/funcs/FunctionBase.cpp \
    src/funcs/GaussCalculator.cpp \
    src/funcs/InfoFunctions.cpp \
    src/funcs/PlotFunction.cpp \
    src/funcs/PumpCalculator.cpp \
    src/funcs/RoundTripCalculator.cpp \
    src/funcs/StabilityMap2DFunction.cpp \
    src/funcs/StabilityMapFunction.cpp \
    src/funcs_window/CausticWindow.cpp \
    src/funcs_window/InfoFuncWindow.cpp \
    src/funcs_window/PlotFuncWindow.cpp \
    src/funcs_window/PlotFuncWindowStorable.cpp \
    src/funcs_window/StabilityMap2DWindow.cpp \
    src/funcs_window/StabilityMapWindow.cpp \
    src/io/SchemaReaderIni.cpp \
    src/io/SchemaReaderJson.cpp \
    src/io/SchemaWriterJson.cpp \
    src/io/z_io_json.cpp \
    src/io/z_io_utils.cpp \
    src/main.cpp \
    src/tests/test_ElemSelectorWidget.cpp \
    src/tests/test_Element.cpp \
    src/tests/test_ElementFilter.cpp \
    src/tests/test_Elements.cpp \
    src/tests/test_ElementsImages.cpp \
    src/tests/test_InfoFunctions.cpp \
    src/tests/test_Math.cpp \
    src/tests/test_Parameters.cpp \
    src/tests/test_Report.cpp \
    src/tests/test_RoundTripCalculator.cpp \
    src/tests/test_Schema.cpp \
    src/tests/test_SchemaReaderIni.cpp \
    src/tests/test_Units.cpp \
    src/tests/test_Values.cpp \
    src/widgets/Appearance.cpp \
    src/widgets/CursorPanel.cpp \
    src/widgets/ElemSelectorWidget.cpp \
    src/widgets/ElementTypesListView.cpp \
    src/widgets/FormulaEditor.cpp \
    src/widgets/FrozenStateButton.cpp \
    src/widgets/GraphDataGrid.cpp \
    src/widgets/ParamEditor.cpp \
    src/widgets/ParamEditorEx.cpp \
    src/widgets/ParamsEditor.cpp \
    src/widgets/ParamsListWidget.cpp \
    src/widgets/Plot.cpp \
    src/widgets/PlotParamsPanel.cpp \
    src/widgets/RichTextItemDelegate.cpp \
    src/widgets/SchemaElemsTable.cpp \
    src/widgets/SchemaLayout.cpp \
    src/widgets/SchemaParamsTable.cpp \
    src/widgets/UnitWidgets.cpp \
    src/widgets/ValueEditor.cpp \
    src/widgets/ValuesEditorTS.cpp \
    src/widgets/VariableEditor.cpp \
    src/widgets/WidgetResult.cpp \
    src/tests/test_GaussCalculator.cpp \
    src/tests/test_PumpCalculator.cpp \
    src/tests/test_SchemaReaderJson.cpp \
    src/tests/test_PumpWindow.cpp \
    src/GaussCalculatorWindow.cpp \
    src/funcs_window/MultiCausticWindow.cpp \
    src/funcs/MultiCausticFunction.cpp \
    src/widgets/VariableRangeEditor.cpp \
    src/funcs_window/FuncOptionsPanel.cpp \
    src/io/ISchemaWindowStorable.cpp \
    src/StartWindow.cpp \
    src/CommonData.cpp \
    src/AppSettingsDialog.cpp \
    src/SchemaPropsDialog.cpp

DISTFILES += \
    src/StartWindow.qss \
    src/tips.json
