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
    explicit DesktopLauncher(const QString &path, QObject *parent = nullptr);

    int launch();

private:
    int init();
    int start();

private:
    QString m_path;
    Mere::XDG::DesktopEntry m_desktopEntry;

    class ProcessPrivate;
    ProcessPrivate *m_process;
};

}
}

#endif // MERE_LAUNCH_DESKTOP_LAUNCHER_H
