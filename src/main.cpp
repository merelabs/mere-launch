#include "launchapp.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("mere-launch");
    QCoreApplication::setApplicationVersion("0.0.1b");

    LaunchApp app(argc, argv);

    int err = app.init();
    if (err) ::exit(1);

    err = app.start();
    ::exit(err);

    //return app.exec();
}
