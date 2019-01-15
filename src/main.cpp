#include "GaussCalculatorWindow.h"
#include "ProjectWindow.h"
#include "StartWindow.h"
#include "tests/TestSuite.h"
#include "tools/OriDebug.h"
#include "testing/OriTestManager.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    //Ori::Debug::installMessageHandler();

    QApplication app(argc, argv);
    app.setApplicationName("reZonator");
    app.setOrganizationName("orion-project.org");

    if (Ori::Testing::isTesting())
        return Ori::Testing::run(app, { ADD_SUITE(Z::Tests) });

    int res = 0;

    Settings::instance().load();

    if (QApplication::arguments().contains("gauss"))
    {
        GaussCalculatorWindow::showCalcWindow();
        res = app.exec();
    }
    else
    {
        (new StartWindow)->show();

        //ProjectWindow projWindow;
        //projWindow.show();

        res = app.exec();
    }

    return res;
}
