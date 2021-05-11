#include "desktoplauncher.h"

#include "mere/utils/stringutils.h"
#include "mere/utils/pathutils.h"
#include "mere/utils/fileutils.h"

#include "mere/xdg/desktopentry.h"
#include "mere/xdg/desktopentryparser.h"
#include "mere/xdg/autostartdirectory.h"
#include "mere/xdg/desktopentrydirectory.h"

#include <iostream>
#include <sstream>

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
    m_entry = parser.parse();
    if (!m_entry.valid())
    {
        std::cout << "not a valid .desktop file - " << realpath  << std::endl;
        return 3;
    }

    return 0;
}

std::string Mere::Launch::DesktopLauncher::realpath(const std::string &path)
{
    int resolved;
    std::string realpath = Mere::Utils::PathUtils::resolve(m_path, &resolved);
    if (resolved) return realpath;

    if (path.front() == '/' || path.front() == '.' )
        return "";

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

    return realpath;
}

int Mere::Launch::DesktopLauncher::launch()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    m_process->setProcessEnvironment(env);

    Mere::XDG::DesktopEntry::TypeId type = m_entry.typeId();
    if (type == Mere::XDG::DesktopEntry::TypeId::Application)
    {
        std::string path = m_entry.path();
        if (!path.empty())
            m_process->setWorkingDirectory(path.c_str());
    }

    std::string command = m_entry.exec();
    if (command.empty()) return 1;

    std::vector<std::string> parts = Mere::Utils::StringUtils::split(command);
    unsigned int size = parts.size();
    if (size == 0) return 1;

    m_process->setProgram(parts.at(0).c_str());

    if (size > 1)
    {
        QStringList arguments;
        for(unsigned int i = 1; i < size; i++)
            arguments << parts.at(i).c_str();
        m_process->setArguments(arguments);
    }

    qint64 pid;
    m_process->startDetached(&pid);

    return 0;
}

std::string Mere::Launch::DesktopLauncher::find(const std::string &file, const std::vector<std::string> &dirs) const
{
    if (Mere::Utils::StringUtils::isBlank(file) || dirs.empty())
        return "";

    for(const std::string &dir : dirs)
    {
        std::string path(dir);
        path.append(file);

        if(Mere::Utils::FileUtils::isExist(path))
            return path;
    }

    return "";
}

#include "desktoplauncher.moc"
