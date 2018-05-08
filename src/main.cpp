#include "ProjectWindow.h"
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

    ProjectWindow pw;
    pw.show();

    return app.exec();
}
