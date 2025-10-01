#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <pwd.h>

class Prompt
{
private:
    size_t StrCopy(char* dst, const char* const src, size_t size = 0)
    {
        size_t idx = 0;

        for (idx = 0; src[idx] != '\0' and (size == 0 or idx < size); idx++)
            dst[idx] = src[idx];

        return idx;
    }

    bool StartsWith(const char* str, const char* substr)
    {
        for (size_t idx = 0; substr[idx] != '\0'; idx++)
        {
            if (str[idx] == '\0' or str[idx] != substr[idx])
                return false;
        }

        return true;
    }

    size_t WriteUserName(char* dst)
    {
        return StrCopy(dst, getlogin());
    }

    size_t WriteHostName(char* dst)
    {
        gethostname(dst, 100);
        return strlen(dst);
    }

    void ProcessCwdString(char* str, bool shorten, bool use_tilde)
    {
        char* str_read = str;
        char* str_write = str;

        passwd* pwd = getpwuid(getuid());

        size_t homedir_len = strlen(pwd->pw_dir);

        if (use_tilde and StartsWith(str_read, pwd->pw_dir))
        {
            str_read += homedir_len;
            *(str_write++) = '~';
        }

        char* last_start = nullptr;

        while (*str_read != '\0')
        {
            if (*str_read == '/')
            {
                if (last_start)
                {
                    if (shorten)
                        *(str_write++) = *last_start;
                    else
                        str_write += StrCopy(str_write, last_start, str_read - last_start);
                }

                *(str_write++) = *(str_read++);
                last_start = str_read;
            }
            else
                str_read++;
        }

        if (last_start)
            str_write += StrCopy(str_write, last_start, str_read - last_start);

        *str_write = '\0';
    }

    size_t WriteCwdShort(char* dst)
    {
        char buffer[1000] = { 0 };
        getcwd(buffer, 1000);
        ProcessCwdString(buffer, true, true);
        return StrCopy(dst, buffer);
    }

    size_t ProcessEntry(char* dst, const char* const entry_name)
    {
        if (!strcmp(entry_name, "username"))
            return WriteUserName(dst);
        else if (!strcmp(entry_name, "hostname"))
            return WriteHostName(dst);
        else if (!strcmp(entry_name, "cwd_short"))
            return WriteCwdShort(dst);
        else
            return 0;
    }

public:
    char scheme[1000] = "\e[1;35m{username}\e[0m\e[1m@\e[1;36m{hostname}\e[0m \e[32m{cwd_short}\e[0m> ";

    void Substitute(const char** scheme_ptr, char** buffer_ptr)
    {
        char entry[1000] = { 0 };
        char* entry_ptr = entry;
        (*scheme_ptr)++;
        
        while (**scheme_ptr != '}')
            *(entry_ptr++) = *((*scheme_ptr)++);
        *entry_ptr = 0;
        (*scheme_ptr)++;

        *buffer_ptr += ProcessEntry(*buffer_ptr, entry);
    }

    void Print(int error = 0)
    {
        char* buffer = new char[1000] { 0 };
        char* buffer_ptr = buffer;

        const char* scheme_ptr = scheme;

        while (*scheme_ptr != 0)
        {
            switch (*scheme_ptr)
            {
                case '{':
                    Substitute(&scheme_ptr, &buffer_ptr);
                    break;
                default:
                    *(buffer_ptr++) = *(scheme_ptr++);
                    break;
            }
        }

        printf("%s\e7", buffer);
    }
};
