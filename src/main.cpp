#include <QtGlobal>

#ifdef Q_OS_WASM
#include "onlineapplication.h"
#else
#include "desktopapplication.h"
#endif


int main(int argc, char *argv[])
{
    AbstractApplication::initialize();
    QGuiApplication qapp(argc, argv);

#ifdef Q_OS_WASM
    OnlineApplication app(&qapp);
#else
    DesktopApplication app(&qapp);
#endif

    app.initialize();
    return app.run();

}
