#include "desktoplauncher.h"

#include "mere/utils/stringutils.h"
#include "mere/xdg/desktopentry.h"
#include "mere/xdg/desktopentryparser.h"
#include "mere/xdg/autostartdirectory.h"
#include "mere/xdg/desktopentrydirectory.h"

#include <fstream>
#include <iostream>
#include <limits.h>
#include <stdlib.h>

#include <QDir>
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
    // check for the file extension
    std::string ext(".desktop");
    auto pos = m_path.find(ext);
    if (pos != m_path.length() - ext.length())
    {
        std::cout << "file has no .desktop extension, please check file name - " << m_path << std::endl;
        return 1;
    }

    std::string realpath = this->realpath(m_path);
    if (realpath.empty())
    {
        std::cout << "file not found, please check file name - " << m_path << std::endl;
        return 2;
    }

    Mere::XDG::DesktopEntryParser parser(realpath);
    if(!parser.parse())
    {
        std::cout << "file parsing failed, please check file name - " << realpath << std::endl;
        return 3;
    }

    m_entry = parser.entry();
    if (!m_entry.valid())
    {
        std::cout << "not a valid .desktop file - " << realpath  << std::endl;
        return 4;
    }

    return 0;
}

std::string Mere::Launch::DesktopLauncher::realpath(const std::string &path)
{
    std::string realpath(path);

    char rpath[PATH_MAX];
    if(::realpath(path.c_str(), rpath))
    {
        realpath.replace(0, realpath.length(), rpath);
    }
    else
    {
        realpath = find(path, {QDir::currentPath().append("/").toStdString()});
        if (realpath.empty())
        {
            std::vector<std::string> dirs = Mere::XDG::DesktopEntryDirectory::directories();
            realpath = find(path, dirs);
            if (realpath.empty())
            {
                std::vector<std::string> dirs = Mere::XDG::AutostartDirectory::directories();
                realpath = find(path, dirs);
            }
        }
    }

    return realpath;
}

int Mere::Launch::DesktopLauncher::launch()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    m_process->setProcessEnvironment(env);

    Mere::XDG::DesktopEntry::Type type = m_entry.typeId();
    if (type == Mere::XDG::DesktopEntry::Type::Application)
    {
        std::string path = m_entry.path();
        if (!path.empty())
            m_process->setWorkingDirectory(path.c_str());
    }

    std::string program = m_entry.exec();
    if (program.empty()) return 5;

    bool ok = m_process->startDetached(program.c_str(), {});

    return !ok;
}

std::string Mere::Launch::DesktopLauncher::find(const std::string &file, const std::vector<std::string> &dirs) const
{
    if (Mere::Utils::StringUtils::isBlank(file) || dirs.empty())
        return "";

    for(const std::string &dir : dirs)
    {
        std::string path(dir);
        path.append(file);

        if(std::ifstream(path).good())
            return path;
    }

    return "";
}

#include "desktoplauncher.moc"
