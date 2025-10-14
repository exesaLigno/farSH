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

void Greeting::CheckAndReplaceHomeDir(char workdir[])
{
    #define IsSepSymbol(sym) (sym == '/' or sym == '\\' or sym == '\0')

    const char* homedir = GetHomeDir();
    size_t homedir_len = strlen(homedir);
    size_t workdir_len = strlen(workdir);
    
    if (homedir_len > workdir_len)
        return;

    bool replace = true;
    size_t replace_until_idx = 0;

    for (replace_until_idx = 0; replace_until_idx < homedir_len; replace_until_idx++)
        if (homedir[replace_until_idx] != workdir[replace_until_idx])
        {
            replace = false;
            break;
        }

    if (replace and IsSepSymbol(workdir[replace_until_idx]))
    {
        workdir[0] = '~';

        size_t destination_idx = 1;
        for (size_t source_idx = replace_until_idx; source_idx < workdir_len; source_idx++, destination_idx++)
            workdir[destination_idx] = workdir[source_idx];

        workdir[destination_idx] = '\0';
    }

    #undef IsSepSymbol
}

void Greeting::ShortenParentDirectoryPath(char workdir[])
{
    
}

const char* Greeting::GetWorkDir(bool shorten_home_dir, bool shorten_parential_dir_path, bool truncate_parential_dir_path)
{
    static char workdir[workDirBufferSize] = { 0 };
    getcwd(workdir, workDirBufferSize);
    workdir[workDirBufferSize - 1] = 0;

    if (shorten_home_dir)
        CheckAndReplaceHomeDir(workdir);

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
