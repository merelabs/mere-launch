#include "desktoplauncher.h"

#include "mere/utils/stringutils.h"
#include "mere/xdg/desktopentry.h"
#include "mere/xdg/desktopentryspec.h"
#include "mere/xdg/autostartdirectoryspec.h"
#include "mere/xdg/desktopentrydirectoryspec.h"

#include <fstream>
#include <iostream>

#include <QDir>
#include <QFileInfo>
#include <QProcess>

class Mere::Launch::DesktopLauncher::ProcessPrivate : public QProcess
{
    Q_OBJECT
public:
    ProcessPrivate(DesktopLauncher *q, QObject *parent = nullptr)
        : QProcess(parent),
          q_ptr(q)
    {

    }

protected:
    void setupChildProcess() override
    {
    }

private:
    DesktopLauncher *q_ptr;
};

Mere::Launch::DesktopLauncher::DesktopLauncher(const std::string &path, QObject *parent)
    : QObject(parent),
      m_path(path),
      m_process (new ProcessPrivate(this))
{
    init();
}

int Mere::Launch::DesktopLauncher::init()
{
    std::string suffix = m_path.substr(m_path.length() - strlen(".desktop"));
    if (suffix != ".desktop")
    {
        std::cout << "only desktop entry (.desktop) file supported - " << m_path  << std::endl;
        return 1;
    }

    if (m_path[0] == '~')
        m_path = m_path.replace(0, 1, QDir::homePath().toStdString());

    if (m_path[0] != '/')
    {
        // check in application directory
        std::vector<std::string> dirs = Mere::XDG::DesktopEntryDirectorySpec::applicationDirectories();
        std::string path = find(m_path, dirs);

        // check in autostart directory
        if (Mere::Utils::StringUtils::isBlank(path))
        {
            std::vector<std::string> dirs = Mere::XDG::AutostartDirectorySpec::autostartDirectories();
            path = find(m_path, dirs);
        }

        if (Mere::Utils::StringUtils::isBlank(path))
        {
            std::cout << "file not found in application directories, please check file name - " << m_path << std::endl;
            return 2;
        }

        m_path = path;
    }
    else
    {
        if (!std::ifstream(m_path).good());
        {
            std::cout << "file not found - " << m_path  << std::endl;
            return 3;
        }
    }

    m_desktopEntry = Mere::XDG::DesktopEntrySpec::parse(m_path);
    if (!m_desktopEntry.valid())
    {
        std::cout << "not a valid .desktop file - " << m_path  << std::endl;
        return 4;
    }

    return 0;
}

int Mere::Launch::DesktopLauncher::launch()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    m_process->setProcessEnvironment(env);

    QVariant type = m_desktopEntry.get(Mere::XDG::DesktopEntry::Type);
    if (type == QVariant("Application"))
    {
        QVariant path = m_desktopEntry.get(Mere::XDG::DesktopEntry::Path);
        if (path.isValid())
            m_process->setWorkingDirectory(path.toString());
    }

    QVariant exec = m_desktopEntry.get(Mere::XDG::DesktopEntry::Exec);
    QString program = exec.toString();
    if (Utils::StringUtils::isBlank(program))
            return 5;

    QStringList arguments;

    //std::cout << "going to start " << program.toStdString().c_str() << " with " << std::endl;

    bool ok = m_process->startDetached(program, arguments);

    return !ok;
}

std::string Mere::Launch::DesktopLauncher::find(const std::string &file, const std::vector<std::string> &dirs) const
{
    if (Mere::Utils::StringUtils::isBlank(file))
        return "";

    if (dirs.empty())
        return "";

    for(const std::string &dir : dirs)
    {
        std::string path(dir);

        if (path[path.length() - 1] != '/')
            path.append("/");

        path.append(file);

        if(std::ifstream(path).good())
            return path;
    }

    return "";
}

#include "desktoplauncher.moc"
