QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = rezonator
TEMPLATE = app

ORION = $$_PRO_FILE_PWD_/orion/
include($$ORION"orion.pri")
include($$ORION"orion_testing.pri")
include($$ORION"orion_tests.pri")
include($$ORION"orion_svg.pri")

DEFINES += "_USE_MATH_DEFINES"

DESTDIR = $$_PRO_FILE_PWD_/bin

DEFINES += "APP_VER_MAJOR=2"
DEFINES += "APP_VER_MINOR=0"
DEFINES += "APP_VER_BUILD=0" # TODO
DEFINES += "SVN_REV=0" # TODO eliminate

win32 {
    DEFINES += "BUILDDATE=\"\\\"$$system(date /T)\\\"\""
    DEFINES += "BUILDTIME=\"\\\"$$system(time /T)\\\"\""
}
else {
    DEFINES += "BUILDDATE=\"\\\"$$system(date '+%F')\\\"\""
    DEFINES += "BUILDTIME=\"\\\"$$system(date '+%T')\\\"\""
}

#TRANSLATIONS = $$DESTDIR/languages/rezonator_ru.ts

RESOURCES += \
    src/images.qrc

HEADERS += \
    libs/qcustomplot/qcpcursor.h \
    libs/qcustomplot/qcustomplot.h \
    src/AppSettings.h \
    src/CalcManager.h \
    src/ConfigDialog.h \
    src/core/Element.h \
    src/core/ElementFilter.h \
    src/core/Elements.h \
    src/core/ElementsCatalog.h \
    src/core/Format.h \
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
    src/ElementPropsDialog.h \
    src/ElementsCatalogDialog.h \
    src/funcs/BeamCalculator.h \
    src/funcs/Calculator.h \
    src/funcs/FormatInfo.h \
    src/funcs/FunctionBase.h \
    src/funcs/InfoFunctions.h \
    src/funcs/PlotFunction.h \
    src/HelpSystem.h \
    src/io/ISchemaStorable.h \
    src/io/SchemaFile.h \
    src/io/SchemaReaderIni.h \
    src/io/SchemaReaderXml.h \
    src/io/SchemaWriterXml.h \
    src/io/z_io_report.h \
    src/io/z_io_utils.h \
    src/io/z_io_xml_reader.h \
    src/io/z_io_xml_writer.h \
    src/ProjectOperations.h \
    src/ProjectWindow.h \
    src/ProtocolWindow.h \
    src/RezonatorDialog.h \
    src/SchemaViewWindow.h \
    src/SchemaWindows.h \
    src/test/rezonator_tests.h \
    src/test/TestSchemaListener.h \
    src/test/TestUtils.h \
    src/VariableDialog.h \
    src/widgets/Appearance.h \
    src/widgets/CursorPanel.h \
    src/widgets/ElementImagesProvider.h \
    src/widgets/ElementTypesListView.h \
    src/widgets/ElemSelectorWidget.h \
    src/widgets/FrozenStateButton.h \
    src/widgets/GraphDataGrid.h \
    src/widgets/LayoutView.h \
    src/widgets/ParamEditor.h \
    src/widgets/Plot.h \
    src/widgets/PlotParamsPanel.h \
    src/widgets/SchemaLayout.h \
    src/widgets/SchemaTable.h \
    src/widgets/UnitWidgets.h \
    src/widgets/ValueEditor.h \
    src/widgets/ValuesEditorTS.h \
    src/widgets/VariableEditor.h \
    src/widgets/VariableRangeWidget.h \
    src/widgets/WidgetResult.h \
    src/WindowsManager.h \
    src/funcs_meat/StabilityMapFunction.h \
    src/funcs_meat/StabilityMap2DFunction.h \
    src/funcs_meat/CausticFunction.h \
    src/funcs_storable/StabilityMapStorable.h \
    src/funcs_window/CausticWindow.h \
    src/funcs_window/InfoFuncWindow.h \
    src/funcs_window/PlotFuncWindow.h \
    src/funcs_window/PlotFuncWindowStorable.h \
    src/funcs_window/StabilityMap2DWindow.h \
    src/funcs_window/StabilityMapWindow.h \
    src/core/CommonTypes.h

SOURCES += \
    libs/qcustomplot/qcpcursor.cpp \
    libs/qcustomplot/qcustomplot.cpp \
    src/AppSettings.cpp \
    src/CalcManager.cpp \
    src/ConfigDialog.cpp \
    src/core/Element.cpp \
    src/core/ElementFilter.cpp \
    src/core/Elements.cpp \
    src/core/ElementsCatalog.cpp \
    src/core/Math.cpp \
    src/core/Parameters.cpp \
    src/core/Protocol.cpp \
    src/core/Pump.cpp \
    src/core/Report.cpp \
    src/core/Schema.cpp \
    src/core/Units.cpp \
    src/core/Values.cpp \
    src/core/Variable.cpp \
    src/ElementPropsDialog.cpp \
    src/ElementsCatalogDialog.cpp \
    src/funcs/BeamCalculator.cpp \
    src/funcs/Calculator.cpp \
    src/funcs/FormatInfo.cpp \
    src/funcs/FunctionBase.cpp \
    src/funcs/InfoFunctions.cpp \
    src/funcs/PlotFunction.cpp \
    src/HelpSystem.cpp \
    src/io/SchemaFile.cpp \
    src/io/SchemaReaderIni.cpp \
    src/io/SchemaReaderXml.cpp \
    src/io/SchemaWriterXml.cpp \
    src/io/z_io_report.cpp \
    src/io/z_io_utils.cpp \
    src/io/z_io_xml_reader.cpp \
    src/io/z_io_xml_writer.cpp \
    src/main.cpp \
    src/ProjectOperations.cpp \
    src/ProjectWindow.cpp \
    src/ProtocolWindow.cpp \
    src/RezonatorDialog.cpp \
    src/SchemaViewWindow.cpp \
    src/SchemaWindows.cpp \
    src/test/test_Calculator.cpp \
    src/test/test_Element.cpp \
    src/test/test_ElementFilter.cpp \
    src/test/test_Elements.cpp \
    src/test/test_ElementsImages.cpp \
    src/test/test_ElemSelectorWidget.cpp \
    src/test/test_InfoFunctions.cpp \
    src/test/test_Math.cpp \
    src/test/test_Parameters.cpp \
    src/test/test_Report.cpp \
    src/test/test_Schema.cpp \
    src/test/test_SchemaFile.cpp \
    src/test/test_SchemaReaderIni.cpp \
    src/test/test_Units.cpp \
    src/test/test_Values.cpp \
    src/test/test_xml_io_general.cpp \
    src/test/test_xml_io_schema.cpp \
    src/VariableDialog.cpp \
    src/widgets/Appearance.cpp \
    src/widgets/CursorPanel.cpp \
    src/widgets/ElementTypesListView.cpp \
    src/widgets/ElemSelectorWidget.cpp \
    src/widgets/FrozenStateButton.cpp \
    src/widgets/GraphDataGrid.cpp \
    src/widgets/LayoutView.cpp \
    src/widgets/ParamEditor.cpp \
    src/widgets/Plot.cpp \
    src/widgets/PlotParamsPanel.cpp \
    src/widgets/SchemaLayout.cpp \
    src/widgets/SchemaTable.cpp \
    src/widgets/UnitWidgets.cpp \
    src/widgets/ValueEditor.cpp \
    src/widgets/ValuesEditorTS.cpp \
    src/widgets/VariableEditor.cpp \
    src/widgets/VariableRangeWidget.cpp \
    src/widgets/WidgetResult.cpp \
    src/WindowsManager.cpp \
    src/funcs_meat/StabilityMapFunction.cpp \
    src/funcs_meat/StabilityMap2DFunction.cpp \
    src/funcs_meat/CausticFunction.cpp \
    src/funcs_window/CausticWindow.cpp \
    src/funcs_window/InfoFuncWindow.cpp \
    src/funcs_window/PlotFuncWindow.cpp \
    src/funcs_window/PlotFuncWindowStorable.cpp \
    src/funcs_window/StabilityMap2DWindow.cpp \
    src/funcs_window/StabilityMapWindow.cpp \
    src/core/CommonTypes.cpp
