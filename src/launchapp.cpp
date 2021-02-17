#include "launchapp.h"
#include "desktoplauncher.h"

#include <QCommandLineParser>

LaunchApp::LaunchApp(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
    QCommandLineParser parser;

//    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("<file>", QCoreApplication::translate("main", "The file to launch of following types \n- desktop entry (.desktop)"));

    parser.process(*this);

    const QStringList arguments = parser.positionalArguments();
    if (arguments.isEmpty())
        parser.showHelp(0);

    m_path = arguments.at(0);
}

int LaunchApp::init()
{
    return 0;
}

int LaunchApp::start()
{
    Mere::Launch::DesktopLauncher launcher(m_path.toStdString());

    int err = launcher.launch();

    return err;
}
