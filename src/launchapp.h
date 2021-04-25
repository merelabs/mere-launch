#ifndef LAUNCHERAPP_H
#define LAUNCHERAPP_H

#include <QCoreApplication>

class LaunchApp : public QCoreApplication
{
    Q_OBJECT
public:
    explicit LaunchApp(int &argc, char **argv);

    int init();
    int start();

private:
    std::string m_path;
};

#endif // LAUNCHERAPP_H
