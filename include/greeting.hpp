#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <pwd.h>

#include "unicode/buffer.hpp"

class Greeting
{
private:
    char scheme[1000] = "{cwd} $> ";
    static constexpr int hostNameBufferSize = 1000;
    static constexpr int workDirBufferSize = 4000;

    static const uid_t GetUid();
    static const passwd* GetPwuid();
    static const char* GetInfo(const char* const entry_name, size_t entry_name_size);

    static void CheckAndReplaceHomeDir(char workdir[]);
    static void ShortenParentDirectoryPath(char workdir[]);
    static void TruncateParentDirectoryPath(char workdir[]);

public:
    Greeting();
    Greeting(const char* _scheme);

    static const char* GetUserName();
    static const char* GetHostName();
    static const char* GetHomeDir();
    static const char* GetWorkDir(bool shorten_home_dir = true, bool shorten_parential_dir_path = true, bool truncate_parential_dir_path = true);

    void WriteTo(UnicodeBuffer& buffer);
};
