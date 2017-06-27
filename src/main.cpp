#include "ProjectWindow.h"
#include "test/rezonator_tests.h"
#include "tools/OriDebug.h"
#include "testing/OriTestManager.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    //Ori::Debug::installMessageHandler();

    QApplication app(argc, argv);
    app.setApplicationName("reZonator");
    app.setOrganizationName("orion-project.org");

    if (Ori::Test::isTesting())
        return Ori::Test::run(app, { ADD_SUITE(Z::Test) });
        // TODO: Segmentation fault (core dumped) после остановки приложения
        // если оба набора Ori и Z добавлены, даже если тесты не запускались
        // если только один из них добавлен (любой), то ошибки нет

    ProjectWindow pw;
    pw.show();

    return app.exec();
}
