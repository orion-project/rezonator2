#include "CommonData.h"
#include "CalculatorWindow.h"
#include "GaussCalculatorWindow.h"
#include "ProjectOperations.h"
#include "ProjectWindow.h"
#include "StartWindow.h"
#include "core/Format.h"
#include "tests/TestSuite.h"

#include "helpers/OriTools.h"
#include "tools/OriDebug.h"
#include "testing/OriTestManager.h"

#include <QApplication>
#include <QFileInfo>
#include <QCommandLineParser>
#include <QMessageBox>

#ifndef Q_OS_WIN
#include <iostream>
#endif

int main(int argc, char* argv[])
{
    qsrand(static_cast<uint>(std::time(nullptr)));

    QApplication app(argc, argv);
    app.setApplicationName("reZonator");
    app.setOrganizationName("orion-project.org");
    app.setApplicationVersion(Z::Strs::appVersion());
    app.setStyle("fusion");

    auto p = app.palette();
    p.setColor(QPalette::AlternateBase, Ori::Color::blend(p.color(QPalette::Button), p.color(QPalette::Highlight), 0.02));
    app.setPalette(p);

    QCommandLineParser parser;
    auto optionHelp = parser.addHelpOption();
    auto optionVersion = parser.addVersionOption();
    QCommandLineOption optionTest("test", "Run unit-test session.");
    QCommandLineOption optionTool("tool", "Run a tool: gauss, calc", "name");
    QCommandLineOption optionDevMode("dev"); optionDevMode.setHidden(true);
    QCommandLineOption optionConsole("console"); optionConsole.setHidden(true);
    QCommandLineOption optionExample("example"); optionExample.setHidden(true);
    parser.addOptions({optionTest, optionTool, optionDevMode, optionConsole, optionExample});

    if (!parser.parse(QApplication::arguments()))
    {
#ifdef Q_OS_WIN
        QMessageBox::critical(nullptr, app.applicationName(), parser.errorText());
#else
        std::cerr << qPrintable(parser.errorText()) << std::endl;
#endif
        return 1;
    }

    // These will quite the app
    if (parser.isSet(optionHelp))
        parser.showHelp();
    if (parser.isSet(optionVersion))
        parser.showVersion();

    // It's only useful on Windows where there is no
    // direct way to use the console for GUI applications.
    if (parser.isSet(optionConsole))
        Ori::Debug::installMessageHandler();

    // Run test session if requested
    if (parser.isSet(optionTest))
        return Ori::Testing::run(app, { ADD_SUITE(Z::Tests) });

    // Load application settings before any command start
    AppSettings::instance().load();
    AppSettings::instance().isDevMode = parser.isSet(optionDevMode);

    // CommonData will be used via its instance pointer
    CommonData commonData;

    // Run a tool if requested
    if (parser.isSet(optionTool))
    {
        auto toolName = parser.value(optionTool);
        if (toolName == "gauss")
        {
            GaussCalculatorWindow::showWindow();
            return app.exec();
        }
        else if (toolName == "calc")
        {
            CalculatorWindow::showWindow();
            return app.exec();
        }
        else
        {
        #ifdef Q_OS_WIN
            QMessageBox::critical(nullptr, app.applicationName(), "Unknown tool name: " + toolName);
        #else
            std::cerr << "Unknown tool name: " << qPrintable(toolName) << std::endl;
        #endif
            return 1;
        }
    }

    // Open a file if given
    auto args = parser.positionalArguments();
    if (!args.isEmpty())
    {
        auto fileName = args.first();
        if (QFileInfo(fileName).exists())
        {
            auto projectWindow = new ProjectWindow(new Schema());
            projectWindow->show();
            if (parser.isSet(optionExample))
                projectWindow->operations()->openExampleFile(fileName);
            else
            {
                projectWindow->operations()->openSchemaFile(fileName);
                CommonData::instance()->addFileToMruList(fileName);
            }
            return app.exec();
        }
    }

    // Open empty project window if start window is disabled
    if (!AppSettings::instance().showStartWindow)
    {
        auto tripType = TripTypes::find(AppSettings::instance().defaultTripType);
        auto schema = ProjectOperations::createDefaultSchema(tripType);
        auto window = new ProjectWindow(schema);
        window->show();
        return app.exec();
    }

    // Start normally
    (new StartWindow)->show();
    return app.exec();
}
