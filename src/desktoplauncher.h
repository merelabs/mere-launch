#ifndef MERE_LAUNCH_DESKTOP_LAUNCHER_H
#define MERE_LAUNCH_DESKTOP_LAUNCHER_H

#include "mere/xdg/desktopentry.h"
#include <QObject>

namespace Mere
{
namespace Launch
{

class DesktopLauncher : public QObject
{
    Q_OBJECT
public:
    explicit DesktopLauncher(const std::string &path, QObject *parent = nullptr);

    int launch();

private:
    int init();
    int start();
    std::string realpath(const std::string &path);

    std::string find(const std::string &file, const std::vector<std::string> &dirs) const;

private:
    std::string m_path;
    Mere::XDG::DesktopEntry m_entry;

    class ProcessPrivate;
    ProcessPrivate *m_process;
};

}
}

#endif // MERE_LAUNCH_DESKTOP_LAUNCHER_H
