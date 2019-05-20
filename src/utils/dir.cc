// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "utils/dir.h"

#include <sys/stat.h>

namespace mpsync {
namespace utils {

bool Dir::Exists(const std::string &path)
{
    struct stat fstat;
    if (stat(path.c_str(), &fstat)) {
        return false;
    }
    return S_ISDIR(fstat.st_mode);
}

bool Dir::Create(const std::string &path)
{
    return mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
}

}  // namespace utils
}  // namespace mpsync