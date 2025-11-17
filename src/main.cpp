#include "app/Appearance.h"
#include "core/Format.h"
#include "core/Protocol.h"
#include "tests/TestSuite.h"
#include "tools/CalculatorWindow.h"
#include "tools/GaussCalculatorWindow.h"
#include "tools/GrinLensWindow.h"
#include "tools/LensmakerWindow.h"
#include "windows/CustomElemsWindow.h"
#include "windows/CustomFuncsWindow.h"
#include "windows/ProjectWindow.h"
#include "windows/StartWindow.h"

#include "qcpl_colors.h"

#include "helpers/OriTheme.h"
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
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
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
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    QCommandLineOption optionTest("test", "Run unit-test session.");
    QCommandLineOption optionTool("tool", "Run a tool: gauss, calc, elems, funcs, grin, lens", "name");
    QCommandLineOption optionDevMode("dev"); optionDevMode.setFlags(QCommandLineOption::HiddenFromHelp);
    QCommandLineOption optionConsole("console"); optionConsole.setFlags(QCommandLineOption::HiddenFromHelp);
    QCommandLineOption optionExample("example"); optionExample.setFlags(QCommandLineOption::HiddenFromHelp);
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
    AppSettings::instance().isDevMode = parser.isSet(optionDevMode);
    
    Z::Protocol::isDebugEnabled = parser.isSet(optionDevMode);

    // Call `setStyleSheet` after setting loaded
    // to be able to apply custom colors (if they are).
    app.setStyleSheet(Ori::Theme::makeStyleSheet(Ori::Theme::loadRawStyleSheet()));
    Ori::Theme::setColors({
        { Ori::Theme::SelectionColor, Z::Gui::selectionColor() },
        { Ori::Theme::PaperColor, Z::Gui::paperColor() },
    });
    QCPL::setGuiColors({
        { QCPL::HintLabelColor, Z::Gui::mutedTextColor() },
    });

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
        else if (toolName == "elems")
        {
            CustomElemsWindow::showWindow();
            return app.exec();
        }
        else if (toolName == "funcs")
        {
            CustomFuncsWindow::showWindow();
            return app.exec();
        }
        else if (toolName == "grin")
        {
            GrinLensWindow::showWindow();
            return app.exec();
        }
        else if (toolName == "lens")
        {
            LensmakerWindow::showWindow();
            return app.exec();
        }
//        else if (toolName == "iris")
//        {
//            IrisWindow::showWindow();
//            return app.exec();
//        }
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
        if (QFileInfo::exists(fileName))
        {
            ProjectWindow::openProject(fileName, {
                .isExample = parser.isSet(optionExample),
                .addToMru = !parser.isSet(optionExample),
            });
            return app.exec();
        }
    }

    // Open empty project window if start window is disabled
    if (!AppSettings::instance().showStartWindow)
    {
        ProjectWindow::createProject(TripTypes::find(AppSettings::instance().defaultTripType));
        return app.exec();
    }

    // Start normally
    (new StartWindow)->show();
    return app.exec();
}
