#include "greeting.hpp"

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <pwd.h>

const char* Greeting::GetUserName()
{
    return getlogin();
}

const char* Greeting::GetHostName()
{
    static char hostname[hostNameBufferSize] = { 0 };
    gethostname(hostname, hostNameBufferSize);
    hostname[hostNameBufferSize - 1] = 0;
    return hostname;
}

const uid_t Greeting::GetUid()
{
    return getuid();
}

const passwd* Greeting::GetPwuid()
{
    return getpwuid(GetUid());
}

const char* Greeting::GetHomeDir()
{
    return GetPwuid()->pw_dir;
}

const char* Greeting::GetWorkDir(bool shorten_home_dir, bool shorten_parential_dir_path, bool truncate_parential_dir_path)
{
    static char workdir[workDirBufferSize] = { 0 };
    getcwd(workdir, workDirBufferSize);
    workdir[workDirBufferSize - 1] = 0;
    return workdir;
}

const char* Greeting::GetInfo(const char* const entry_name, size_t entry_name_size)
{
    if (!strncmp(entry_name, "username", entry_name_size))
        return GetUserName();
    else if (!strncmp(entry_name, "hostname", entry_name_size))
        return GetHostName();
    else if (!strncmp(entry_name, "cwd", entry_name_size))
        return GetWorkDir();
    else
        return 0;
}

Greeting::Greeting() { }
Greeting::Greeting(const char* _scheme)
{
    strcpy(scheme, _scheme);
}

void Greeting::WriteTo(UnicodeBuffer& buffer)
{
    const char* scheme_ptr = scheme;
    const char* substr_start = scheme_ptr;

    // enum class State
    // {
    //     PlainText, EntityName
    // };

    // State state = State::PlainText;

    for (const char* scheme_ptr = scheme; *scheme_ptr != 0; scheme_ptr++)
    {
        if (*scheme_ptr == '{')
        {
            size_t size = scheme_ptr - substr_start;
            if (size > 0)
                buffer.Insert(substr_start, size);
            substr_start = scheme_ptr + 1;
            continue;
        }

        if (*scheme_ptr == '}')
        {
            buffer.Insert(GetInfo(substr_start, scheme_ptr - substr_start));
            substr_start = scheme_ptr + 1;
        }
    }

    buffer.Insert(substr_start, scheme_ptr - substr_start);
}
