#include "CommonData.h"
#include "GaussCalculatorWindow.h"
#include "ProjectOperations.h"
#include "ProjectWindow.h"
#include "StartWindow.h"
#include "tests/TestSuite.h"
#include "tools/OriDebug.h"
#include "testing/OriTestManager.h"

#include <QApplication>
#include <QFileInfo>

int main(int argc, char* argv[])
{
    qsrand(static_cast<uint>(std::time(nullptr)));

    //Ori::Debug::installMessageHandler();

    QApplication app(argc, argv);
    app.setApplicationName("reZonator");
    app.setOrganizationName("orion-project.org");

    // Run test session if requested
    if (Ori::Testing::isTesting())
        return Ori::Testing::run(app, { ADD_SUITE(Z::Tests) });

    QStringList args = QApplication::arguments();

    Settings::instance().load();
    Settings::instance().isDevMode = args.contains("dev_mode");

    // CommonData will be used via its instance pointer
    CommonData commonData;

    // Run gauss calculator tool if requested
    if (args.contains("gauss"))
    {
        GaussCalculatorWindow::showCalcWindow();
        return app.exec();
    }

    // Treat given file as example
    bool isExample = false;
    for (int i = 0; i < args.count(); i++)
    {
        if (args.at(i) == "example")
        {
            isExample = true;
            args.removeAt(i);
            break;
        }
    }

    // Open a file if given
    QString fileName;
    for (int i = 1; i < args.count(); i++)
        if (QFileInfo(args.at(i)).exists())
        {
            fileName = args.at(i);
            break;
        }
    if (!fileName.isEmpty())
    {
        auto projectWindow = new ProjectWindow(nullptr);
        projectWindow->show();
        if (isExample)
            projectWindow->operations()->openExampleFile(fileName);
        else
            projectWindow->operations()->openSchemaFile(fileName);
        return app.exec();
    }

    // Open empty project window in start window is disabled
    if (!Settings::instance().showStartWindow)
    {
        (new ProjectWindow)->show();
        return app.exec();
    }

    // Start normally
    (new StartWindow)->show();
    return app.exec();
}
