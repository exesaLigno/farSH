#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <pwd.h>

#include "buffer.cpp"

class Greeting
{
private:
    static constexpr int hostNameBufferSize = 1000;
    static constexpr int workDirBufferSize = 4000;

    static const char* GetUserName()
    {
        return getlogin();
    }

    static const char* GetHostName()
    {
        static char hostname[hostNameBufferSize] = { 0 };
        gethostname(hostname, hostNameBufferSize);
        hostname[hostNameBufferSize - 1] = 0;
        return hostname;
    }

    static const uid_t GetUid()
    {
        return getuid();
    }

    static const passwd* GetPwuid()
    {
        return getpwuid(GetUid());
    }

    static const char* GetHomeDir()
    {
        return GetPwuid()->pw_dir;
    }

    static const char* GetWorkDir(bool shorten_home_dir = false, bool shorten_parential_dir_path = false, bool truncate_parential_dir_path = false)
    {
        static char workdir[workDirBufferSize] = { 0 };
        getcwd(workdir, workDirBufferSize);
        workdir[workDirBufferSize - 1] = 0;
        return workdir;
    }

    static const char* GetInfo(const char* const entry_name, size_t entry_name_size)
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

public:
    char scheme[1000] = "$> ";

    Greeting() { }
    Greeting(const char* _scheme)
    {
        strcpy(scheme, _scheme);
    }

    void WriteTo(UnicodeBuffer& buffer)
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

    // void WriteTo(char* buffer)
    // {
    //     char* buffer_ptr = buffer;

    //     const char* scheme_ptr = scheme;

    //     while (*scheme_ptr != 0)
    //     {
    //         switch (*scheme_ptr)
    //         {
    //             case '{':
    //                 Substitute(&scheme_ptr, &buffer_ptr);
    //                 break;
    //             default:
    //                 *(buffer_ptr++) = *(scheme_ptr++);
    //                 break;
    //         }
    //     }
    // }
};
