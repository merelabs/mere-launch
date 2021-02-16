#include "desktoplauncher.h"

#include "mere/utils/stringutils.h"
#include "mere/xdg/desktopentry.h"
#include "mere/xdg/desktopentryspec.h"

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

Mere::Launch::DesktopLauncher::DesktopLauncher(const QString &path, QObject *parent)
    : QObject(parent),
      m_path(path),
      m_process (new ProcessPrivate(this))
{
    init();
}

int Mere::Launch::DesktopLauncher::init()
{
    if (m_path.startsWith("~/"))
        m_path = m_path.replace(0, 1, QDir::homePath());

    QFileInfo info(m_path);
    if(!info.isAbsolute())
    {
        std::cout << "please provide absolute path of the file - " << m_path.toStdString() << std::endl;
        return 1;
    }

    if (!info.exists())
    {
        std::cout << "file not found - " << m_path.toStdString()  << std::endl;
        return 2;
    }

    if (info.suffix().compare("desktop") != 0)
    {
        std::cout << "only desktop entry (.desktop) file supported - " << m_path.toStdString()  << std::endl;
        return 3;
    }

    m_desktopEntry = Mere::XDG::DesktopEntrySpec::parse(info.absoluteFilePath());
    if (!m_desktopEntry.valid())
    {
        std::cout << "not a valid .desktop file - " << m_path.toStdString()  << std::endl;
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

#include "desktoplauncher.moc"
